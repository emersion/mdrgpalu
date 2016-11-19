#define KEY_ARROW_UP 1
#define KEY_ARROW_DOWN 2
#define KEY_ARROW_RIGHT 3
#define KEY_ARROW_LEFT 4
#define KEY_END 5
#define KEY_HOME 6
#define KEY_DELETE 7
#define KEY_BACKSPACE 8

#define MODIFIER_SHIFT 1<<1
#define MODIFIER_ALT 1<<2
#define MODIFIER_CTRL 1<<3

struct event {
	char ch;

	int key;
	int modifiers;
};

struct event* event_new() {
	struct event* e = (struct event*) malloc(sizeof(struct event));
	e->ch = 0;
	e->key = 0;
	e->modifiers = 0;
	return e;
}
