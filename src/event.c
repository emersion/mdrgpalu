#define KEY_ARROW_UP 1
#define KEY_ARROW_DOWN 2
#define KEY_ARROW_RIGHT 3
#define KEY_ARROW_LEFT 4
#define KEY_END 5
#define KEY_HOME 6
#define KEY_DELETE 7
#define KEY_BACKSPACE 8
#define KEY_ESC 9

#define MODIFIER_SHIFT 1<<1
#define MODIFIER_ALT 1<<2
#define MODIFIER_CTRL 1<<3

struct event {
	wchar_t ch;

	int key;
	int modifiers;
};

struct event* event_new() {
	struct event* evt = (struct event*) malloc(sizeof(struct event));
	evt->ch = 0;
	evt->key = 0;
	evt->modifiers = 0;
	return evt;
}

void event_free(struct event* evt) {
	free(evt);
}

struct event* event_read(FILE* s);
