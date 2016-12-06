void status_print(struct status* s) {
	ansi_format(FORMAT_DIM);

	// Print file status
	char* filename = s->filename;
	if (filename == NULL) {
		filename = "untitled";
	}
	printf("%s ", filename);

	// Print selection status
	printf("%d:%d", s->curline+1, s->curcol+1);
	if (s->sellen > 0) {
		printf(" (%d)", s->sellen);
	}

	ansi_format(FORMAT_RESET);
}
