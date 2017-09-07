/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for linux.
 * Created on: 2015-04-28
 */

#include <elog.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

// 定义线程本地变量
#if HAS_CXX11_THREAD_LOCAL
#define TLS thread_local
#elif defined (__GNUC__)
#define TLS __thread
#elif defined (_MSC_VER)
#define TLS __declspec(thread)
#else // !C++11 && !__GNUC__ && !_MSC_VER
    #error "Define a thread local storage qualifier for your compiler/platform!"
#endif

// 跨平台的函数
//#if ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux ) || (defined )
#if (defined _WIN32)

#include <windows.h>
#include <share.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

void platform(){
    log_i("in windows os\n");
}
int file_open(const char *name) {
    return _open(name, _O_CREAT | _O_WRONLY | _O_APPEND | _O_BINARY , _S_IREAD | _S_IWRITE);
}
int file_write(int fd, const char *buf, size_t size){
    return _write(fd, buf, size);
}
int file_close(int fd) {
    return _close(fd);
}

#else
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

void platform(){
    log_i("in unix* os\n");
}
int file_open(const char *name){
    return open(name, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
}
int file_write(int fd, const char *buf, size_t size){
    return write(fd, buf, size);
}
int file_close(int fd) {
    return close(fd);
}

#endif

//本地变量
static pthread_mutex_t output_lock = PTHREAD_MUTEX_INITIALIZER;
char g_log_file_name[ MAX_FILENAME + 1] = "";
int g_log_init_flag = 0;
/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;
    return result;
}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size) {
    /* output to terminal */
    if(g_log_file_name[0] == 0) {
        file_write(1, log, size);
    }
    else{
        int fd = file_open(g_log_file_name);
        file_write(fd, log, size);
        file_close(fd);
    }
}

/**
 * output lock
 */
void elog_port_output_lock(void) {
    pthread_mutex_lock(&output_lock);
}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {
    pthread_mutex_unlock(&output_lock);
}

/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void) {
     static char cur_system_time[24] = { 0 };
    time_t timep;
    struct tm *p;

    time(&timep);
    p = localtime(&timep);
    if (p == NULL) {
        return "";
    }
    snprintf(cur_system_time, 18, "%02d-%02d %02d:%02d:%02d", p->tm_mon + 1, p->tm_mday,
            p->tm_hour, p->tm_min, p->tm_sec);

    return cur_system_time;
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {
    static char cur_process_info[10] = { 0 };

    snprintf(cur_process_info, 10, "pid:%04d", getpid());

    return cur_process_info;
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void) {
    static char cur_thread_info[10] = { 0 };

    snprintf(cur_thread_info, 10, "tid:%04ld", (long)pthread_self());


    return cur_thread_info;
}
