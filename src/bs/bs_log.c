#include <stdarg.h>
#include <time.h>
#include "bs_log.h"
#ifdef __SIGNAL__
#include "bs_timer.h"
#endif
#include "bs_common.h"

const static char* _LOG_NAME[] = {
    "EMERG",
    "ALERT",
    "CRIT",
    "ERR",
    "WARNING",
    "NOTICE",
    "INFO",
    "DEBUG"
};

bs_log_t*           g_log = NULL;

static void localtimes(time_t time,long timezone,struct tm* tm_time);
static void log_refresh(bs_log_t* plog);

int bs_log_type(const char* name){
    uint32_t            i;

    for(i=0; i< sizeof(_LOG_NAME)/sizeof(char*); i++){
        if(strcmp(name, _LOG_NAME[i]) == 0){
            return i;
        }
    }

    return -1;
}

state_t bs_log_init(const char* path){
    if (g_log != NULL) {
        return BS_HASEXIST;
    }

    g_log = malloc(sizeof(bs_log_t));
    return bs_log_init_entity(g_log, path);
}

state_t bs_log_init_entity(bs_log_t* plog, const char* path){
    struct timeval      utime;
    char                file[BS_LONG_STRLEN];

    assert(plog!=NULL && path!=NULL);
    memcpy(plog->path, path, BS_DEF_STRLEN);
    plog->path[BS_DEF_STRLEN-1] = '\0';
    memset(plog->flag, 0, sizeof(int)*LOG_NUM);
    memset(plog->type_file, 0, sizeof(FILE*)*LOG_NUM);

    if(strcmp(path, "stdout")==0){
        plog->file = stdout;
    }
    else{
        memset(&plog->ltm, 0, sizeof(struct tm));
        gettimeofday(&utime, NULL);
        localtimes((time_t)(utime.tv_sec), -28800, &plog->ltm);

        sprintf(file, "%s.%d-%d-%d.log", plog->path, plog->ltm.tm_year+1900, plog->ltm.tm_mon+1, plog->ltm.tm_mday);
        plog->file = fopen(file, "a+");
        if(plog->file == NULL){
            return BS_NOTFOUND;
        }
    }

    plog->flag[LOG_EMERG] = 1;
    plog->flag[LOG_ALERT] = 1;
    plog->flag[LOG_CRIT] = 1;
    plog->flag[LOG_ERR] = 1;
    plog->flag[LOG_INFO] = 1;
    plog->flag[LOG_DEBUG] = 1;

    return BS_SUCCESS;
}

state_t bs_log_set(bs_log_t* plog, int log, int flag){

    assert(0<=log && log<LOG_NUM);
    plog->flag[log] = flag;
    /*
    if(plog->flag[log] && plog->file[log] == NULL){
        sprintf(file, "%s_%s", plog->path, _LOG_NAME[log]);
        plog->file[log] = fopen(file, "w+");
        if(plog->file[log] == NULL){
            return BS_NOTFOUND;
        }
    }
    */

    return BS_SUCCESS;
}

state_t bs_log_setflag(bs_log_t* plog, const int* flag, uint32_t size){
    assert(flag != NULL && size >= 0);
    uint32_t logsize = bs_min(size, LOG_NUM);
    memcpy(plog->flag, flag, logsize);

    /*
    for(i = 0; i<logsize; i++){
        plog->flag[i] = flag[i];
        if(plog->flag[i] && plog->file[i] == NULL){
            sprintf(file, "%s_%s", plog->path, _LOG_NAME[i]);
            plog->file[i] = fopen(file, "w+");
        }
    }
    */
    return BS_SUCCESS;
}

state_t bs_log(bs_log_t* plog, int log, const char fmt[], ...){
    char                buf[10*BS_LONG_STRLEN];
    va_list             va;
    struct tm           ltm;
    struct timeval      utime;

    assert(0<=log && log<LOG_NUM);
    if (plog == NULL) {
        return BS_INVALID;
    }
    if(!plog->flag[log]){
        return BS_AUTHORITY;
    }

    //bs_wrlock(&plog->lock);
    va_start(va, fmt);
    vsnprintf(buf, 10*BS_LONG_STRLEN, fmt, va);
    if(plog->flag[log] && plog->file != NULL){
        memset(&ltm, 0, sizeof(struct tm));
        gettimeofday(&utime, NULL);
        localtimes((time_t)(utime.tv_sec), -28800, &ltm);
        if(ltm.tm_mday!=plog->ltm.tm_mday || ltm.tm_mon!=plog->ltm.tm_mon || ltm.tm_year!= plog->ltm.tm_year){
            log_refresh(plog);
        }
        #ifdef __SIGNAL__
        // 定时器中如果有log语句，可能会造成fprintf的死锁
        bs_timer_suspend();
        #endif
        fprintf(plog->file , "[%s]<%02d:%02d:%02d:%06d(%05d %05lu)> %s\n",
                _LOG_NAME[log], ltm.tm_hour, ltm.tm_min, ltm.tm_sec, (int)utime.tv_usec,
                (int)getpid(), (unsigned long)pthread_self(), buf);
        fflush(plog->file);
        #ifdef __SIGNAL__
        bs_timer_resume();
        #endif
    }
    else if(plog->flag[log] && plog->file == NULL){
        return BS_NOTFOUND;
    }

    va_end(va);
    //bs_wrlock_unlock(&plog->lock);

    return BS_SUCCESS;
}

state_t bs_log_destroy(){
    if (g_log == NULL) {
        return BS_NOTFOUND;
    }

    bs_log_close(g_log);
    free(g_log);

    return BS_SUCCESS;
}

state_t bs_log_close(bs_log_t* plog){
    /*
    uint32_t            i;

    for(i = 0; i<LOG_NUM; i++){
        if(plog->file[i]){
            fclose(plog->file[i]);
        }
    }
    */

    fclose(plog->file);
    memset(plog, 0, sizeof(bs_log_t));
    return BS_SUCCESS;
}

static void log_refresh(bs_log_t* plog){
    struct timeval      utime;
    char                file[BS_LONG_STRLEN];

    memset(&plog->ltm, 0, sizeof(struct tm));
    gettimeofday(&utime, NULL);
    localtimes((time_t)(utime.tv_sec), -28800, &plog->ltm);

    if(strcmp(plog->path, "stdout")!=0){
        sprintf(file, "%s.%s.%d-%d-%d", plog->path,"log", plog->ltm.tm_year+1900, plog->ltm.tm_mon+1, plog->ltm.tm_mday);
        fclose(plog->file);
        plog->file = fopen(file, "w+");
    }
}

const static char Days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static void localtimes(time_t time,long timezone,struct tm* tm_time)
{
    int32_t     n32_Pass4year;
    int32_t     n32_hpery;

    time=time-timezone;
    if (time < 0) {
        time = 0;
    }
    tm_time->tm_sec=(int)(time % 60);
    time /= 60;
    tm_time->tm_min=(int)(time % 60);
    time /= 60;
    n32_Pass4year=((unsigned int)time / (1461L * 24L));
    tm_time->tm_year=(n32_Pass4year << 2)+70;
    time %= 1461L * 24L;
    for (;;) {
        n32_hpery = 365 * 24;
        if ((tm_time->tm_year & 3) == 0) {
            n32_hpery += 24;
        }
        if (time < n32_hpery) {
            break;
        }
        tm_time->tm_year++;
        time -= n32_hpery;
    }

    tm_time->tm_hour = (int)(time % 24);
    time /= 24;
    time++;
    if ((tm_time->tm_year & 3) == 0) {
        if (time > 60) {
            time--;
        }
        else{
            if (time == 60) {
                tm_time->tm_mon = 1;
                tm_time->tm_mday = 29;
                return ;
            }
        }
    }

    for (tm_time->tm_mon = 0; Days[tm_time->tm_mon] < time;tm_time->tm_mon++) {
        time -= Days[tm_time->tm_mon];
    }

    tm_time->tm_mday = (int)(time);
    return;
}
