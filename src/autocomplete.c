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
	int n;
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

// trie_node_len returns the number of strings in the tree. It counts
// duplicates.
int trie_node_len(struct trie_node* first) {
	int n = 0;
	for (struct trie_node* node = first; node != NULL; node = node->next) {
		n += node->n;
	}
	return n;
}

// A trie_list is a chained list of strings, sorted by n in descending order.
struct trie_list {
	struct trie_list* next;
	char* str;
	int n;
};

static struct trie_list* trie_list_new() {
	struct trie_list* list = (struct trie_list*) malloc(sizeof(struct trie_list));
	list->next = NULL;
	list->str = NULL;
	list->n = 0;
	return list;
}

void trie_list_free(struct trie_list* list) {
	struct trie_list* item = list;
	while (item != NULL) {
		struct trie_list* next = item->next;
		free(item->str);
		free(item);
		item = next;
	}
}

static void trie_list_merge(struct trie_list** list, struct trie_list* other) {
	struct trie_list* item = *list;
	struct trie_list* new = other;
	struct trie_list* next = NULL;
	while (new != NULL) {
		next = new->next;

		if (*list == NULL || (*list)->n <= new->n) {
			new->next = *list;
			*list = new;
			item = new;
		} else {
			while (item->next != NULL && item->next->n > new->n) {
				item = item->next;
			}
			new->next = item->next;
			item->next = new;
		}

		new = next;
	}
}

// _trie_node_list is a recursive helper function for trie_node_list. first,
// list and cap are the same as trie_node_list.
// The tree will be browsed from bottom to top because we want to remove
// duplicates. Thus, strings will be filled from right to left.
// duplen will be set to the number of strings in the tree, including
// duplicates.
static struct trie_list* _trie_node_list(struct trie_node* first, int stroffset, int* duplen) {
	*duplen = 0;

	if (first == NULL) {
		// This is a NULL node, nothing to do
		return NULL;
	}

	// Iterate through root nodes
	struct trie_list* list = NULL;
	for (struct trie_node* node = first; node != NULL; node = node->next) {
		// Add strings for children
		int dupchildren;
		struct trie_list* children = _trie_node_list(node->first, stroffset+1, &dupchildren);

		if (dupchildren < node->n) {
			// Some strings end at this node
			// Initialize a new string
			struct trie_list* item = trie_list_new();
			item->str = (char*) malloc((stroffset+2) * sizeof(char));
			item->str[stroffset+1] = '\0';
			item->n = node->n - dupchildren;
			trie_list_merge(&children, item);
		}

		// Set this node's char for each children
		for (struct trie_list* item = children; item != NULL; item = item->next) {
			item->str[stroffset] = node->ch;
		}

		trie_list_merge(&list, children);
		*duplen += node->n;
	}

	return list;
}

// trie_node_list lists the most used strings from the tree and stores the first
// cap of them in list. Duplicate strings are removed.
struct trie_list* trie_node_list(struct trie_node* first) {
	int duplen; // We don't care about this value here
	return _trie_node_list(first, 0, &duplen);
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
