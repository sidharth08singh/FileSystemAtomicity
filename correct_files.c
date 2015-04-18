#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<uuid/uuid.h>
#define MAX 1000 
#define MAXFILES 100
struct file_display
{
	char fname[MAX];
	int sectors;
};

main()
{
	struct file_display f_disp[MAXFILES];
	int i = 0; 
	int j;
	int k;
	char list_output[MAX];
	FILE *l = popen("ls -ltrh ~/AOS_Project/dummy_filesystem", "r");
	while(fgets(list_output, sizeof(list_output), l) != 0)
	{
		printf("%s\n", list_output);
		if(strstr(list_output, "_sector") == NULL)
		{
			printf("Here1\n");
			strcpy(f_disp[i].fname, list_output);
			f_disp[i].sectors = 1;
			printf("Here1 : %s : %d\n", f_disp[i].fname, f_disp[i].sectors);
			i++;
		}
		
		else
		{	
			printf("Here2\n");
			for (k=1; k<i; k++)
			{
				if (!strncmp(list_output, f_disp[k].fname, strlen(f_disp[k].fname-1)))
				{
					printf("Here 3\n");
					if (strstr(list_output, "_sector2") != NULL) 
					{
						printf("Here4\n");
						f_disp[k].sectors++; 
						break;
					}
					else if (strstr(list_output, "_sector3") != NULL)
					{
						printf("Here5\n");
						f_disp[k].sectors++;
						break;
					}
				}
			}
		}
	}
	
	for(k=0; k<=i; k++)
	{
		printf("Display : %s : %d\n", f_disp[k].fname, f_disp[k].sectors);
		if(f_disp[k].sectors == 3)
		{
			printf("%s\n", f_disp[k].fname);
		}
	}
}
