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
	// This node's value.
	void* val;
};

// A trie_list is a chained list of strings, sorted by n in descending order.
struct trie_list {
	// The next item in the list.
	struct trie_list* next;
	// This item's string.
	char* str;
	// The number of times the string appears in the tree.
	int n;
	// The string's value.
	void* val;
};

// trie_node_new allocates a new trie node.
struct trie_node* trie_node_new();

// trie_node_free deallocates a trie node and all its children. free_val is a
// function that deallocates val. If free_val is NULL, val is not deallocated.
void trie_node_free(struct trie_node* first, void (*free_val)(void* val));

// trie_node_match extracts the node that begins with the string s of len
// characters. Returns NULL if no such node is found.
struct trie_node* trie_node_match(struct trie_node* first, char* s, int len);

// trie_node_val returns the value associated with the string s in the tree.
void* trie_node_val(struct trie_node* first, char* s, int len);

// trie_node_len returns the number of strings in the tree. It counts
// duplicates.
int trie_node_len(struct trie_node* first);

// trie_node_list lists the most used strings from the tree. Duplicate strings
// are removed.
struct trie_list* trie_node_list(struct trie_node* first);

// trie_node_insert inserts the string s of len characters in the tree. It
// returns the leaf.
struct trie_node* trie_node_insert(struct trie_node** tree, char* s, int len);

// trie_node_remove removes the string s of len characters from the tree. It
// returns the removed leaf without deallocating it.
struct trie_node* trie_node_remove(struct trie_node** tree, char* s, int len);

// trie_list_free deallocates a trie list. val is not deallocated.
void trie_list_free(struct trie_list* list);

// trie_list_len computes list's length.
int trie_list_len(struct trie_list* list);
