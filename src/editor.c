struct editor {
	struct buffer* buf;
	char* filename;
};

struct editor* editor_new() {
	struct editor* e = (struct editor*) malloc(sizeof(struct editor));
	e->buf = buffer_new();
	e->filename = NULL;
	return e;
}

void editor_free(struct editor* e) {
	buffer_free(e->buf);
	free(e->filename);
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

	print_escape(CODE_CURSOR_SHOW);

	int len = 0;
	int cap = 32;
	char* res = (char*) malloc(cap);
	while (1) {
		char c = fgetc(stdin);
		// TODO: error handling

		// TODO: support Backspace
		if (c == '\r') {
			continue;
		} else if (c == '\n') {
			c = 0; // Insert trailing \0
		} else if (c == '\033') { // Esc
			free(res);
			res = NULL;
			break;
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

	print_escape(CODE_CURSOR_HIDE);
	return res;
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

	char c;
	int prev = -1;
	char* statustext = NULL;
	while (1) {
		c = fgetc(stdin);
		if (feof(stdin)) {
			return 0;
		} else if (ferror(stdin)) {
			return 1;
		}

		if (prev == '\033' && c == '[') { // Escape sequence
			struct sequence* s = sequence_parse();
			if (s == NULL) {
				printf("Cannot parse escape sequence\n");
				continue;
			}

			switch (s->code) {
			case CODE_CUU:
			case CODE_CUD:
			case CODE_CUF:
			case CODE_CUB:; // Arrow
				int delta = s->params[0];
				if (delta == 0) {
					delta = 1;
				}
				int modifiers = s->params[1];

				int i = 0, j = 0;
				switch (s->code) {
				case CODE_CUU:
					i = -delta;
					break;
				case CODE_CUD:
					i = delta;
					break;
				case CODE_CUF:
					j = delta;
					break;
				case CODE_CUB:
					j = -delta;
					break;
				}

				if (modifiers == MODIFIER_CTRL_SHIFT) {
					if (j != 0) {
						buffer_extend_jump_selection(b, j);
					}
				} else if (modifiers == MODIFIER_CTRL) {
					if (j != 0) {
						buffer_jump_selection(b, j);
					} else if (i != 0) {
						struct line* other = line_walk_line(b->sel->line, i);
						buffer_swap_lines(b, b->sel->line, other);
					}
				} else if (modifiers == MODIFIER_SHIFT) {
					buffer_extend_selection(b, i, j);
				} else {
					if (b->sel->len > 0) {
						buffer_shrink_selection(b, i + j);
					} else {
						buffer_move_selection(b, i, j);
					}
				}
				break;
			case CODE_CPL:
				if (s->params[0] == 0) { // End
					b->sel->ch = b->sel->line->len;
				} else {
					// TODO
				}
				break;
			case CODE_CUP:
				if (s->params[0] == 0 && s->params[1] == 0) { // Home
					b->sel->ch = 0;
				} else {
					// TODO
				}
				break;
			case CODE_DECDC:
				//int modifier = s->params[1]; // TODO: support Ctrl
				buffer_delete_char(b, 0);
				break;
			}

			sequence_free(s);
		} else if (c == '\033') { // Esc
			prev = c;
			continue;
		} else if (c == '\t' || c == '\n' || c >= ' ') {
			buffer_insert_char(b, c);
		} else {
			switch (c) {
				case 1: // Ctrl+A
					buffer_set_selection(b, 0, 0, buffer_len(b));
					break;
				case 3: // Ctrl+C
				case 24: { // Ctrl+X
					FILE* f = clipboard_open("w");
					if (f == NULL) {
						return 1;
					}
					int err;
					if (b->sel->len == 0) {
						err = line_write_to(b->sel->line, f);
					} else {
						err = buffer_write_selection_to(b, f);
					}
					clipboard_close(f);
					if (err) {
						return err;
					}

					if (c == 24) { // Cut
						if (b->sel->len == 0) {
							buffer_delete_line(b, b->sel->line);
						} else {
							buffer_delete_selection(b);
						}
					}
					break;
				}
				case 7:; // Ctrl+G
					char* s = editor_prompt(e, "Go to line: ");
					if (s == NULL) {
						break;
					}
					buffer_set_position_string(b, s);
					free(s);
					break;
				case 17: // Ctrl+Q
				case 23: // Ctrl+W
					return 0;
				case 19: // Ctrl+S
					if (e->filename != NULL) {
						FILE* f = fopen(e->filename, "w+");
						if (f == NULL) {
							return 1;
						}
						int err = buffer_write_to(b, f);
						fclose(f);
						if (err) {
							return err;
						}
						statustext = strdup("File saved.");
					}
					break;
				case 22: { // Ctrl+V
					FILE* f = clipboard_open("r");
					if (f == NULL) {
						return 1;
					}
					buffer_read_from(b, f);
					int err = ferror(f);
					clipboard_close(f);
					if (err) {
						return err;
					}
					break;
				}
			}
		}

		buffer_print(b, s);
		if (statustext != NULL) {
			print_format(FORMAT_DIM, statustext);
			free(statustext);
			statustext = NULL;
		} else {
			status_print(s);
		}

		prev = c;
	}
}
