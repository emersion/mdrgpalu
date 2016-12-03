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

int clipboard_xclip_supported() {
	// Check if the process is executed in a graphical session
	if (getenv("DISPLAY") == NULL) {
		return 0;
	}

	// Check if xclip is available
	FILE* p = popen("xclip -version &>/dev/null", "r");
	if (p == NULL) {
		return 0;
	}
	int err = pclose(p);
	return (err == 0);
}
