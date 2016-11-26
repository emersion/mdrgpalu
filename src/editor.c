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
		if (errno == ENOENT) {
			return 0;
		}
		return 1;
	}

	buffer_read_from(e->buf, f);
	int err = ferror(f);
	fclose(f);
	if (err) {
		return err;
	}

	buffer_set_selection(e->buf, 0, 0, 0);
	e->filename = filename;
	return 0;
}

char* editor_prompt(struct editor* e, char* prompt) {
	buffer_print(e->buf, NULL);
	print_format(FORMAT_DIM, prompt);
	printf(" ");

	term_cursor_toggle(1);

	int len = 0;
	int cap = 32;
	char* res = (char*) malloc(cap);
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

		char c = evt->ch;
		if (evt->ch == '\n') {
			c = 0; // Insert trailing \0
		} else {
			printf("%c", c);
		}

		if (len + 1 > cap) {
			cap += 32;
			res = (char*) realloc(res, cap);
		}

		res[len] = c;
		len++;
		if (c == 0) {
			break;
		}
	}

	term_cursor_toggle(0);
	return res;
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
		if (err) {
			return err;
		}
	}

	struct status* s = (struct status*) malloc(sizeof(struct status));
	buffer_print(b, s);
	status_print(s);

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
			struct command* cmd = command_match((struct command**) &commands, sizeof(commands)/sizeof(commands[0]), evt);
			if (cmd != NULL) {
				cmd->exec(e, evt);
			}
		} else {
			buffer_insert_char(b, evt->ch);
		}

		event_free(evt);

		buffer_print(b, s);
		if (e->status != NULL) {
			print_format(FORMAT_DIM, e->status);
			editor_set_status(e, NULL);
		} else {
			status_print(s);
		}
	}
}
