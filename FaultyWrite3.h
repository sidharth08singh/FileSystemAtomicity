#define MAX 1000
void faulty_put3(char data[], char file_to_write[], int tid);
void faulty_write_crash_before_commit(char data[], struct FileSector *obj1, int tid); 
