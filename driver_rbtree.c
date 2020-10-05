#include <stdio.h>
#include "red_black_tree.h"

define_rbtree(int, char)

int main() {
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
	
	printf("Testing insert function\n");
	
	printf("First insert...\n");
		
	result = tree->insert(tree, 21,'a');
	
	if (result == new_node_failed) {
		fprintf(stderr, "New node failed!\n");
		return 1;
	}
	
	printf("Second insert...\n");
	
	result = tree->insert(tree, 24, 'b');

	if (result == new_node_failed) {
		fprintf(stderr, "New node failed!\n");
	}

	printf("Testing third insert for rebalancing\n");

	result = tree->insert(tree, 23, 'c');
	
	if (result == new_node_failed) {
		fprintf(stderr, "new node failed!\n");
		return 1;
	}
	
	printf("Insert function testing successful\n");
	
	tree->inorder_traverse(tree, tree->root);
	
	printf("Testing destroy_tree function\n");
	
	tree = tree->destroy_rbtree(tree);
	
	printf("destroy_rbtree function testing successful\n");
	
	printf("Size of node: %ld bytes\n", sizeof(node(int,char)));
	printf("Size of rb_tree struct: %ld bytes\n", sizeof(rb_tree(int, char)));
	
	return 0;
}
