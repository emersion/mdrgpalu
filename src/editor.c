struct editor* editor_new() {
	struct editor* e = (struct editor*) malloc(sizeof(struct editor));
	e->buf = buffer_new();
	e->filename = NULL;
	e->saved = 1;
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
	e->saved = 1;
	return 0;
}

int editor_save(struct editor* e) {
	if (e->filename == NULL) {
		e->filename = editor_prompt_filename(e, "Save as");
		if (e->filename == NULL) {
			return -1;
		}
	}

	FILE* f = fopen(e->filename, "w+");
	if (f == NULL) {
		editor_set_status(e, "Cannot open file");
		return 1;
	}

	int err = buffer_write_to(e->buf, f);
	fclose(f);
	if (err) {
		editor_set_status(e, "Error while writing file");
		return 1;
	}

	editor_set_status(e, "File saved");
	e->saved = 1;
	return 0;
}

char* editor_prompt_filename(struct editor* e, char* prompt) {
	// TODO: autocomplete filename
	return editor_prompt(e, prompt, NULL);
}

int editor_prompt_bool(struct editor* e, char* prompt) {
	while (1) {
		char* res = editor_prompt(e, prompt, NULL);
		if (res == NULL) {
			continue;
		}

		int v = -1;
		if (strcmp(res, "y") == 0) {
			v = 1;
		} else if (strcmp(res, "n") == 0) {
			v = 0;
		}

		free(res);
		if (v >= 0) {
			return v;
		}
	}
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
				e->exitcode = 0;
				break;
			} else if (ferror(stdin)) {
				e->exitcode = 1;
				break;
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
			e->saved = 0;
		}

		event_free(evt);

		if (e->exitcode >= 0) {
			break;
		}

		editor_print(e);
	}

	int exitcode = e->exitcode;
	editor_free(e);
	return exitcode;
}
