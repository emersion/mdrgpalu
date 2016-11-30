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
	struct trie_list* next;
	char* str;
	int n;
	void* val;
};

struct trie_node* trie_node_new();
void trie_node_free(struct trie_node* node);
struct trie_node* trie_node_match(struct trie_node* first, char* s, int len);
void* trie_node_val(struct trie_node* first, char* s, int len);
int trie_node_len(struct trie_node* first);
struct trie_list* trie_node_list(struct trie_node* first);
struct trie_node* trie_node_insert(struct trie_node** tree, char* s, int len);
void trie_node_remove(struct trie_node** tree, char* s, int len);

void trie_list_free(struct trie_list* list);
int trie_list_len(struct trie_list* list);
