#include <curl/curl.h>


int fetch(const char *url, char *savename, curl_progress_callback cb, void *user_data)
{
	int ret;
	CURL *curl;
	FILE *fp;

	fp = fopen(savename, "w");
	curl = curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,url);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,fp);
	//自动跟踪和跟随URL
	curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION, 1);

	//下载进度跟踪函数配置
	if (cb!=NULL) {
		curl_easy_setopt(curl,CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt(curl,CURLOPT_PROGRESSFUNCTION, cb);
		curl_easy_setopt(curl,CURLOPT_PROGRESSDATA, user_data);
	}

	ret = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	fclose(fp);

	printf("result %d\n", ret);

	return 0;
}
