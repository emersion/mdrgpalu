// A command is an action triggered when an event occurs.
struct command {
	// The command's name.
	char* title;
	// The event that triggers this command.
	struct event* evt;
	// A function called when this command is executed.
	void (*exec)(struct editor* e, struct event* evt);
};

// A list of default commands.
struct command commands[];

// command_match takes a list of commands and an event, and returns a command
// that matches this event. It returns NULL if no such command is found.
struct command* command_match(struct command commands[], int len, struct event* evt);
