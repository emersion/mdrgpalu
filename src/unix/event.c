struct event* event_read(FILE* s) {
	struct event* e = NULL;
	wchar_t c = 0;
	wchar_t prev = 0;
	while (1) {
		int n = utf8_read_from(&c, s);
		if (n == EOF) {
			return NULL;
		}

		if (prev == CSI[0] && c == CSI[1]) { // This is a control sequence
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
		} else if (prev == CSI[0]) { // Esc
			// TODO: detect Esc key if no more data is available
			//ungetc(c, s);

			e = event_new();
			e->key = KEY_ESC;
			return e;
		} else if (c != CSI[0]) { // Not the begining of a Control Sequence Introducer
			if (c == '\r') {
				return NULL;
			}

			e = event_new();
			if (c == 127) { // Backspace
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
