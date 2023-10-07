
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
void *ThreadA(void *m)
{


	system("ffmpeg -i udp://171.240.150.103:1977 -s 854x480 -c:v libx264 -b:v 800000 -hls_time 10 -hls_list_size 10 -start_number 1 mystream1.m3u8");
}

void *ThreadB(void *m)
{

	system("ffmpeg -i udp://171.240.150.103:1978 -s 854x480 -c:v libx264 -b:v 800000 -hls_time 10 -hls_list_size 10 -start_number 1 mystream2.m3u8");
}
int main()
{
        pthread_t th1, th2;
        pthread_create(&th1, NULL, &ThreadA, NULL);
        pthread_create(&th2, NULL, &ThreadB, NULL);
	pthread_join(th1,NULL);
        pthread_join(th2,NULL);
}

