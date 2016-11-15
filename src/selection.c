#define SELECTION_DIR_RIGHT 0
#define SELECTION_DIR_LEFT  1

// A selection is a range of text in a buffer. The range starts at line at index
// ch and ends len characters after. dir is a hint of the direction of the
// selection.
struct selection {
	struct line* line;
	int ch;
	int len;
	int dir;
};

struct selection* selection_new() {
	struct selection* s = (struct selection*) malloc(sizeof(struct selection));
	s->line = NULL;
	s->ch = 0;
	s->len = 0;
	s->dir = SELECTION_DIR_RIGHT;
	return s;
}

void selection_free(struct selection* s) {
	free(s);
}
