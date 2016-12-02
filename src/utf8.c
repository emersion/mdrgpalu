#define UTF8_REPLACEMENT_CODEPOINT 0xFFFD

#define UTF8_CONTINUATION_BYTE 0
#define UTF8_SINGLE_BYTE 1

#define UTF8_MAX_LEN 4

static const unsigned char utf8_mask[] = {
	0x7F,
	0x1F,
	0x0F,
	0x07
};

size_t utf8_len(unsigned char c) {
	if ((c & (1 << 7)) == 0) {
		return UTF8_SINGLE_BYTE;
	}
	if ((c & (1 << 6)) == 0) {
		return UTF8_CONTINUATION_BYTE;
	}
	if ((c & (1 << 5)) == 0) {
		return 2;
	}
	if ((c & (1 << 4)) == 0) {
		return 3;
	}
	return 4;
}

size_t utf8_format(char* str, wchar_t codepoint) {
	size_t len = 0;
	int first;
	if (codepoint < 0x80) {
		first = 0;
		len = 1;
	} else if (codepoint < 0x800) {
		first = 0xC0;
		len = 2;
	} else if (codepoint < 0x10000) {
		first = 0xE0;
		len = 3;
	} else {
		first = 0xF0;
		len = 4;
	}

	for (int i = len-1; i > 0; i--) {
		str[i] = (codepoint & 0x3F) | 0x80;
		codepoint >>= 6;
	}
	str[0] = codepoint | first;

	return len;
}

int utf8_write_to(wchar_t codepoint, FILE* s) {
	char str[UTF8_MAX_LEN];
	size_t len = utf8_format((char*) &str, codepoint);
	size_t n = fwrite(&str, sizeof(char), len, s);
	if (n != len) {
		return 1;
	}
	return 0;
}

int utf8_read_from(wchar_t* codepoint, FILE* s) {
	int c = fgetc(s);
	if (c == EOF) {
		return EOF;
	}
	*codepoint = (wchar_t) c;

	size_t len = utf8_len(c);
	if (len == UTF8_CONTINUATION_BYTE) {
		*codepoint = UTF8_REPLACEMENT_CODEPOINT;
		return 1;
	}

	*codepoint &= utf8_mask[len-1];
	for (size_t i = 1; i < len; i++) {
		c = fgetc(s);
		if (c == EOF || utf8_len(c) != UTF8_CONTINUATION_BYTE) {
			*codepoint = UTF8_REPLACEMENT_CODEPOINT;
			if (c == EOF) {
				return EOF;
			}
			return i;
		}

		*codepoint <<= 6;
		*codepoint |= c & 0x3F;
	}

	return len;
}
