struct trie_node {
	struct trie_node* next;
	struct trie_node* first;
	char ch;
};

struct trie_node* trie_node_new() {
	struct trie_node* n = (struct trie_node*) malloc(sizeof(struct trie_node));
	n->next = NULL;
	n->first = NULL;
	n->ch = 0;
	return n;
}

void trie_node_free(struct trie_node* n) {
	free(n);
}

struct trie_node* trie_node_match(struct trie_node* first, char* query, int len) {
	if (len == 0) {
		return NULL;
	}

	struct trie_node* n = first;
	while (n != NULL && n->ch < query[0]) {
		n = n->next;
	}

	if (n == NULL || n->ch != query[0]) {
		return NULL;
	}
	if (n->first == NULL) {
		return NULL;
	}
	return trie_node_match(n->first, &query[1], len-1);
}

void trie_node_insert(struct trie_node* first, struct trie_node* new) {
	struct trie_node* n = first;
	while (n->ch < new->ch && n->next != NULL) {
		n = n->next;
	}

	if (n->ch == new->ch) {
		if (n->first != NULL && new->first != NULL) {
			trie_node_insert(n->first, new->first);
		} else if (n->first == NULL) {
			n->first = new->first;
		}
	} else {
		new->next = n->next;
		n->next = new;
	}
}
