#include <fetch.h>

int fetch(const char *url, char *buff)
{
	fetchIO * fp;
	char flags[10]={0};
	size_t size;
	char tmp[1024];
	size_t ret=0;
	struct url_stat stat={0};

	ret = fetchStatURL(url, &stat, flags);
	printf("todo %d %d\n", ret, stat.size);
	ret=0;

	fp = fetchGetURL(url, flags);
	if (!fp) {
		printf("error: ");
		return 1;
	}

	do{
		size = fetchIO_read(fp, tmp, sizeof(tmp));
		ret +=size;
	}while(size);

	fetchIO_close(fp);

	printf("result %d\n", ret);

	return 0;
}
