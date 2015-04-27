#define MAX 1000
struct marked_for_recovery
{
	int tid;
	char name[MAX];
	char operation[MAX];
	int pending;
	int commited;
	int end;
	struct marked_for_recovery* link;
};
int start_recovery();
int execute_recovery(struct marked_for_recovery* head);
struct marked_for_recovery* create_recovery_list(struct marked_for_recovery* head, int field1, char field2[], char field3[], char field4[]);
int check_tid_in_list(struct marked_for_recovery* head, int field1);
void update_recovery_list(struct marked_for_recovery* head, int field1, char field4[]);

