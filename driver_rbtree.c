#include <stdio.h>
#include "red_black_tree.h"

define_node(int, char)
define_rbtree(int, char)

int main() {
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
	printf("Testing destroy_tree function\n");
	
	tree = tree->destroy_rbtree(tree);
	
	printf("destroy_rbtree function testing successful\n");
	
	return 0;
}
