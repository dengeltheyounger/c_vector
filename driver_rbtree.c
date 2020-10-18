//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
//#include <stddef.h>
#include "red_black_tree.h"

define_rbtree(int, char)

#define get_key()	({ int x = rand() % 100000; x; })
#define get_value()	({ int x = (rand() % 10) + 48; x; })

typedef struct key_holder {
	int keyset[10];
	int chosen[10];
	size_t keynum;
	size_t nextindex;
} key_holder;

// This is inefficient, but it'll work
int choose_key(key_holder *holder) {
	size_t index = 0;
	int foundindex = 0;
	// Get a random index and check to see if that index has already been used
	while (1) {
		foundindex = 0;
		index = (rand() % holder->keynum);
		// If it has, then repeat process
		for (size_t i = 0; i < holder->nextindex; ++i) {
			if (index == holder->chosen[i]) {
				foundindex = 1;
				break;
			}
		}
		/* If it hasn't, then add the new index to the list of chosen indices
		 * increment nextindex for the next time
		 * return the key
		 */
		if (!foundindex) {
			holder->chosen[holder->nextindex] = index;
			++holder->nextindex;
			return holder->keyset[index];
		}
	}
}

int main() {
	srand(time(NULL));
	int key;
	char value;
	rbtree_code result;
	
	key_holder holder;
	holder.keynum = 10;
	holder.nextindex = 0;
	
	printf("Endianness of machine: %s\n", (ENDIANNESS == BIG) ? "BIG" : "LITTLE");
	
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
		holder.keyset[i] = key;
		
	}
	
	for (size_t i = 0; i < 10; ++i) {
		fprintf(stderr, "Insertion %ld\nKey: %d\n", i, holder.keyset[i]);
		tree->insert(tree, holder.keyset[i], value);
	}
	
	fprintf(stderr, "Insert function testing successful\n\n");
	
	fprintf(stderr, "Testing delete function\n");
	
	tree->inorder_traverse(tree, tree->root);
	
	fprintf(stderr, "Value of root node: %d\n\n", tree->root->key);

	for (int i = 0; i < 5; ++i) {
		key = choose_key(&holder);
		/* fprintf(stderr, "Key to be deleted: %d\n", key); */
		result = tree->delete_pair(tree, key);
		if (result != 0) {
			fprintf(stderr, "Error code: %d\n", result);
			return 1;
		}
	}
	
	tree->inorder_traverse(tree, tree->root);	\
	fprintf(stderr, "Value of root node: %d\n\n", tree->root->key);	\
	fprintf(stderr, "Testing destroy_tree function\n");
	
	tree = tree->destroy_rbtree(tree);
	
	fprintf(stderr, "destroy_rbtree function testing successful\n");
	
	fprintf(stderr, "Size of node: %ld bytes\n", sizeof(node(int,char)));
	fprintf(stderr, "Size of rb_tree struct: %ld bytes\n", sizeof(rb_tree(int, char)));
	
	return 0;
}
