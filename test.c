#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#define MAX 1000

int main()
{
	char a[MAX] = "this is\npoint\nless";
	char b[MAX];
	printf("%s", a);
	sprintf(b, "echo '%s' > /tmp/abc", a);
	printf("###%s", b);
	system(b);
}
