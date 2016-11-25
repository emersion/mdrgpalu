#define UTF8_REPLACEMENT_CODEPOINT 0xFFFD

static const unsigned char utf8_mask[6] = {
	0x7F,
	0x1F,
	0x0F,
	0x07,
	0x03,
	0x01
};

char utf8_len(unsigned char c) {
	if ((c & (1 << 7)) == 0) {
		return 1; // Non-unicode, single char
	}
	if ((c & (1 << 6)) == 0) {
		return 0; // Unknown
	}
	if ((c & (1 << 5)) == 0) {
		return 2;
	}
	if ((c & (1 << 4)) == 0) {
		return 3;
	}
	if ((c & (1 << 3)) == 0) {
		return 4;
	}
	if ((c & (1 << 2)) == 0) {
		return 5;
	}
	if ((c & (1 << 1)) == 0) {
		return 6;
	}
	return -1;
}

char utf8_format(char* str, char32_t codepoint) {
	char len = 0;
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
	} else if (codepoint < 0x200000) {
		first = 0xF0;
		len = 4;
	} else if (codepoint < 0x4000000) {
		first = 0xF8;
		len = 5;
	} else {
		first = 0xFC;
		len = 6;
	}

	for (int i = len-1; i > 0; i--) {
		str[i] = (codepoint & 0x3F) | 0x80;
		codepoint >>= 6;
	}
	str[0] = codepoint | first;

	return len;
}

int utf8_write_to(char32_t codepoint, FILE* s) {
	char str[6];
	char n = utf8_format((char*) &str, codepoint);
	fwrite(&str, sizeof(char), n, s);
	return ferror(s);
}

char utf8_read_from(char32_t* codepoint, FILE* s) {
	unsigned char c = fgetc(s);
	if (ferror(s)) {
		return 0;
	}
	*codepoint = (char32_t) c;

	char len = utf8_len(c);
	*codepoint &= utf8_mask[len-1];
	for (int i = 1; i < len; i++) {
		c = fgetc(s);
		if (ferror(s)) {
			return 0;
		}
		if (utf8_len(c) != 0) {
			*codepoint = UTF8_REPLACEMENT_CODEPOINT;
			return i;
		}

		*codepoint <<= 6;
		*codepoint |= c & 0x3F;
	}

	return len;
}
