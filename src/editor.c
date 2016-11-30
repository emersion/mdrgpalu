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

char* editor_prompt_filename(struct editor* e, char* prompt) {
	// TODO: autocomplete filename
	return editor_prompt(e, prompt, NULL);
}

void editor_set_status(struct editor* e, char* status) {
	free(e->status);
	if (status != NULL) {
		status = strdup(status);
	}
	e->status = status;
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

	editor_print(e);

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

		editor_print(e);
	}
}
