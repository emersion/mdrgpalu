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
	buffer_print(e->buf, NULL);
	print_format(FORMAT_DIM);
	printf("%s: ", prompt);
	print_format(FORMAT_RESET);

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

char* editor_prompt_filename(struct editor* e, char* prompt) {
	// TODO: autocomplete filename
	return editor_prompt(e, prompt);
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
	buffer_print(b, s);
	status_print(s);

	term_clear();
	term_cursor_move(0, 0);
	buffer_print(b, s);
	term_cursor_move(0, term_height()-1);
	status_print(s);
	term_flush();

	struct event* evt;
	char* statustext = NULL;
	while (1) {
		evt = event_read(stdin);
		if (evt == NULL) {
			if (feof(stdin)) {
				return 0;
			} else if (ferror(stdin)) {
				return 1;
			} else {
				continue;
			}
		}

		if (evt->key) {
			switch (evt->key) {
				case KEY_ARROW_UP:
				case KEY_ARROW_DOWN:
				case KEY_ARROW_RIGHT:
				case KEY_ARROW_LEFT: {
					int delta = 1;
					int i = 0, j = 0;
					switch (evt->key) {
					case KEY_ARROW_UP:
						i = -delta;
						break;
					case KEY_ARROW_DOWN:
						i = delta;
						break;
					case KEY_ARROW_RIGHT:
						j = delta;
						break;
					case KEY_ARROW_LEFT:
						j = -delta;
						break;
					}

					if (evt->modifiers == (MODIFIER_CTRL | MODIFIER_SHIFT)) {
						if (j != 0) {
							buffer_extend_jump_selection(b, j);
						}
					} else if (evt->modifiers == MODIFIER_CTRL) {
						if (j != 0) {
							buffer_jump_selection(b, j);
						} else if (i != 0) {
							struct line* other = line_walk_line(b->sel->line, i);
							buffer_swap_lines(b, b->sel->line, other);
						}
					} else if (evt->modifiers == MODIFIER_SHIFT) {
						buffer_extend_selection(b, i, j);
					} else {
						if (b->sel->len > 0) {
							buffer_shrink_selection(b, i + j);
						} else {
							buffer_move_selection(b, i, j);
						}
					}
					break;
				}
				case KEY_END: {
					if (evt->modifiers == MODIFIER_CTRL) {
						b->sel->line = b->last;
						b->sel->ch = b->sel->line->len;
					} else if (evt->modifiers == MODIFIER_SHIFT) {
						int ch = b->sel->ch;
						if (ch > b->sel->line->len) {
							ch = b->sel->line->len;
						}
						b->sel->len = b->sel->line->len - ch;
					} else {
						b->sel->ch = b->sel->line->len;
					}
					break;
				}
				case KEY_HOME: {
					if (evt->modifiers == MODIFIER_CTRL) {
						b->sel->line = b->first;
						b->sel->ch = 0;
					} else if (evt->modifiers == MODIFIER_SHIFT) {
						int len = b->sel->ch;
						if (len > b->sel->line->len) {
							len = b->sel->line->len;
						}
						b->sel->ch = 0;
						b->sel->len = len;
					} else {
						b->sel->ch = 0;
					}
					break;
				}
				case KEY_DELETE:
				case KEY_BACKSPACE: {
					// TODO: support Ctrl
					if (b->sel->len > 0) {
						buffer_delete_selection(b);
					} else {
						int delta = 0;
						if (evt->key == KEY_BACKSPACE) {
							delta = -1;
						}
						buffer_delete_char(b, delta);
					}
					break;
				}
			}
		} else if (evt->modifiers) {
			if (evt->modifiers == MODIFIER_CTRL) {
				switch (evt->ch) {
					case 'A': {
						buffer_set_selection(b, 0, 0, buffer_len(b));
						break;
					}
					case 'C':
					case 'X': {
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

						if (evt->ch == 'X') {
							if (b->sel->len == 0) {
								buffer_delete_line(b, b->sel->line);
							} else {
								buffer_delete_selection(b);
							}
						}
						break;
					}
					case 'G': {
						char* s = editor_prompt(e, "Go to line");
						if (s == NULL) {
							break;
						}
						buffer_set_position_string(b, s);
						free(s);
						break;
					}
					case 'L': {
						b->sel->ch = 0;
						selection_set_len(b->sel, b->sel->line->len + 1);
						break;
					}
					case 'O': {
						char* filename = editor_prompt_filename(e, "Open file");
						if (filename == NULL) {
							break;
						}
						int err = editor_open(e, filename);
						free(filename);
						if (err) {
							if (errno == ENOENT) {
								statustext = strdup("File not found.");
							} else {
								return err;
							}
						}
						break;
					}
					case 'Q':
					case 'W': {
						return 0;
					}
					case 'S': {
						if (e->filename == NULL) {
							e->filename = editor_prompt_filename(e, "Save as");
						}
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
					}
					case 'V': {
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
		} else {
			buffer_insert_char(b, evt->ch);
		}

		term_clear();
		term_cursor_move(0, 0);
		buffer_print(b, s);
		term_cursor_move(0, term_height()-1);
		if (statustext != NULL) {
			print_format(FORMAT_DIM);
			printf("%s", statustext);
			print_format(FORMAT_RESET);
			free(statustext);
			statustext = NULL;
		} else {
			status_print(s);
		}
		term_flush();

		event_free(evt);
	}
}
