#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "red_black_tree.h"

define_rbtree(int, char)

#define get_key()	({ int x = rand() % 100000; x; })
#define get_value()	({ int x = (rand() % 10) + 48; x; })

int main() {
	srand(time(NULL));
	int key;
	char value;
	node(int, char) *node = NULL;
	
	fprintf(stderr, "Testing new_node function\n");
	
	node = new_node(int, char);
	
	if (node == NULL) {
		fprintf(stderr, "Error creating node!\n");
		return 1;
	}
	
	fprintf(stderr, "new_node function testing successful\n");
	fprintf(stderr, "Testing destroy_node function\n");
	
	node = node->destroy_node(node);
	
	fprintf(stderr, "destroy_node function testing successful\n");
	
	rb_tree(int, char) *tree = NULL;
	
	fprintf(stderr, "Testing new_rbtree function\n");
	
	tree = new_rbtree(int, char);
	
	if (tree == NULL) {
		fprintf(stderr, "Error creating tree!\n");
		return 1;
	}
	
	fprintf(stderr, "new_rbtree function testing successful\n");
	
	fprintf(stderr, "Testing insert function using random key value pairs\n");
	
	for (int i = 0; i < 10; ++i) {
		key = get_key();
		value = get_value();
		fprintf(stderr, "Insertion %d\nKey: %d, value: %c\n", i, key, value);
		tree->insert(tree, key, value);
	}
	
	fprintf(stderr, "Insert function testing successful\n");
	
	tree->inorder_traverse(tree, tree->root);
	
	fprintf(stderr, "Value of root node: %d\n", tree->root->key);
	
	fprintf(stderr, "Testing destroy_tree function\n");
	
	tree = tree->destroy_rbtree(tree);
	
	fprintf(stderr, "destroy_rbtree function testing successful\n");
	
	fprintf(stderr, "Size of node: %ld bytes\n", sizeof(node(int,char)));
	fprintf(stderr, "Size of rb_tree struct: %ld bytes\n", sizeof(rb_tree(int, char)));
	
	return 0;
}
