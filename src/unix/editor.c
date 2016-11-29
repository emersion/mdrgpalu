void editor_print(struct editor* e) {
	struct status* s = (struct status*) malloc(sizeof(struct status));
	s->filename = e->filename;

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

static void editor_autocomplete(struct editor* e, int offset, int* prevlen, struct trie_list* list) {
	int h = term_height();

	// Erase previous suggestions
	for (int i = 0; i < *prevlen; i++) {
		term_cursor_move(offset, h - 1 - (i+1));
		printf("%-50s", "");
	}

	// Redraw buffer
	term_cursor_move(0, 0);
	buffer_print(e->buf, NULL);

	// Print new suggestions
	int i = 0;
	for (struct trie_list* item = list; item != NULL; item = item->next) {
		term_cursor_move(offset, h - 1 - (i+1));
		print_foreground(COLOR_WHITE);
		print_background(COLOR_BLUE);
		printf("%-50s", item->str);
		print_format(FORMAT_RESET);
		i++;

		if (h - 1 < i + 1) {
			break;
		}
	}
	*prevlen = i+1;
}

char* editor_prompt(struct editor* e, char* prompt, struct trie_list* (*autocomplete)(char* val)) {
	term_cursor_move(0, term_height() - 1);
	term_clear_line();
	print_format(FORMAT_DIM);
	int offset = 0;
	if (prompt != NULL) {
		offset = printf("%s: ", prompt);
	}
	print_format(FORMAT_RESET);

	struct trie_list* list = NULL;
	int prevlistlen = 0;
	if (autocomplete != NULL) {
		list = autocomplete("");
		editor_autocomplete(e, offset, &prevlistlen, list);
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

		if (evt->key == KEY_BACKSPACE) {
			if (len > 0) {
				printf("\b \b");
				len--;
			}
			res[len] = '\0';
		} else if (!evt->key && !evt->modifiers) {
			wchar_t c = evt->ch;
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
		}

		if (autocomplete != NULL) {
			list = autocomplete(res);
			editor_autocomplete(e, offset, &prevlistlen, list);
		}
	}

	term_cursor_toggle(0);
	return res;
}
