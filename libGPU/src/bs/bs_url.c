/**
 * file :	bs_url.c
 * author :	bushaofeng
 * create :	2014-10-06 23:05
 * func : 
 * history:
 */

#include "bs_url.h"
#include "bs_socket.h"

void* url_init(void* p){
    url_t* url = (url_t*)p;
    
    data_init(&url->url);
    data_init(&url->domain);
    data_init(&url->host);
    data_init(&url->path);
    data_init(&url->protocal);
    data_init(&url->query);
    return url;
}
void url_destroy(void* p){
    url_t* url = (url_t*)p;
    
    data_destroy(&url->url);
    data_destroy(&url->domain);
    data_destroy(&url->host);
    data_destroy(&url->path);
    data_destroy(&url->protocal);
    data_destroy(&url->query);
}

void* http_init(void* p){
    http_t* http = (http_t*)p;
    
    url_init(&http->url);
    data_init(&http->req);
    
    return http;
}

void http_destroy(void* p){
    http_t* http = (http_t*)p;
    
    url_destroy(&http->url);
    data_destroy(&http->req);
}

state_t url_parse(url_t* url, const char* url_str){
    int     i;
    char    port[8] = {0};

    if(url==NULL || url_str==NULL){
        return BS_INVALID;
    }

    char* start;
    char* end;

    url->url.set(&url->url, url_str, (uint32_t)strlen(url_str));
    start = (char*)url_str;

    // protocal
    end = strstr(start, "://");
    if(end != NULL){
        url->protocal.set(&url->protocal, start, (uint32_t)(end-start));
        start = end+3;
    }

    // host/port
    end = strchr(start, '/');
    if(end != NULL){
        url->domain.set(&url->domain, start, (uint32_t)(end-start));
        url->host.set(&url->host, start, (uint32_t)(end-start));
        url->port = 80;
        for(i=0; i<end-start; i++){
            if (start[i]==':'){
                url->domain.set(&url->domain, start, i);
                bs_memcpy(port, sizeof(port)-1, start+i+1, (uint32_t)(end-start-i-1));
                url->port = atoi(port);
                break;
            }
        }
        start = end;
    }

    url->path.set(&url->path, start, (uint32_t)strlen(url_str)-(uint32_t)(start - url_str));
    
    // path
    start = strchr(start, '?');
    if (start==NULL){
        return BS_SUCCESS;
    }
    else {
        start += 1;
        url->query.set(&url->query, start, (uint32_t)strlen(url_str)-(uint32_t)(start - url_str));
    }
    
    return BS_SUCCESS;
}

http_t* http_create(const char* url, const char* method){
    char    buffer[URL_SIZE];
    
    http_t* http = bs_new(http);
    url_parse(&http->url, url);
    snprintf(buffer, URL_SIZE, "%s %s HTTP/1.1\r\nAccept: */*\r\nHost: %s\r\n", method, http->url.path.mem, http->url.host.mem);
    data_set(&http->req, buffer, (uint32_t)strlen(buffer));
    
    return http;
}

void http_set_header(http_t* http, const char* key, const char* value){
    char    buffer[URL_SIZE];
    snprintf(buffer, URL_SIZE, "%s: %s\r\n", key, value);
    data_append(&http->req, buffer, (uint32_t)strlen(buffer));
}

/// 设置http body
void http_set_body(http_t* http, const char* body, uint32_t body_size){
    char buffer[URL_SIZE];
    
    snprintf(buffer, URL_SIZE, "Content-Length: %d\r\nConnection: Close\r\n\r\n", body_size);
    data_append(&http->req, buffer, (uint32_t)strlen(buffer));
    if (body!=NULL && body_size!=0) {
        http->body = http->req.mem + http->req.len;
        data_append(&http->req, body, body_size);
        http->body_size = body_size;
    }
}

http_res_t* http_perform(http_t* http){
    int         sock;
    
    http_res_t* res = bs_new(http_res);
    sock = socket_tcp(BS_FALSE);
    if(bs_sock_connect(sock, http->url.domain.mem, http->url.port) != BS_SUCCESS){
        close(sock);
        bs_delete(http);
        res->response_code =  BS_CONNERR;
        return res;
    }
    
    if(write(sock, http->req.mem, http->req.len)<=0){
        close(sock);
        bs_delete(http);
        res->response_code = BS_SENDERR;
        return res;
    }
    
    char buf[1024];
    int len = 0;
    while( (len = (int)read(sock, buf, sizeof(buf)))>0){
        data_append(&res->response, buf, len);
    }
    close(sock);
    bs_delete(http);

    sscanf(res->response.mem, "HTTP/1.1 %d", &res->response_code);
    res->body = bs_strrstr(res->response.mem, "\r\n")+2;
    res->body_size = res->response.len - (uint32_t)(res->body-res->response.mem);
    
    return res;
}

state_t http_download(http_t* http, const char* path){
    int         sock;
    data_t      response;
    int         res_code;
    char*       body;
    bool_t      has_body;
    char        buf[1024];
    int         len = 0;
    FILE*       file;

    sock = socket_tcp(BS_FALSE);
    if(bs_sock_connect(sock, http->url.domain.mem, http->url.port) != BS_SUCCESS){
        close(sock);
        bs_delete(http);
        return BS_CONNERR;
    }
    
    if(write(sock, http->req.mem, http->req.len)<=0){
        close(sock);
        bs_delete(http);
        return BS_SENDERR;
    }
    
    file = fopen(path, "wb+");
    if(file==NULL){
        close(sock);
        bs_delete(http);
        return BS_CREATERR;
    }
    
    
    data_init(&response);
    has_body = BS_FALSE;
    len = (int)read(sock, buf, sizeof(buf));
    sscanf(buf, "HTTP/1.1 %d", &res_code);
    if (res_code != HTTP_OK) {
        close(sock);
        bs_delete(http);
        return res_code;
    }
    
    do{
        if (!has_body) {
            data_append(&response, buf, len);
            // 找到http头末尾
            body = bs_strrstr(response.mem, "\r\n\r\n");
            if (body!=NULL) {
                has_body = BS_TRUE;
                fwrite(body+4, response.len-(body-response.mem), 1, file);
                data_destroy(&response);
            }
        }
        else{
            fwrite(buf, len, 1, file);
        }
    }while( (len = (int)read(sock, buf, sizeof(buf)))>0);
    
    fclose(file);
    close(sock);
    bs_delete(http);
    
    return BS_SUCCESS;
}

void* http_res_init(void * p){
    http_res_t* res = (http_res_t*)p;
    data_init(&res->response);
    res->response_code = 0;
    res->body = NULL;
    res->body_size = 0;

    return res;
}

void http_res_destroy(void* p){
    http_res_t* res = (http_res_t*)p;
    data_destroy(&res->response);
}

state_t http_response_parse(http_res_t* res, const char* buffer, uint32_t size){
    if (res==NULL || buffer==NULL) {
        return BS_PARANULL;
    }
    
    sscanf(buffer, "HTTP/1.1 %d", &res->response_code);
    res->body = bs_strrstr(buffer, "\r\n")+2;
    res->body_size = size - (uint32_t)(res->body-buffer);
    
    return BS_SUCCESS;
}

void url_print(url_t* url){
    fprintf(stdout, "protocal[%s] host[%s] domain[%s] port[%d] path[%s] query[%s]\n", url->protocal.mem, url->host.mem, url->domain.mem, url->port, url->path.mem, url->query.mem);
}
