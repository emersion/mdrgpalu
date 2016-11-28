struct status {
	char* filename;

	int curline;
	int curcol;
	int sellen;
};

void status_print(struct status* s);
