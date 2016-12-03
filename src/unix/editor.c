#define EDITOR_AUTOCOMPLETE_COLOR 236

static void editor_print_title(struct editor* e) {
	char* filename = e->filename;
	if (filename == NULL) {
		filename = "untitled";
	}

	char title[128];
	snprintf(title, sizeof(title), "%s - mdrgpalu", filename);
	term_set_title(title);
}

void editor_print(struct editor* e) {
	struct status* s = (struct status*) malloc(sizeof(struct status));
	s->filename = e->filename;

	editor_print_title(e);

	term_clear_screen();
	term_cursor_move(0, 0);
	buffer_print(e->buf, s);
	term_cursor_move(0, term_height()-1);
	if (e->status != NULL) {
		print_format(FORMAT_DIM);
		printf("%s", e->status);
		print_format(FORMAT_RESET);
		editor_set_status(e, NULL);
	} else {
		status_print(s);
	}
	term_flush();

	free(s);
}

static void editor_autocomplete(struct editor* e, int offset, char** list, int len, int sel) {
	int h = term_height();
	term_cursor_toggle(0);

	// Erase previous suggestions
	for (int i = len; i < h - 1; i++) {
		term_cursor_move(0, h - 1 - (i+1));
		term_clear_line();
	}

	// Redraw buffer
	term_cursor_move(0, 0);
	buffer_print(e->buf, NULL);

	// Print new suggestions
	for (int i = 0; i < len; i++) {
		term_cursor_move(offset, h - 1 - (i+1));
		if (i == sel) {
			print_foreground(EDITOR_AUTOCOMPLETE_COLOR);
			print_background(COLOR_WHITE);
		} else {
			print_foreground(COLOR_WHITE);
			print_background(EDITOR_AUTOCOMPLETE_COLOR);
		}
		printf("%-50s", list[i]);
		print_format(FORMAT_RESET);

		if (i >= h - 1) {
			break;
		}
	}

	term_cursor_toggle(1);
}

static void autocomplete_free(char** list, int len) {
	for (int i = 0; i < len; i++) {
		free(list[i]);
	}
}

char* editor_prompt(struct editor* e, char* prompt, int (*autocomplete)(char* val, char** results, int cap)) {
	term_cursor_move(0, term_height() - 1);
	term_clear_line();
	print_format(FORMAT_DIM);
	int offset = 0;
	if (prompt != NULL) {
		offset = printf("%s: ", prompt);
	}
	print_format(FORMAT_RESET);

	int accap = term_height() - 1;
	int aclen = 0;
	int acsel = -1;
	char** aclist = NULL;
	if (autocomplete != NULL) {
		aclist = (char**) malloc(accap * sizeof(char*));
		acsel = 0;
		aclen = autocomplete("", aclist, accap);
		editor_autocomplete(e, offset, aclist, aclen, acsel);
	}

	term_cursor_toggle(1);

	int len = 0;
	int cap = 32;
	char* res = (char*) malloc(cap * sizeof(char));
	while (1) {
		term_cursor_move(offset + len, term_height() - 1);

		struct event* evt = event_read(stdin);
		if (evt == NULL || evt->key == KEY_ESC) {
			free(res);
			res = NULL;
			break;
		}

		int changed = 0;
		if (evt->key == KEY_BACKSPACE) {
			if (len > 0) {
				printf("\b \b");
				len--;
			}
			res[len] = '\0';
			changed = 1;
		} else if (evt->key == KEY_ARROW_UP) {
			if (autocomplete != NULL && acsel < aclen - 1) {
				acsel++;
			}
		} else if (evt->key == KEY_ARROW_DOWN) {
			if (autocomplete != NULL && acsel > 0) {
				acsel--;
			}
		} else if (!evt->key && !evt->modifiers) {
			char32_t c = evt->ch;
			if (c == '\n') {
				break;
			} else {
				utf8_write_to(c, stdout);
			}

			if (len + UTF8_MAX_LEN + 1 > cap) {
				cap += 32;
				res = (char*) realloc(res, cap * sizeof(char));
			}

			len += utf8_format(&res[len], c);
			res[len] = '\0';
			changed = 1;
		}

		if (autocomplete != NULL) {
			if (changed) {
				autocomplete_free(aclist, aclen);
				acsel = 0;
				aclen = autocomplete(res, aclist, accap);
			}
			editor_autocomplete(e, offset, aclist, aclen, acsel);
		}
	}

	term_cursor_toggle(0);

	if (res != NULL) {
		if (acsel >= 0) {
			free(res);
			res = strdup(aclist[acsel]);
		}
		autocomplete_free(aclist, aclen);
		free(aclist);
	}
	return res;
}
