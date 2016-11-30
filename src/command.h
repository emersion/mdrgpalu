struct command {
	struct command* next;

	char* title;
	struct event* evt;
	void (*exec)(struct editor* e, struct event* evt);
};

struct command commands[];

struct command* command_match(struct command commands[], int len, struct event* evt);
