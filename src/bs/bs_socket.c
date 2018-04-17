/**
 * file :	bs_socket.c
 * author :	bushaofeng
 * created :	2013-11-18 15:03
 * func : 
 */
#include "bs_socket.h"
#include "bs_common.h"
#include "bs_type.h"

static int socket_create(int sock_type, int is_nonblock);
static int sockaddr_create(struct sockaddr_in* addr, in_addr_t s_addr, int port);

int socket_udp(bool_t is_nonblock){
    return socket_create(SOCK_DGRAM, is_nonblock);
}

int socket_tcp(bool_t is_nonblock){
    return socket_create(SOCK_STREAM, is_nonblock);
}

state_t bs_sock_keepalive(int sock){
    uint32_t    len = sizeof(unsigned int);
    int         optval = 1;
    
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &optval, len);//使用KEEPALIVE
    return BS_SUCCESS;
}

state_t bs_sock_nodelay(int sock){
    uint32_t    len = sizeof(unsigned int);
    int         optval = 1;
    
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &optval, len);//禁用NAGLE算法，立刻发送
    return BS_SUCCESS;
}

state_t bs_sock_optimize(int sock){
    uint32_t    len = sizeof(unsigned int);
    int         optval = 1;
    
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &optval, len);//使用KEEPALIVE
    optval = 0;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &optval, len);//禁用NAGLE算法，立刻发送
    return BS_SUCCESS;
}

state_t bs_sock_bind(int sock, int port){
    struct sockaddr_in addr;

    sockaddr_create(&addr, htonl(INADDR_ANY), port); 
    if(bind(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr))==-1){
        return BS_HOSTERR;
    }

    return BS_SUCCESS;
}

state_t bs_sock_addr(struct sockaddr_in* addr, const char* ip, int port){
    struct hostent* host;

    assert(addr!=NULL && ip!=NULL);
    if ((host=gethostbyname(ip))==NULL){
        return BS_HOSTERR;
    }

    bzero(addr, sizeof(addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr = *((struct in_addr *)host->h_addr);

    return BS_SUCCESS;
}

state_t bs_sock_connect(int sock, const char* ip, int port){
    state_t             st;
    struct sockaddr_in  addr;

    st = bs_sock_addr(&addr, ip, port);
    if(st != BS_SUCCESS){
        return st;
    }

    st = connect(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    if(st==0){
        return BS_SUCCESS;
    }

    return errno;
}

struct sockaddr_in bs_int2addr(uint64_t value){
    struct sockaddr_in  addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = value >> 32;
    addr.sin_port = value & 0xffff;
    
    return addr;
}

int bs_sock_recvfrom(int sock, struct sockaddr_in* addr, char* buf, size_t size){
    int len;
    socklen_t sin_size = sizeof(struct sockaddr_in);

    assert(addr!=NULL && buf!=NULL);
    len = recvfrom(sock, buf, size, 0, (struct sockaddr*)addr, &sin_size);
    if(len>0){
        buf[len] = '\0';
    }

    return len;
}

int bs_sock_sendto(int sock, struct sockaddr_in* addr, char* buf, size_t size){
    socklen_t sin_size = sizeof(struct sockaddr_in);
    assert(addr!=NULL && buf!=NULL);
    return sendto(sock, buf, size, 0, (struct sockaddr*)addr, sin_size);
}

state_t bs_sock_localip(char* ip, int size)
{
    /*
    int inet_sock;
    int i;
    struct ifreq ifr;
    const char* ifr_name[] = {"eth0", "en0"};
    
    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
    memset(ip, 0, size);
    for (i=0; i<sizeof(ifr_name)/sizeof(char*); i++) {
        strcpy(ifr.ifr_name, ifr_name[i]);
        int rv = ioctl(inet_sock, SIOCGIFADDR, &ifr);
        if(rv < 0){
            //fprintf(stderr, "%s: ioctl error", __FUNCTION__);
            continue;
        }
        char* local = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
        bs_strcpy(ip, size, local);
        close(inet_sock);
        return BS_SUCCESS;
    }
    
    fprintf(stderr, "%s: ioctl error", __FUNCTION__);
    close(inet_sock);
    */
    return BS_INVALID;
    /*
    char hname[128];
    struct hostent *hent;
    int i;
    
    gethostname(hname, sizeof(hname));
    
    //hent = gethostent();
    hent = gethostbyname(hname);
    
    printf("hostname: %s\naddress list: ", hent->h_name);
    for(i = 0; hent->h_addr_list[i]; i++) {
        printf("%s\t", inet_ntoa(*(struct in_addr*)(hent->h_addr_list[i])));
    }
    */
    
    /*
    int MAXINTERFACES=16;
    int fd, intrface, retn = 0;
    struct ifreq buf[MAXINTERFACES];
    struct ifconf ifc;
    
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t)buf;
        if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
        {
            intrface = ifc.ifc_len / sizeof(struct ifreq);
            
            while (intrface-- > 0)
            {
                if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
                {
                    char* local=(inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));
                    bs_strcpy(ip, size, local, (uint32_t)strlen(local));
                    close (fd);
                    return BS_SUCCESS;
                }
            }
        }
        close (fd);
    }
    */
    
    return BS_INVALID;
}

static int sockaddr_create(struct sockaddr_in* addr, in_addr_t s_addr, int port){
    assert(addr != NULL);
    bzero(addr, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = s_addr;

    return BS_SUCCESS;
}

static int socket_create(int sock_type, int is_nonblock){
    int sock;

    if((sock = socket(AF_INET, sock_type, 0)) == -1){
        return -1;
    }

    if (sock_type == SOCK_STREAM) {
        // 设置发送不延迟
        int flag = 0;
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(int));     //使用KEEPALIVE
        setsockopt (sock, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(int));    // 立刻发送
    }
    
    if(is_nonblock){
        socket_unblock(sock);
    }
    else{
        socket_block(sock);
    }

    return sock;
}

state_t socket_unblock(int sock){
    int flags;
    
    if((flags = fcntl(sock, F_GETFL, 0)) < 0) {
        close(sock);
        return BS_INVALID;
    }
    if(fcntl(sock, F_SETFL, flags|O_NONBLOCK) < 0){
        close(sock);
        return BS_INVALID;
    }
    
    return BS_SUCCESS;
}
state_t socket_block(int sock){
    int flags;
    
    if((flags = fcntl(sock, F_GETFL, 0)) < 0) {
        close(sock);
        return BS_INVALID;
    }
    if(fcntl(sock, F_SETFL, flags & ~O_NONBLOCK) < 0){
        close(sock);
        return BS_INVALID;
    }
    
    return BS_SUCCESS;
}
