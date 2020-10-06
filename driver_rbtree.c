#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "red_black_tree.h"

define_rbtree(int, char)

#define get_key()	({ int x = rand() % 100; x; })
#define get_value()	({ int x = (rand() % 10) + 48; x; })

int main() {
	srand(time(NULL));
	rbtree_code result = 0;
	node(int, char) *node = NULL;
	
	printf("Testing new_node function\n");
	
	node = new_node(int, char);
	
	if (node == NULL) {
		fprintf(stderr, "Error creating node!\n");
		return 1;
	}
	
	printf("new_node function testing successful\n");
	printf("Testing destroy_node function\n");
	
	node = node->destroy_node(node);
	
	printf("destroy_node function testing successful\n");
	
	rb_tree(int, char) *tree = NULL;
	
	printf("Testing new_rbtree function\n");
	
	tree = new_rbtree(int, char);
	
	if (tree == NULL) {
		fprintf(stderr, "Error creating tree!\n");
		return 1;
	}
	
	printf("new_rbtree function testing successful\n");
	
	printf("Testing insert function using random key value pairs\n");
	
	for (int i = 0; i < 10; ++i) {
		printf("Insertion %d\n", i);
		tree->insert(tree, get_key(), get_value());
	}
	
	printf("Insert function testing successful\n");
	
	tree->inorder_traverse(tree, tree->root);
	
	printf("Value of root node: %d\n", tree->root->key);
	
	printf("Testing destroy_tree function\n");
	
	tree = tree->destroy_rbtree(tree);
	
	printf("destroy_rbtree function testing successful\n");
	
	printf("Size of node: %ld bytes\n", sizeof(node(int,char)));
	printf("Size of rb_tree struct: %ld bytes\n", sizeof(rb_tree(int, char)));
	
	return 0;
}
