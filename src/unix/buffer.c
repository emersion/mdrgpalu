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

		wchar_t c;
		for (int j = 0; j <= l->len; j++) {
			if (j == l->len) {
				c = '\n';
			} else {
				c = l->chars[j];
			}
			if (sellen >= 0 && j == selch) {
				print_format(FORMAT_REVERSE);
				curchar = j;
				selch = -1;
				if (j == l->len) {
					printf(" ");
				}
			}
			utf8_write_to(c, stdout);
			if (selch == -1 && sellen > 0) {
				sellen--;
				if (sellen <= 0) {
					print_format(FORMAT_RESET);
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
