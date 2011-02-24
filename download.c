#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <curl/curl.h>

/*
 * 	下载函数
 *
 * 说明:
 * 	自动下载url到filename中。返回libcurl下载的状态
 * 	如果filename为NULL，则自动生成一个随机文件名
 * 	下载成功返回filename，否则返回NULL
 * 	如果cb非NULL，下载过程中会每秒以已下载/总大小、已上传/总大小和user_data为参数调用该函数。
 * 	回调函数声明如下：（具体参考curl手册）
 * 		typedef int (*curl_progress_callback)(void *user_data, double dltotal, double dlnow, double ultotal, double ulnow);
 */
char * fetch(const char *url, curl_progress_callback cb, void *user_data)
{
	int fd, ret;
	FILE *fp;
	CURL *curl;
	char tmpname[]="/tmp/huen_download.XXXXXX";

	if (url==NULL)
		return NULL;

	ret = mkstemp(tmpname);
	if (ret<0)
		return NULL;
	close(ret);//we open it manually!

	fp = fopen(tmpname, "w");
	curl = curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,url);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,fp);

	//自动解析用GZIP等压缩过的页面
	curl_easy_setopt(curl,CURLOPT_ENCODING,fp);

	//自动跟踪和跟随URL
	curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION, 1);

	//下载进度跟踪函数配置
	if (cb!=NULL) {
		curl_easy_setopt(curl,CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt(curl,CURLOPT_PROGRESSFUNCTION, cb);
		curl_easy_setopt(curl,CURLOPT_PROGRESSDATA, user_data);
	}

	ret = curl_easy_perform(curl);

	fclose(fp);
	curl_easy_cleanup(curl);

	return ret ? NULL : strdup(tmpname);
}

/*
 * 	获取影片信息中的vid
 *
 * 说明：
 * 	传入影片信息页面地址，返回影片下载页面地址；
 * 	失败，返回NULL；
 */
const char * get_download_url(const char *url)
{
	FILE *fp;
	char *pos, *filename;
	char buf[1024]={0};
	unsigned int vid=0,inscript=0;

	filename = fetch(url, NULL, NULL);
	if (filename==NULL) {
		return NULL;
	}

	fp = fopen(filename, "r");
	while(fgets(buf, sizeof(buf)-1, fp)){
		if (strstr(buf, "<script"))	inscript=1;

		if(inscript){
			pos=strstr(buf, "vid=\"");
			if (pos && (pos[-1]==',' || pos[-1]==' ')) {
				vid = atoi(pos+5);
			}
		}

		if (strstr(buf, "</script>"))	inscript=0;
	}
	fclose(fp);
	unlink(filename);

	if (!vid)
		return NULL;

	return g_strdup_printf("http://hot.vrs.sohu.com/vrs_flash.action?vid=%d\0", vid);
}


/*
 *	获取影片列表
 *
 * 传入影片关键字
 *
 * 输出搜索到的影片信息URL地址单链表
 *
 */
#define URL_KEYSTR	"<a class=\"name\" href=\"/to?u="
GSList * get_movie_list(const char *keyword)
{
	FILE *fp;
	char buf[1024]={0};
	char *url,*pos, *filename=NULL;
	GSList *lst=NULL;

	//FIXME:
	//	将keyword编码成%xx%xx%xx的形式
	url = g_strdup_printf("http://so.tv.sohu.com/mts?wd=%s", keyword);
	filename = fetch(url, NULL, NULL);
	if (!filename) {
		return NULL;
	}

	fp = fopen(filename, "r");
	while(fgets(buf, sizeof(buf)-1, fp)){
		url = strstr(buf, URL_KEYSTR);
		if (url){
			url += strlen(URL_KEYSTR);
			pos  = strchr(url,'&');
			*pos ='\0';
			lst = g_slist_append(lst, strdup(url));
		}
	}
	fclose(fp);
	unlink(filename);
	return lst;
}
