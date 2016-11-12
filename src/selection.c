#define SELECTION_DIR_RIGHT 0
#define SELECTION_DIR_LEFT  1

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
