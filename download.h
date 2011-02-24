#ifndef __DOWNLOAD_H__
#define __DOWNLOAD_H__

#include <curl/curl.h> //For curl_progress_callback's define

const char * fetch(const char *url, curl_progress_callback cb, void *user_data);

//传入影片信息页地址，返回影片下载页地址
const char * get_download_url(const char *info_url);

//传入影片关键字，返回搜索到的影片列表
GSList * get_movie_list(const char *keyword);

#endif //__DOWNLOAD_H__
