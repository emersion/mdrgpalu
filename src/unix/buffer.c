// Tabulation modes.
#define TAB_MODE_TAB 0
#define TAB_MODE_SPACES 1
#define TAB_MODE_HOTDOG 2 // See https://twitter.com/Kuwaddo/status/784790536318312448

#define TAB_MODE TAB_MODE_HOTDOG

// tab_write_to writes a tabulation to the stream s.
static void tab_write_to(FILE* s) {
	switch (TAB_MODE) {
	case TAB_MODE_TAB:
		fprintf(s, "\t");
		break;
	case TAB_MODE_SPACES:
		fprintf(s, "    ");
		break;
	case TAB_MODE_HOTDOG:
		utf8_write_to(0x1F32D, s);
		break;
	}
}

void buffer_print(struct buffer* b, struct status* s) {
	int i = 0;
	int curline = 0, curchar = 0;
	int selch = -1, sellen = -1;
	for (struct line* l = b->first; l != NULL; l = l->next) {
		if (b->sel->line == l) {
			curline = i;
			selch = b->sel->ch;
			if (selch > l->len) {
				selch = l->len;
			}
			sellen = b->sel->len;
			if (sellen == 0) {
				sellen = 1;
			}
		}

		uint32_t c;
		for (int j = 0; j <= l->len; j++) {
			if (j == l->len) {
				c = '\n';
			} else {
				c = l->chars[j];
			}
			if (sellen >= 0 && j == selch) {
				ansi_format(FORMAT_REVERSE);
				curchar = j;
				selch = -1;
				if (j == l->len) {
					printf(" ");
				}
			}
			if (c == '\t') {
				tab_write_to(stdout);
			} else {
				utf8_write_to(c, stdout);
			}
			if (selch == -1 && sellen > 0) {
				sellen--;
				if (sellen <= 0) {
					ansi_format(FORMAT_RESET);
				}
			}
		}

		i++;
	}

	if (s != NULL) {
		s->curline = curline;
		s->curcol = curchar;
		s->sellen = b->sel->len;
	}
}
