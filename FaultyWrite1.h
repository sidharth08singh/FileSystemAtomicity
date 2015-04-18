#define MAX 1000
void faulty_put1(char data[], char file_to_write[], int tid);
void faulty_write_abort_before_commit(char data[], struct FileSector *obj1, int tid); 
