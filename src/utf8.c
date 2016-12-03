// UTF8_REPLACEMENT_CODEPOINT is the UTF-8 codepoint used to represent an
// invalid character.
#define UTF8_REPLACEMENT_CODEPOINT 0xFFFD

// UTF8_CONTINUATION_BYTE is returned by utf8_len when the provided char is a
// continuation byte (ie. we're in the middle of an UTF-8 codepoint).
#define UTF8_CONTINUATION_BYTE 0

// UTF8_MAX_LEN is the maximum number of bytes used in a UTF-8 codepoint.
#define UTF8_MAX_LEN 4

static const unsigned char utf8_mask[] = {
	0x7F,
	0x1F,
	0x0F,
	0x07
};

// utf8_len returns the number of bytes used by the codepoint begining with c.
// If c is part of a codepoint but is not the first byte, it returns
// UTF8_CONTINUATION_BYTE. If c is not UTF-8-encoded, it returns 1.
size_t utf8_len(unsigned char c) {
	if ((c & (1 << 7)) == 0) {
		return 1;
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

// utf8_format encodes codepoint to str. It returns the number of bytes written.
// str must have a lenth of at least UTF8_MAX_LEN bytes.
size_t utf8_format(char* str, char32_t codepoint) {
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

// utf8_write_to writes codepoint to the stream s. It returns a non-zero value
// on error.
int utf8_write_to(char32_t codepoint, FILE* s) {
	char str[UTF8_MAX_LEN];
	size_t len = utf8_format((char*) &str, codepoint);
	size_t n = fwrite(&str, sizeof(char), len, s);
	if (n != len) {
		return 1;
	}
	return 0;
}

// utf8_read_from reads a single codepoint from the stream s. It returns the
// number of bytes read, or EOF on error.
int utf8_read_from(char32_t* codepoint, FILE* s) {
	int c = fgetc(s);
	if (c == EOF) {
		return EOF;
	}
	*codepoint = (char32_t) c;

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
