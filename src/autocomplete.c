struct autocomplete_node {
	struct node* next;
	struct node* first;
	char ch;
};

struct node* node_new() {
	struct node* n = (struct node*) malloc(sizeof(struct node));
	n->next = NULL;
	n->first = NULL;
	n->ch = 0;
	return n;
}

void node_free(struct node* n) {
	free(n);
}

struct node* node_match(struct node* first, char* query, int len) {
	if (len == 0) {
		return NULL;
	}

	struct node* n = first;
	while (n != NULL && n->ch < query[0]) {
		n = n->next;
	}

	if (n == NULL || n->ch != query[0]) {
		return NULL;
	}
	if (n->first == NULL) {
		return NULL;
	}
	return node_match(n->first, &query[1], len-1);
}

void node_insert(struct node* first, struct node* new) {
	struct node* n = first;
	while (n->ch < new->ch && n->next != NULL) {
		n = n->next;
	}

	if (n->ch == new->ch) {
		if (n->first != NULL && new->first != NULL) {
			node_insert(n->first, new->first);
		} else if (n->first == NULL) {
			n->first = new->first;
		}
	} else {
		new->next = n->next;
		n->next = new;
	}
}
