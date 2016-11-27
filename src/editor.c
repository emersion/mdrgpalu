struct editor* editor_new() {
	struct editor* e = (struct editor*) malloc(sizeof(struct editor));
	e->buf = buffer_new();
	e->filename = NULL;
	e->status = NULL;
	e->exitcode = -1;
	return e;
}

void editor_free(struct editor* e) {
	buffer_free(e->buf);
	free(e->filename);
	free(e->status);
	free(e);
}

int editor_open(struct editor* e, char* filename) {
	FILE* f = fopen(filename, "r");
	if (f == NULL) {
		return 1;
	}

	buffer_reset(e->buf);
	buffer_read_from(e->buf, f);
	int err = ferror(f);
	fclose(f);
	if (err) {
		return err;
	}

	buffer_set_selection(e->buf, 0, 0, 0);

	free(e->filename);
	e->filename = strdup(filename);
	return 0;
}

char* editor_prompt(struct editor* e, char* prompt) {
	term_cursor_move(0, 0);
	buffer_print(e->buf, NULL);
	term_cursor_move(0, term_height()-1);
	print_format(FORMAT_DIM);
	printf("%s: ", prompt);
	print_format(FORMAT_RESET);

	term_cursor_toggle(1);

	int len = 0;
	int cap = 32;
	char* res = (char*) malloc(cap * sizeof(char));
	while (1) {
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
			continue;
		}
		if (!evt->ch) {
			continue;
		}

		wchar_t c = evt->ch;
		if (c == '\n') {
			c = 0; // Insert trailing \0
		} else {
			utf8_write_to(c, stdout);
		}

		if (len + UTF8_MAX_LEN > cap) {
			cap += 32;
			res = (char*) realloc(res, cap * sizeof(char));
		}

		len += utf8_format(&res[len], c);
		if (c == 0) {
			break;
		}
	}

	term_cursor_toggle(0);
	return res;
}

char* editor_prompt_filename(struct editor* e, char* prompt) {
	// TODO: autocomplete filename
	return editor_prompt(e, prompt);
}

void editor_set_status(struct editor* e, char* status) {
	free(e->status);
	e->status = strdup(status);
}

void editor_quit(struct editor* e) {
	e->exitcode = 0;
}

int editor_main(int argc, char** argv) {
	struct editor* e = editor_new();
	struct buffer* b = e->buf;

	if (argc == 2) {
		int err = editor_open(e, argv[1]);
		if (err && errno != ENOENT) {
			return err;
		}
	}

	struct status* s = (struct status*) malloc(sizeof(struct status));
	term_clear();
	term_cursor_move(0, 0);
	buffer_print(b, s);
	term_cursor_move(0, term_height()-1);
	status_print(s);
	term_flush();

	while (1) {
		struct event* evt = event_read(stdin);
		if (evt == NULL) {
			if (feof(stdin)) {
				return 0;
			} else if (ferror(stdin)) {
				return 1;
			} else {
				continue;
			}
		}

		if (evt->key || evt->modifiers) {
			struct command* cmd = command_match(commands, sizeof(commands)/sizeof(commands[0]), evt);
			if (cmd != NULL) {
				cmd->exec(e, evt);
			}
		} else {
			buffer_insert_char(b, evt->ch);
		}

		event_free(evt);

		if (e->exitcode >= 0) {
			return e->exitcode;
		}

		term_clear();
		term_cursor_move(0, 0);
		buffer_print(b, s);
		term_cursor_move(0, term_height()-1);
		if (e->status != NULL) {
			print_format(FORMAT_DIM);
			printf("%s", e->status);
			editor_set_status(e, NULL);
		} else {
			status_print(s);
		}
		term_flush();
	}
}
