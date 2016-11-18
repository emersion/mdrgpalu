FILE* clipboard_xclip_open(char* mode) {
	char* cmd;
	if (mode[0] == 'r') {
		cmd = "xclip -selection clipboard -o";
	} else {
		cmd = "xclip -selection clipboard -i";
	}

	return popen(cmd, mode);
}

int clipboard_xclip_close(FILE* p) {
	return pclose(p);
}
