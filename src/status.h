// The editor's current status.
struct status {
	// The editor's filename.
	char* filename;

	// The currently selected line.
	int curline;
	// The currently selected column.
	int curcol;
	// The length of the selection.
	int sellen;
};

// status_print prints the status s.
void status_print(struct status* s);
