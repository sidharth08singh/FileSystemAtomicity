#include<stdio.h>
#include<stdlib.h>
#include<string.h>

main()
{
	char data[100] = "abcdefghijklmnop";
	printf("%s\n", data);
	int i = strlen(data)/2;
	data[i] = '\0';
	printf("%s\n", data);
}
