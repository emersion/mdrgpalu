// A trie node. See https://en.wikipedia.org/wiki/Trie
struct trie_node {
	// The next sibling node.
	struct trie_node* next;
	// The first child node.
	struct trie_node* first;
	// The node's character.
	char ch;
};

// trie_node_new allocates a new trie node.
struct trie_node* trie_node_new() {
	struct trie_node* n = (struct trie_node*) malloc(sizeof(struct trie_node));
	n->next = NULL;
	n->first = NULL;
	n->ch = 0;
	return n;
}

// trie_node_free deallocates a trie node.
void trie_node_free(struct trie_node* n) {
	free(n);
}

// trie_node_match extracts the node that begins with the string s of len
// characters. Returns NULL if no such node is found.
struct trie_node* trie_node_match(struct trie_node* first, char* s, int len) {
	if (len == 0) {
		return first;
	}

	// Iterate through siblings until we reach s[0]
	struct trie_node* n = first;
	while (n != NULL && n->ch < s[0]) {
		n = n->next;
	}

	if (n == NULL || n->ch != s[0]) {
		// Either we reached the end, either s[0] is not in the tree
		return NULL;
	}
	if (n->first == NULL) {
		// No more childs
		return n;
	}
	return trie_node_match(n->first, &s[1], len-1);
}

// trie_node_insert inserts the string s of len characters in the tree. It
// returns the new tree.
struct trie_node* trie_node_insert(struct trie_node* first, char* s, int len) {
	if (len == 0) {
		return first;
	}

	// Create a new node for s[0]
	struct trie_node* new = trie_node_new();
	new->ch = s[0];

	if (first == NULL || first->ch > new->ch) {
		// Insert the new node at the begining
		new->first = trie_node_insert(NULL, &s[1], len-1);
		new->next = first;
		return new;
	}

	// Find where the new node will be inserted
	struct trie_node* n = first;
	while (n->ch < new->ch && n->next != NULL) {
		n = n->next;
	}

	// Insert the new node right after n
	if (n->ch == new->ch) {
		n->first = trie_node_insert(n->first, &s[1], len-1);
	} else {
		new->first = trie_node_insert(NULL, &s[1], len-1);

		new->next = n->next;
		n->next = new;
	}
	return first;
}

// trie_node_print prints the tree, useful when debugging.
void trie_node_print(struct trie_node* first) {
	if (first == NULL) {
		printf("NULL");
		return;
	}

	for (struct trie_node* n = first; n != NULL; n = n->next) {
		printf("(%c ", n->ch);
		trie_node_print(n->first);
		printf(")");
	}
}
