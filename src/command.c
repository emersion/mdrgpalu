struct command* command_match(struct command commands[], int len, struct event* evt) {
	for (int i = 0; i < len; i++) {
		if (event_equal(evt, commands[i].evt)) {
			return &commands[i];
		}
	}
	return NULL;
}

void command_move(struct editor* e, struct event* evt);
void command_move_home(struct editor* e, struct event* evt);
void command_move_end(struct editor* e, struct event* evt);
void command_delete(struct editor* e, struct event* evt);
void command_select_all(struct editor* e, struct event* evt);
void command_cut_copy(struct editor* e, struct event* evt);
void command_go_to_line(struct editor* e, struct event* evt);
void command_select_line(struct editor* e, struct event* evt);
void command_quit(struct editor* e, struct event* evt);
void command_open(struct editor* e, struct event* evt);
void command_save(struct editor* e, struct event* evt);
void command_paste(struct editor* e, struct event* evt);
void command_palette(struct editor* e, struct event* evt);

struct command commands[] = {
	{ .title = "Move left", .evt = &(struct event) { .key = KEY_ARROW_LEFT }, .exec = command_move },
	{ .title = "Move right", .evt = &(struct event) { .key = KEY_ARROW_RIGHT }, .exec = command_move },
	{ .title = "Move up", .evt = &(struct event) { .key = KEY_ARROW_UP }, .exec = command_move },
	{ .title = "Move down", .evt = &(struct event) { .key = KEY_ARROW_DOWN }, .exec = command_move },
	{ .title = "Move to begining of word", .evt = &(struct event) { .key = KEY_ARROW_LEFT, .modifiers = MODIFIER_CTRL }, .exec = command_move },
	{ .title = "Move to end of word", .evt = &(struct event) { .key = KEY_ARROW_RIGHT, .modifiers = MODIFIER_CTRL }, .exec = command_move },
	{ .title = "Move line up", .evt = &(struct event) { .key = KEY_ARROW_UP, .modifiers = MODIFIER_CTRL }, .exec = command_move },
	{ .title = "Move line down", .evt = &(struct event) { .key = KEY_ARROW_DOWN, .modifiers = MODIFIER_CTRL }, .exec = command_move },
	{ .title = "Select left", .evt = &(struct event) { .key = KEY_ARROW_LEFT, .modifiers = MODIFIER_SHIFT }, .exec = command_move },
	{ .title = "Select right", .evt = &(struct event) { .key = KEY_ARROW_RIGHT, .modifiers = MODIFIER_SHIFT }, .exec = command_move },
	{ .title = "Select to begining of word", .evt = &(struct event) { .key = KEY_ARROW_LEFT, .modifiers = MODIFIER_SHIFT | MODIFIER_CTRL }, .exec = command_move },
	{ .title = "Select to end of word", .evt = &(struct event) { .key = KEY_ARROW_RIGHT, .modifiers = MODIFIER_SHIFT | MODIFIER_CTRL }, .exec = command_move },
	{ .title = "Move to begining of line", .evt = &(struct event) { .key = KEY_HOME }, .exec = command_move_home },
	{ .title = "Move to end of line", .evt = &(struct event) { .key = KEY_END }, .exec = command_move_end },
	{ .title = "Select to begining of line", .evt = &(struct event) { .key = KEY_HOME, .modifiers = MODIFIER_SHIFT }, .exec = command_move_home },
	{ .title = "Select to end of line", .evt = &(struct event) { .key = KEY_END, .modifiers = MODIFIER_SHIFT }, .exec = command_move_end },
	{ .title = "Move to begining of file", .evt = &(struct event) { .key = KEY_HOME, .modifiers = MODIFIER_CTRL }, .exec = command_move_home },
	{ .title = "Move to end of file", .evt = &(struct event) { .key = KEY_END, .modifiers = MODIFIER_CTRL }, .exec = command_move_end },
	//{ .title = "Select to begining of file", .evt = &(struct event) { .key = KEY_HOME, .modifiers = MODIFIER_SHIFT | MODIFIER_CTRL }, .exec = command_move_home },
	//{ .title = "Select to end of file", .evt = &(struct event) { .key = KEY_END, .modifiers = MODIFIER_SHIFT | MODIFIER_CTRL }, .exec = command_move_end },
	{ .title = "Backspace", .evt = &(struct event) { .key = KEY_BACKSPACE }, .exec = command_delete },
	{ .title = "Delete", .evt = &(struct event) { .key = KEY_DELETE }, .exec = command_delete },
	//{ .title = "Delete to begining of word", .evt = &(struct event) { .key = KEY_BACKSPACE, .modifiers = MODIFIER_CTRL }, .exec = command_delete },
	//{ .title = "Delete to end of word", .evt = &(struct event) { .key = KEY_DELETE, .modifiers = MODIFIER_CTRL }, .exec = command_delete },

	{ .title = "Select all", .evt = &(struct event) { .modifiers = MODIFIER_CTRL, .ch = 'A' }, .exec = command_select_all },
	{ .title = "Copy", .evt = &(struct event) { .modifiers = MODIFIER_CTRL, .ch = 'C' }, .exec = command_cut_copy },
	{ .title = "Go to line", .evt = &(struct event) { .modifiers = MODIFIER_CTRL, .ch = 'G' }, .exec = command_go_to_line },
	{ .title = "Select line", .evt = &(struct event) { .modifiers = MODIFIER_CTRL, .ch = 'L' }, .exec = command_select_line },
	{ .title = "Open file", .evt = &(struct event) { .modifiers = MODIFIER_CTRL, .ch = 'O' }, .exec = command_open },
	{ .title = "Command palette", .evt = &(struct event) { .modifiers = MODIFIER_CTRL, .ch = 'P' }, .exec = command_palette },
	{ .title = "Quit", .evt = &(struct event) { .modifiers = MODIFIER_CTRL, .ch = 'Q' }, .exec = command_quit },
	{ .title = "Save file", .evt = &(struct event) { .modifiers = MODIFIER_CTRL, .ch = 'S' }, .exec = command_save },
	{ .title = "Paste", .evt = &(struct event) { .modifiers = MODIFIER_CTRL, .ch = 'V' }, .exec = command_paste },
	{ .title = "Close", .evt = &(struct event) { .modifiers = MODIFIER_CTRL, .ch = 'W' }, .exec = command_quit },
	{ .title = "Cut", .evt = &(struct event) { .modifiers = MODIFIER_CTRL, .ch = 'X' }, .exec = command_cut_copy },
};

void command_move(struct editor* e, struct event* evt) {
	struct buffer* b = e->buf;

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
}

void command_move_home(struct editor* e, struct event* evt) {
	struct buffer* b = e->buf;

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
}

void command_move_end(struct editor* e, struct event* evt) {
	struct buffer* b = e->buf;

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
}

void command_delete(struct editor* e, struct event* evt) {
	struct buffer* b = e->buf;

	if (b->sel->len > 0) {
		buffer_delete_selection(b);
	} else {
		int delta = 0;
		if (evt->key == KEY_BACKSPACE) {
			delta = -1;
		}
		buffer_delete_char(b, delta);
	}
}

void command_select_all(struct editor* e, struct event* evt) {
	buffer_set_selection(e->buf, 0, 0, buffer_len(e->buf));
}

void command_cut_copy(struct editor* e, struct event* evt) {
	struct buffer* b = e->buf;

	FILE* f = clipboard_open("w");
	if (f == NULL) {
		editor_set_status(e, "Cannot open clipboard");
		return;
	}

	int err;
	if (b->sel->len == 0) {
		err = line_write_to(b->sel->line, f);
	} else {
		err = buffer_write_selection_to(b, f);
	}
	clipboard_close(f);
	if (err) {
		editor_set_status(e, "Error while writing to clipboard");
		return;
	}

	if (evt->ch == 'X') {
		if (b->sel->len == 0) {
			buffer_delete_line(b, b->sel->line);
		} else {
			buffer_delete_selection(b);
		}
	}
}

void command_go_to_line(struct editor* e, struct event* evt) {
	char* s = editor_prompt(e, "Go to line", NULL);
	if (s == NULL) {
		return;
	}
	buffer_set_position_string(e->buf, s);
	free(s);
}

void command_select_line(struct editor* e, struct event* evt) {
	struct selection* s = e->buf->sel;
	s->ch = 0;
	selection_set_len(s, s->line->len + 1);
}

void command_quit(struct editor* e, struct event* evt) {
	editor_quit(e);
}

void command_open(struct editor* e, struct event* evt) {
	char* filename = editor_prompt_filename(e, "Open file");
	if (filename == NULL) {
		return;
	}

	int err = editor_open(e, filename);
	free(filename);
	if (err) {
		if (errno == ENOENT) {
			editor_set_status(e, "File not found");
		} else {
			editor_set_status(e, "Cannot open file");
		}
	}
}

void command_save(struct editor* e, struct event* evt) {
	if (e->filename == NULL) {
		e->filename = editor_prompt_filename(e, "Save as");
	}
	if (e->filename != NULL) {
		FILE* f = fopen(e->filename, "w+");
		if (f == NULL) {
			editor_set_status(e, "Cannot open file");
			return;
		}

		int err = buffer_write_to(e->buf, f);
		fclose(f);
		if (err) {
			editor_set_status(e, "Error while writing file");
			return;
		}

		editor_set_status(e, "File saved");
	}
}

void command_paste(struct editor* e, struct event* evt) {
	FILE* f = clipboard_open("r");
	if (f == NULL) {
		editor_set_status(e, "Cannot open clipboard");
		return;
	}

	int n = buffer_read_from(e->buf, f);
	clipboard_close(f);
	if (n == EOF) {
		editor_set_status(e, "Error while reading clipboard");
	}
}

struct trie_node* commands_tree = NULL;

static struct trie_list* command_palette_autocomplete(char* val) {
	struct trie_node* node = trie_node_match(commands_tree, val, strlen(val));
	return trie_node_list(node);
}

void command_palette(struct editor* e, struct event* evt) {
	if (commands_tree == NULL) {
		for (unsigned int i = 0; i < sizeof(commands)/sizeof(commands[0]); i++) {
			char* s = commands[i].title;
			struct trie_node* node = trie_node_insert(&commands_tree, s, strlen(s));
			node->val = &commands[i];
		}
	}

	char* res = editor_prompt(e, NULL, &command_palette_autocomplete);
	if (res == NULL) {
		return;
	}

	struct command* cmd = (struct command*) trie_node_val(commands_tree, res, strlen(res));
	free(res);
	if (cmd != NULL) {
		cmd->exec(e, cmd->evt);
	}
}
