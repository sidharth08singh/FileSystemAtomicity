#define MAX 1000
struct operation
{
        int tid; //size of int = 4 bytes on development machine
};
void add_log_record_entry(char operation[], char state[], char filename[], int tid);
void create_tid(struct operation *op1);
