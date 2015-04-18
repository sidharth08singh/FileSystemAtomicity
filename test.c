#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int threads = 5;
int i;

	
void *foobar_function(void *arguments)
{
	int x;
	x = (int)arguments;
	printf("%d\n", x);
}
	
int main()
{
	pthread_t * thread = malloc(sizeof(pthread_t)*threads);
	for (i = 0; i < threads; i++) 
	{
		if(i==2)
			sleep(5);
    		int ret = pthread_create(&thread[i], NULL, &foobar_function, (void*)i);
    		if(ret != 0) 
		{
        		printf ("Create pthread error!\n");
        		exit (1);
    		}
	}

	for(i = 0; i < threads; i++)
	{
		pthread_join(thread[i], NULL); // don't actually want this here :)
	}
	sleep(1);     // main() will probably finish before your threads do,
	free(thread); // so we'll sleep for illustrative purposes
}
