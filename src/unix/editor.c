void editor_print(struct editor* e) {
	struct status* s = (struct status*) malloc(sizeof(struct status));
	s->filename = e->filename;

	term_clear_screen();
	term_cursor_move(0, 0);
	buffer_print(e->buf, s);
	term_cursor_move(0, term_height()-1);
	if (e->status != NULL) {
		print_format(FORMAT_DIM);
		printf("%s", e->status);
		print_format(FORMAT_RESET);
		editor_set_status(e, NULL);
	} else {
		status_print(s);
	}
	term_flush();

	free(s);
}
