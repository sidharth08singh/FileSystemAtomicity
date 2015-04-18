#define MAX 1000
void faulty_put2(char data[], char file_to_write[], int tid);
void faulty_write_abort_after_commit(char data[], struct FileSector *obj1, int tid); 
