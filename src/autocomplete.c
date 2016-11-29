// trie_node_new allocates a new trie node.
struct trie_node* trie_node_new() {
	struct trie_node* node = (struct trie_node*) malloc(sizeof(struct trie_node));
	node->next = NULL;
	node->first = NULL;
	node->ch = 0;
	node->n = 0;
	node->val = NULL;
	return node;
}

// trie_node_free deallocates a trie node. val is not deallocated.
void trie_node_free(struct trie_node* node) {
	free(node);
}

// trie_node_match extracts the node that begins with the string s of len
// characters. Returns NULL if no such node is found.
struct trie_node* trie_node_match(struct trie_node* first, char* s, int len) {
	if (len == 0 || first == NULL) {
		return first;
	}

	char ch = tolower(s[0]);

	// Iterate through siblings until we reach ch
	struct trie_node* node = first;
	while (node != NULL && node->ch < ch) {
		node = node->next;
	}

	if (node == NULL || node->ch != ch) {
		// Either we reached the end, either ch is not in the tree
		return NULL;
	}
	return trie_node_match(node->first, &s[1], len-1);
}

static void* _trie_node_val(struct trie_node* first, char* s, int len, void* val) {
	if (len == 0) {
		return val;
	}
	if (first == NULL) {
		return NULL;
	}

	char ch = tolower(s[0]);

	// Iterate through siblings until we reach ch
	struct trie_node* node = first;
	while (node != NULL && node->ch < ch) {
		node = node->next;
	}

	if (node == NULL || node->ch != ch) {
		// Either we reached the end, either ch is not in the tree
		return NULL;
	}
	return _trie_node_val(node->first, &s[1], len-1, node->val);
}

// trie_node_val returns the value associated with the string s in the tree.
void* trie_node_val(struct trie_node* first, char* s, int len) {
	return _trie_node_val(first, s, len, NULL);
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

// trie_list_new allocates a new trie list.
static struct trie_list* trie_list_new() {
	struct trie_list* list = (struct trie_list*) malloc(sizeof(struct trie_list));
	list->next = NULL;
	list->str = NULL;
	list->n = 0;
	list->val = NULL;
	return list;
}

// trie_list_free deallocates a trie list. val is not deallocated.
void trie_list_free(struct trie_list* list) {
	struct trie_list* item = list;
	while (item != NULL) {
		struct trie_list* next = item->next;
		free(item->str);
		free(item);
		item = next;
	}
}

// trie_list_len computes list's length.
int trie_list_len(struct trie_list* list) {
	int len = 0;
	for (struct trie_list* item = list; item != NULL; item = item->next) {
		len++;
	}
	return len;
}

// trie_list_merge merges other into list.
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

// _trie_node_list is a recursive helper function for trie_node_list.
// It returns a list of strings contained in first, leaving stroffset-1 chars
// unused at the begining of each string.
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

// trie_node_list lists the most used strings from the tree. Duplicate strings
// are removed.
struct trie_list* trie_node_list(struct trie_node* first) {
	int duplen; // We don't care about this value here
	return _trie_node_list(first, 0, &duplen);
}

// trie_node_insert inserts the string s of len characters in the tree. It
// returns the leaf.
struct trie_node* trie_node_insert(struct trie_node** tree, char* s, int len) {
	if (len == 0) {
		return *tree;
	}

	char ch = tolower(s[0]);

	// Find where the new node will be inserted
	struct trie_node** node = tree;
	while (*node != NULL && (*node)->ch < ch) {
		node = &(*node)->next;
	}

	struct trie_node* leaf = NULL;
	if (*node != NULL && (*node)->ch == ch) {
		// A node for ch already exists
		(*node)->n++;
		leaf = trie_node_insert(&(*node)->first, &s[1], len-1);
	} else {
		// Insert a new node for ch
		struct trie_node* new = trie_node_new();
		new->ch = ch;
		new->n = 1;
		leaf = trie_node_insert(&new->first, &s[1], len-1);
		new->next = *node;
		*node = new;
	}

	if (leaf != NULL) {
		return leaf;
	} else {
		return *node;
	}
}

// trie_node_remove removes the string s of len characters from the tree.
void trie_node_remove(struct trie_node** tree, char* s, int len) {
	if (len == 0 || *tree == NULL) {
		return;
	}

	char ch = tolower(s[0]);

	// Iterate through siblings until we reach ch
	struct trie_node* node = *tree;
	struct trie_node* prev = NULL;
	while (node != NULL && node->ch < ch) {
		prev = node;
		node = node->next;
	}

	if (node == NULL || node->ch != ch) {
		// ch is not in the tree, nothing to do
		return;
	}

	// Remove s[1:] from the subtree
	trie_node_remove(&node->first, &s[1], len-1);

	node->n--;
	if (node->n <= 0) {
		// node is not used anymore, remove it from the tree
		if (prev == NULL) {
			trie_node_free(node);
			*tree = NULL;
		} else {
			prev->next = node->next;
			trie_node_free(node);
		}
	}
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
