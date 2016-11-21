struct command {
	struct command* next;

	char* title;
	struct event* evt;
	void (*exec)(struct editor* e);
};

struct command* command_new() {
	struct command* cmd = (struct command*) malloc(sizeof(struct command));
	cmd->next = NULL;
	cmd->title = NULL;
	cmd->evt = NULL;
	cmd->exec = NULL;
	return cmd;
}

void command_free(struct command* cmd) {
	free(cmd->title);
	event_free(cmd->evt);
	free(cmd);
}

void command_move_right(struct editor* e) {
	e = e; // TODO
}

void command_move_left(struct editor* e) {
	e = e; // TODO
}

struct command* command_defaults() {
	struct command* cmd;

	cmd = command_new();
	cmd->title = "Move right";
	cmd->evt = event_new();
	cmd->evt->key = KEY_ARROW_RIGHT;
	cmd->exec = &command_move_right;

	cmd->next = command_new();
	cmd = cmd->next;
	cmd->title = "Move left";
	cmd->evt = event_new();
	cmd->evt->key = KEY_ARROW_LEFT;
	cmd->exec = &command_move_left;

	return cmd;
}
