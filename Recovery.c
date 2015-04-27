#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<limits.h>
#include<string.h>
#include<pthread.h>
#include<Recovery.h>
#include "LogRecord.h"
#include "WriteFile.h"
#include "ReadFile.h"

int start_recovery()
{
	pthread_mutex_lock(&lock);
	struct marked_for_recovery* head;
	head = (struct marked_for_recovery*)malloc(sizeof(struct marked_for_recovery));
	head->link = NULL;
	printf("Here in Recovery 1\n");
	int field1;
	char field2[MAX];
	char field3[MAX];
	char field4[MAX];
	FILE* fd = NULL;
        long sz;
	int count=0;
        fd = fopen("journal.txt", "rb");
        sz = fsize(fd);
        if (sz > 0)
        {
		printf("Here in Recovery 2\n");
                char buf[MAX];
                char tmp[MAX];
                char *token;
                fseek(fd, sz, SEEK_SET);
                while (fgetsr(buf, sizeof(buf), fd) != NULL)
                {
			printf("Here in Recovery 3\n");
                        if(strstr(buf, "RECOVERY END") != NULL)
                        {
				break;
			}
			else
			{
				if(strstr(buf, "RECOVERY START") == NULL)
				{
					field1=0;
					memset(field2, 0, MAX);
					memset(field3, 0, MAX);
					memset(field4, 0, MAX);

					printf("Here in Recovery 4\n");
                                	printf("Log Entry Encountered : %s\n", buf);
                                	strcpy(tmp,buf);
                                	token = strtok(tmp,",");
                                	while(isspace(*token)) token++; //Remove leading whitespaces
                                	field1 = atoi(token);
	
					token = strtok(NULL, ",");
                                	while(isspace(*token)) token++; //Remove leading whitespaces
					strcpy(field2, token);
			
					token = strtok(NULL, ",");
                                	while(isspace(*token)) token++; //Remove leading whitespaces
                                	strcpy(field3, token);
			
					token = strtok(NULL, ",");
                                	while(isspace(*token)) token++; //Remove leading whitespaces
					size_t ln = strlen(token) - 1; //Remove trailing newlines
                                	if(token[ln] == '\n')
                                        	token[ln] = '\0';
                                	strcpy(field4, token);

					head = create_recovery_list(head, field1, field2, field3, field4);
				}
			}
		}
	}
	pthread_mutex_unlock(&lock);
	count = execute_recovery(head);
	return count;
}

int execute_recovery(struct marked_for_recovery* head)
{
	printf("In Execute Recovery\n");
	struct marked_for_recovery* temp;
	int count = 0; 
        temp = (struct marked_for_recovery*)malloc(sizeof(struct marked_for_recovery));
        temp = head->link;
        while(temp!=NULL)
        {
                if(temp->commited != 1)
                {
			count++;
			printf("Running Recovery for %s\n", temp->name);
                        all_or_nothing_get(temp->name);
                }
		temp = temp->link;
        }
        return count;
}

struct marked_for_recovery* create_recovery_list(struct marked_for_recovery* head, int field1, char field2[], char field3[], char field4[])
{
	printf("In Create Recovery List\n");
	int isExist=0;
	isExist = check_tid_in_list(head,field1);
	struct marked_for_recovery* temp;
	if(isExist == 0) //tid not present in recovery list. Add to list
	{
		printf("TID Not Present: Adding TID\n");
		temp = (struct marked_for_recovery*)malloc(sizeof(struct marked_for_recovery));

		temp->link = NULL;

		temp->tid = field1;
		strcpy(temp->name, field3);
		strcpy(temp->operation, field2);

		if(!strcmp(field4, "pending"))
			temp->pending = 1;
		else if(!strcmp(field4, "commit"))
			temp->commited = 1;
		else if(!strcmp(field4, "end"))
			temp->end = 1;

		if(head->link == NULL) //If the list is Empty
        	{
                	head->link = temp;
                	return head;
        	}

        	temp->link = head->link;
        	head->link = temp;
	}
	else
	{
		//tid already in recovery list. Update. 
		printf("TID Present, Updating\n");
		update_recovery_list(head, field1, field4);
	}
	return head;
}
	
int check_tid_in_list(struct marked_for_recovery* head, int field1)
{
	printf("In check tid in list\n");
	struct marked_for_recovery* temp;
        temp = (struct marked_for_recovery*)malloc(sizeof(struct marked_for_recovery));
        temp = head->link;
        while(temp!=NULL)
        {
                if(temp->tid == field1)
		{
                	return 1;
		}
		temp = temp->link;
        }
	return 0;
}

void update_recovery_list(struct marked_for_recovery* head, int field1, char field4[])
{
	printf("In update recovery list\n");
	struct marked_for_recovery* temp;
        temp = (struct marked_for_recovery*)malloc(sizeof(struct marked_for_recovery));
        temp = head->link;
        while(temp!=NULL)
        {
                if(temp->tid == field1)
                {
                	if(!strcmp(field4, "pending"))
                        	temp->pending = 1;
                	else if(!strcmp(field4, "commit"))
                        	temp->commited = 1;
                	else if(!strcmp(field4, "end"))
                        	temp->end = 1;
			break;
                }
		temp = temp->link; 
        }
}
	
	

	
