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

// selection_new allocates a new empty selection.
struct selection* selection_new() {
	struct selection* s = (struct selection*) malloc(sizeof(struct selection));
	s->line = NULL;
	s->ch = 0;
	s->len = 0;
	s->dir = SELECTION_DIR_RIGHT;
	return s;
}

// selection_free deallocates a selection. s cannot be used anymore after a call
// to this function.
void selection_free(struct selection* s) {
	free(s);
}

// selection_set_ch sets the selection's character index. It changes the line
// and the character if needed (e.g. if the index is after the end of line or
// if the index is negative).
void selection_set_ch(struct selection* s, int at) {
	struct line* l = NULL;
	int ch = 0;
	line_walk_char(s->line, at, &l, &ch);
	s->line = l;
	s->ch = ch;
}

// selection_move_ch moves the selection's character by i.
void selection_move_ch(struct selection* s, int i) {
	int at = s->ch;
	if (at > s->line->len) {
		at = s->line->len;
	}
	at += i;
	selection_set_ch(s, at);
}

// selection_set_len sets the selection's length. It truncates it if the
// selection ends beyond the buffer.
void selection_set_len(struct selection* s, int len) {
	// Check that selection is not beyond the end of the buffer
	int end = s->ch + len;
	if (end > s->line->len) {
		struct line* l = s->line;
		while (l != NULL && end >= 0) {
			end -= l->len;
			if (l->next != NULL) {
				end -= 1;
			}
			l = l->next;
		}
		if (end > 0) {
			len -= end;
		}
	}
	s->len = len;
}

// selection_extend_ch extends the selection by i characters.
void selection_extend_ch(struct selection* s, int i) {
	int len = s->len;

	if (s->dir == SELECTION_DIR_RIGHT) {
		len += i;
	} else {
		// Move the start of the selection
		int at = s->ch + i;
		struct line* l = s->line;
		while (at < 0 && l->prev != NULL) {
			at += l->prev->len + 1;
			l = l->prev;
		}
		if (at < 0) {
			at = 0;
		} else {
			len -= i;
		}
		s->line = l;
		s->ch = at;
	}

	selection_set_len(s, len);
}

// selection_shrink sets the selection's length to zero.
void selection_shrink(struct selection* s, int dir) {
	if (dir > 0) {
		selection_move_ch(s, s->len);
	}
	s->len = 0;
}

// selection_jump jumps the selection s to the right if dir > 0, to the left if
// dir < 0.
void selection_jump(struct selection* s, int dir) {
	int d = line_jump(s->line, s->ch, dir);
	selection_move_ch(s, d);
}

// selection_extend_jump jumps and extends the selection s to the right if dir >
// 0, to the left if dir < 0.
void selection_extend_jump(struct selection* s, int dir) {
	int d = line_jump(s->line, s->ch, dir);
	selection_extend_ch(s, d);
}
