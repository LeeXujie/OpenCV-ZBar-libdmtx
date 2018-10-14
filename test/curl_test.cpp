#include <stdio.h>
#include <string.h>
#include "curl/curl.h"

size_t write_data(void* buffer, size_t size, size_t nmemb, void* userp)
{
	static int current_index = 0;
	current_index++;
	printf("current: %d\n", current_index);
	printf("%s\n", (char*)buffer);
	int temp = *(int*)userp;
	return nmemb;
}

int curl_test(void)
{
	CURL* curl = curl_easy_init();

	if (NULL == curl)
	{
		printf("(curl is NULL!/r/n");
		return -1;
	}

	int my_param = 1;
	// ����Ŀ��URL 
	curl_easy_setopt(curl, CURLOPT_URL, "http://www.baidu.com");
	// ���ý��յ�HTTP������������ʱ���õĻص����� 
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	// �����Զ������(�ص������ĵ��ĸ�����) 
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &my_param);
	// ִ��һ��URL���� 
	CURLcode res = curl_easy_perform(curl);
	// ����ɾ� 
	curl_easy_cleanup(curl);

	return 0;
}

int main()
{
	curl_global_init(CURL_GLOBAL_ALL);
	curl_test();
	curl_global_cleanup();
	getchar();
	return 0;
}