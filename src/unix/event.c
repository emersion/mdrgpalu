struct event* event_read(FILE* s) {
	struct event* e;
	char c;
	char prev = -1;
	while (1) {
		c = fgetc(s);
		if (feof(s)) {
			return NULL;
		} else if (ferror(stdin)) {
			return NULL;
		}

		if (prev == '\033' && c == '[') { // Escape sequence
			struct sequence* s = sequence_parse();
			if (s == NULL) {
				return NULL;
			}

			int modifiers = parse_modifiers(s->params[1]);

			int key = 0;
			switch (s->code) {
			case CODE_CUU:
				key = KEY_ARROW_UP;
				break;
			case CODE_CUD:
				key = KEY_ARROW_DOWN;
				break;
			case CODE_CUF:
				key = KEY_ARROW_RIGHT;
				break;
			case CODE_CUB:
				key = KEY_ARROW_LEFT;
				break;
			case CODE_CPL:
				key = KEY_END;
				break;
			case CODE_CUP:
				key = KEY_HOME;
				break;
			case CODE_DECDC:
				key = KEY_DELETE;
				break;
			default:
				free(s);
				return NULL;
			}
			free(s);

			e = event_new();
			e->key = key;
			e->modifiers = modifiers;
			return e;
		} else if (prev == '\033') {
			ungetc(c, s);

			e = event_new();
			e->key = KEY_ESC;
			return e;
		} else if (c != '\033') { // Not the begining of an escape sequence
			if (c == '\r') {
				return NULL;
			}

			e = event_new();
			if (c == 127) {
				e->key = KEY_BACKSPACE;
			} else if (c == '\t' || c == '\n' || c >= ' ') {
				e->ch = c;
			} else {
				e->modifiers = MODIFIER_CTRL;
				e->ch = c + '@';
			}
			return e;
		}

		prev = c;
	}
}
