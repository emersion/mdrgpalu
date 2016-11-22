// A trie node. See https://en.wikipedia.org/wiki/Trie
// Note: to support storing properly a string multiple times, keeping the number
// of strings that uses each node is necessary.
struct trie_node {
	// The next sibling node.
	struct trie_node* next;
	// The first child node.
	struct trie_node* first;
	// The node's character.
	char ch;
	// The number of strings using this node.
	unsigned int n;
};

// trie_node_new allocates a new trie node.
struct trie_node* trie_node_new() {
	struct trie_node* node = (struct trie_node*) malloc(sizeof(struct trie_node));
	node->next = NULL;
	node->first = NULL;
	node->ch = 0;
	node->n = 0;
	return node;
}

// trie_node_free deallocates a trie node.
void trie_node_free(struct trie_node* node) {
	free(node);
}

// trie_node_match extracts the node that begins with the string s of len
// characters. Returns NULL if no such node is found.
struct trie_node* trie_node_match(struct trie_node* first, char* s, int len) {
	if (len == 0 || first == NULL) {
		return first;
	}

	// Iterate through siblings until we reach s[0]
	struct trie_node* node = first;
	while (node != NULL && node->ch < s[0]) {
		node = node->next;
	}

	if (node == NULL || node->ch != s[0]) {
		// Either we reached the end, either s[0] is not in the tree
		return NULL;
	}
	return trie_node_match(node->first, &s[1], len-1);
}

// trie_node_insert inserts the string s of len characters in the tree. It
// returns the new tree.
struct trie_node* trie_node_insert(struct trie_node* first, char* s, int len) {
	if (len == 0) {
		return first;
	}

	if (first == NULL || first->ch > s[0]) {
		// Insert a new node for s[0] at the begining
		struct trie_node* new = trie_node_new();
		new->ch = s[0];
		new->n = 1;
		new->first = trie_node_insert(NULL, &s[1], len-1);
		new->next = first;
		return new;
	}

	// Find where the new node will be inserted
	struct trie_node* node = first;
	while (node->ch < s[0] && node->next != NULL) {
		node = node->next;
	}

	if (node->ch == s[0]) {
		// A node for s[0] already exists
		node->n++;
		node->first = trie_node_insert(node->first, &s[1], len-1);
	} else {
		// Insert the new node right after node
		struct trie_node* new = trie_node_new();
		new->ch = s[0];
		new->n = 1;
		new->first = trie_node_insert(NULL, &s[1], len-1);

		new->next = node->next;
		node->next = new;
	}
	return first;
}

// trie_node_remove removes the string s of len characters from the tree. It
// returns the new tree.
struct trie_node* trie_node_remove(struct trie_node* first, char* s, int len) {
	if (len == 0 || first == NULL) {
		return first;
	}

	// Remove s[1:] from the subtree
	if (first->first != NULL) {
		first->first = trie_node_remove(first->first, &s[1], len-1);
	}

	// Iterate through siblings until we reach s[0]
	struct trie_node* node = first;
	struct trie_node* prev = NULL;
	while (node != NULL && node->ch < s[0]) {
		prev = node;
		node = node->next;
	}

	if (node == NULL || node->ch != s[0]) {
		// s[0] is not in the tree, nothing to do
		return first;
	}

	node->n--;
	if (node->n <= 0) {
		// node is not used anymore, remove it from the tree
		if (prev == NULL) {
			return NULL;
		} else {
			prev->next = node->next;
		}
	}
	return first;
}

// trie_node_print prints the tree, useful when debugging.
void trie_node_print(struct trie_node* first) {
	if (first == NULL) {
		printf("NULL");
		return;
	}

	for (struct trie_node* node = first; node != NULL; node = node->next) {
		printf("(%c:%d ", node->ch, node->n);
		trie_node_print(node->first);
		printf(")");
	}
}
