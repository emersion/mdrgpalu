struct selection {
	struct line* line;
	int ch;
	int len;
};

struct selection* selection_new() {
	struct selection* s = (struct selection*) malloc(sizeof(struct selection));
	s->line = NULL;
	s->ch = 0;
	s->len = 0;
	return s;
}

void selection_free(struct selection* s) {
	free(s);
}
