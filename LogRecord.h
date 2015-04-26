#define MAX 1000
struct operation
{
        int tid; //size of int = 4 bytes on development machine
};
struct last_transaction
{
	int tid;
	char operation[MAX];
	char fname[MAX];
	char status[MAX];
};
void add_log_record_entry(char operation[], char state[], char filename[], int tid);
void create_tid(struct operation *op1);
struct last_transaction search_log_record_entry(char filename[]);
char* fgetsr(char* buf, int n, FILE* binaryStream);
long fsize(FILE* binaryStream);
void add_recovery_entry(int state);
