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

int event_equal(struct event* evt, struct event* other) {
	return (evt->ch == other->ch && evt->key == other->key && evt->modifiers == other->modifiers);
}

size_t event_string(struct event* evt, char* s) {
	char* p = s;

	if (evt->modifiers & MODIFIER_CTRL) {
		p = stpcpy(p, "Ctrl+");
	}
	if (evt->modifiers & MODIFIER_ALT) {
		p = stpcpy(p, "Alt+");
	}
	if (evt->modifiers & MODIFIER_SHIFT) {
		p = stpcpy(p, "Shift+");
	}
	if (evt->key) {
		switch (evt->key) {
		case KEY_ARROW_UP:
			p = stpcpy(p, "Arrow Up");
			break;
		case KEY_ARROW_DOWN:
			p = stpcpy(p, "Arrow Down");
			break;
		case KEY_ARROW_LEFT:
			p = stpcpy(p, "Arrow Left");
			break;
		case KEY_ARROW_RIGHT:
			p = stpcpy(p, "Arrow Right");
			break;
		case KEY_HOME:
			p = stpcpy(p, "Home");
			break;
		case KEY_END:
			p = stpcpy(p, "End");
			break;
		case KEY_DELETE:
			p = stpcpy(p, "Delete");
			break;
		case KEY_BACKSPACE:
			p = stpcpy(p, "Backspace");
			break;
		case KEY_ESC:
			p = stpcpy(p, "Esc");
			break;
		}
	} else if (evt->ch) {
		p[0] = evt->ch;
		p[1] = '\0';
		p += 2 * sizeof(char);
	}

	return p - s;
}

struct event* event_read(FILE* s);
