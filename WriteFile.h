#define MAX 1000
#define MAXFILENAMELEN 255
#define MAXLINE 150
#define MAXFILES 200

struct FileSector
{
	char fs1[MAX];
	char cs1[MAX];
	char fs2[MAX];
	char cs2[MAX];
	char fs3[MAX];
	char cs3[MAX];
};

pthread_mutex_t lock;


void all_or_nothing_put(char data[], char file_to_write[], int tid);
void compute_md5sum(struct FileSector *obj1);
void find_sectors(char filename[], struct FileSector *obj1);
void check_and_repair(struct FileSector *obj1);
void write_and_commit(char data[], struct FileSector *obj1, int tid); 
