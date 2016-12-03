// Event keys.
#define KEY_ARROW_UP 1
#define KEY_ARROW_DOWN 2
#define KEY_ARROW_RIGHT 3
#define KEY_ARROW_LEFT 4
#define KEY_END 5
#define KEY_HOME 6
#define KEY_DELETE 7
#define KEY_BACKSPACE 8
#define KEY_ESC 9

// Event modifiers.
#define MODIFIER_SHIFT 1<<1
#define MODIFIER_ALT 1<<2
#define MODIFIER_CTRL 1<<3

// An event.
struct event {
	// If non-zero, contains the typed character.
	char32_t ch;
	// If non-zero, contains the typed key.
	int key;
	// Contains the logical conjunction of modifiers pressed with ch and key.
	int modifiers;
};

// event_new allocates a new event.
struct event* event_new() {
	struct event* evt = (struct event*) malloc(sizeof(struct event));
	evt->ch = 0;
	evt->key = 0;
	evt->modifiers = 0;
	return evt;
}

// event_free deallocates an event.
void event_free(struct event* evt) {
	free(evt);
}

// event_equal checks if evt and other are equal.
int event_equal(struct event* evt, struct event* other) {
	return (evt->ch == other->ch && evt->key == other->key && evt->modifiers == other->modifiers);
}

// event_read reads a single event from the stream s. It returns NULL on error.
struct event* event_read(FILE* s);
