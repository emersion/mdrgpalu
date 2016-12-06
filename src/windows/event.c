static struct event* event_parse_key(KEY_EVENT_RECORD record) {
	if (record.bKeyDown == 0) {
		return NULL;
	}

	struct event* e = event_new();

	e->ch = (uint32_t) record.uChar.UnicodeChar;
	if (e->ch == '\r') {
		e->ch = '\n';
	}

	// Parse modifiers
	if (record.dwControlKeyState & SHIFT_PRESSED) {
		e->modifiers |= MODIFIER_SHIFT;
	}
	if (record.dwControlKeyState & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) {
		e->modifiers |= MODIFIER_ALT;
	}
	if (record.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) {
		e->modifiers |= MODIFIER_CTRL;
		e->ch += '@';
	}

	switch (record.wVirtualKeyCode) {
	case VK_BACK:
		e->key = KEY_BACKSPACE;
		break;
	case VK_ESCAPE:
		e->key = KEY_ESC;
		break;
	case VK_PRIOR:
		e->key = KEY_PAGE_UP;
		break;
	case VK_NEXT:
		e->key = KEY_PAGE_DOWN;
		break;
	case VK_END:
		e->key = KEY_END;
		break;
	case VK_HOME:
		e->key = KEY_HOME;
		break;
	case VK_LEFT:
		e->key = KEY_ARROW_LEFT;
		break;
	case VK_UP:
		e->key = KEY_ARROW_UP;
		break;
	case VK_RIGHT:
		e->key = KEY_ARROW_RIGHT;
		break;
	case VK_DOWN:
		e->key = KEY_ARROW_DOWN;
		break;
	case VK_INSERT:
		e->key = KEY_INSERT;
		break;
	case VK_DELETE:
		e->key = KEY_DELETE;
		break;
	}

	return e;
}

struct event* event_read(FILE* s) {
	INPUT_RECORD buf[1];

	while (1) {
		DWORD n;
		int ok = ReadConsoleInput(term_stdin, buf, sizeof(buf), &n);
		if (!ok) {
			return NULL;
		}
		if (n == 0) {
			continue;
		}

		INPUT_RECORD record = buf[0];
		switch (record.EventType) {
		case KEY_EVENT: {
			struct event* e = event_parse_key(record.Event.KeyEvent);
			if (e != NULL) {
				return e;
			}
		}
		case MOUSE_EVENT:
			break;
		case WINDOW_BUFFER_SIZE_EVENT:
			term_load_csbi();
			break;
		}
	}
}
