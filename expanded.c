/* Copyright (C) 1991-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/* rb_tree compares the byte value of keys. This means that the endianness
 * of the machine matters. For that reason, a custom memcmp type function
 * is used that accounts for endianness
 */
static uint32_t endiantest = 0xdeadbeef;
typedef enum endianness { LITTLE, BIG } endianness;
typedef struct error_info {
  char *functname;
  char *line;
  char *code;
} error_info;
// This can be used for error logging
error_info _err = {.functname = 0, .line = 0, .code = 0};
typedef enum color_t { BLACK, RED } color_t;
typedef enum rbtree_code {
  realloc_failed = 1,
  make_sentinels_failed,
  new_node_failed,
  basic_insert_failed,
  memcpy_failed,
  key_not_found
} rbtree_code;
/* define_node(K,V)
 * INPUT: K -> key data type, V -> value data type
 * OUTPUT: NONE
 * USAGE define_node(int, char)
 * NOTES: This is designed to be used internally by the red and black tree
 *
 * node(K,V) *destroy_node_##K##_##V(node(K,V) *node)
 * INPUT: node(K,V) *node -> pointer to node based on a given key type and value
 * type OUTPUT: node(K,V) * -> pointer to node based on a given key type and
 * value type USAGE: node = node->destroy_node(node); NOTES: This will
 * recursively delete all child nodes
 *
 * void set_sentinels_##K##_##V(node(K,V) *node)
 * INPUT: node(K,V) *node -> pointer to node of key and value data types
 * OUTPUT: none
 * USAGE: node->set_sentinels(node);
 * NOTES: This is used internally to initialize sentinel nodes. It will
 * not test to see if the sentinel nodes already exist.
 *
 * node(K,V) *new_node_##K##_##V()
 * INPUT: none
 * OUTPUT: node pointer of given key type and value type
 * USAGE: node(K,V) *node = new_node(K,V);
 * NOTES: This will create and initialize two sentinal children
 */
/* node_generic contains all of the operations and members that are common to
 * all nodes. node_generic will be a member of node_##K##_##V (not as a
 * pointer). By using indirection, it can appear as though the node_generic
 * members and operations belong to node_##K##_##V. It seems that this could be
 * used to implement a kind of inheritance in C. The real advantage to
 * node_generic is that the functions and struct will only need to be defined
 * once. I think that will make a big difference in terms of executable size. My
 * original method will lead to the same functions being defined repeatedly for
 * each node_##K##_##V defined. The purpose of kvnodesize is to allow for
 * efficient copying of nodes
 */
typedef struct node_generic {
  color_t color;
  struct node_generic *parent;
  struct node_generic *rchild;
  struct node_generic *lchild;
  bool (*is_sentinel)(struct node_generic *);
  struct node_generic *(*set_sentinels)(struct node_generic *,
                                        struct node_generic *);
  struct node_generic *(*sibling)(struct node_generic *);
  struct node_generic *(*uncle)(struct node_generic *);
  struct node_generic *(*grandparent)(struct node_generic *);
  struct node_generic *(*minimum)(struct node_generic *);
  struct node_generic *(*maximum)(struct node_generic *);
  struct node_generic *(*successor)(struct node_generic *);
  struct node_generic *(*predecessor)(struct node_generic *);
  size_t kvnodesize; // This contains the size of the kv node struct, not the
                     // generic node struct
} node_generic;
/* define_rbtree(K,V)
 * INPUT: K -> key data type, V -> value data type
 * OUTPUT: none
 * USAGE: define_rbtree(int, char)
 * NOTES: This is used internally by c_map
 *
 * static inline void rotate_left_##K##_##V(node(K,V) *root, node(K,V) *rchild)
 * INPUT: node(K,V) *root -> root node of subtree, node(K,V) *rchild -> right
 * child of subtree OUTPUT: none USAGE: rotate_left_##K##_##V(root,
 * root->rchild) NOTES: This will do a left rotate of tree. rchild becomes root
 * root becomes lchild, and the original lchild of rchild becomes the
 * rchild of root
 *
 * static inline void rotate_right_##K##_##V(node(K,V) *root, node(K,V) *lchild)
 * INPUT: node(K,V) *root -> root node of subtree, node(K,V) *lchild -> left
 * child of subtree OUTPUT none USAGE: rotate_right_##K##_##V(root, lchild);
 * NOTES: This  will do a right rotate of tree. lchild becomes root
 * root becomes rchild, and the original rchild of lchild becomes the
 * lchild of root
 *
 * rb_tree(K,V) *destroy_rbtree_##K##_##V(rb_tree(K,V) *tree)
 * INPUT: rb_tree(K,V) *tree -> red black tree struct of given key and value
 * data types OUTPUT: rb_tree(K,V) * -> pointer to red black tree struct USAGE:
 * tree = tree->destroy_rbtree(tree); NOTES: This will free the node. The entire
 * tree will be freed as well
 *
 * void set_rbtree_ptr_##K##_##V(rb_tree(K,V) *tree)
 * INPUT: rb_tree(K,V) *tree -> pointer to red black tree struct
 * OUTPUT: none
 * USAGE: set_rbtree_ptr_##K##_##V(tree)
 * NOTES: This is used internally by rbtree to set function pointers
 *
 * rb_tree(K,V) *new_rbtree_##K##_##V()
 * INPUT: none
 * OUTPUT: rb_tree(K,V) * -> pointer to red black tree struct
 * USAGE: rb_tree(int, char) *tree = new_tree(int, char);
 * NOTES: This will create a root node with two sentinel children
 */
typedef struct node_int_char {
  color_t color;
  int key;
  char value;
  struct node_int_char *parent;
  struct node_int_char *rchild;
  struct node_int_char *lchild;
  struct node_int_char *(*destroy_node)(struct node_int_char *);
  struct node_int_char *(*set_sentinels)(struct node_int_char *,
                                         struct node_int_char *);
  bool (*is_sentinel)(struct node_int_char *);
  struct node_int_char *(*sibling)(struct node_int_char *);
  struct node_int_char *(*uncle)(struct node_int_char *);
  struct node_int_char *(*grandparent)(struct node_int_char *);
  struct node_int_char *(*minimum)(struct node_int_char *);
  struct node_int_char *(*maximum)(struct node_int_char *);
  struct node_int_char *(*successor)(struct node_int_char *);
  struct node_int_char *(*predecessor)(struct node_int_char *);
} node_int_char;
static inline bool is_sentinel_int_char(node_int_char *node) {
  bool result = true;
  if (node->lchild != NULL && node->rchild != NULL)
    result = false;
  return result;
}
node_int_char *destroy_node_int_char(node_int_char *node) {
  if (node == NULL)
    return NULL;
  else if (node->is_sentinel(node))
    return NULL;
  if (node->rchild != NULL) {
    node->rchild = destroy_node_int_char(node->rchild);
  }
  if (node->lchild != NULL)
    node->lchild = destroy_node_int_char(node->lchild);
  free(node);
  return NULL;
}
static inline node_int_char *make_sentinel_int_char() {
  node_int_char *node = (node_int_char *)calloc(1, sizeof(node_int_char));
  node->color = BLACK;
  node->is_sentinel = &is_sentinel_int_char;
  return node;
}
node_int_char *set_sentinels_int_char(node_int_char *node,
                                      node_int_char *sentinel) {
  if (sentinel == NULL) {
    sentinel = make_sentinel_int_char(sentinel);
    if (sentinel == NULL)
      return NULL;
  }
  node->rchild = sentinel;
  node->lchild = sentinel;
  return sentinel;
}
node_int_char *sibling_int_char(node_int_char *node) {
  if (node == NULL)
    return NULL;
  node_int_char *p = node->parent;
  if (p == NULL)
    return NULL;
  return (node == p->lchild) ? p->rchild : p->lchild;
}
node_int_char *uncle_int_char(node_int_char *node) {
  node_int_char *p = node->parent;
  return p->sibling(p);
}
node_int_char *grandparent_int_char(node_int_char *node) {
  node_int_char *p = node->parent;
  return (p == NULL) ? NULL : p->parent;
}
node_int_char *minimum_int_char(node_int_char *node) {
  node_int_char *temp = node;
  while (!temp->is_sentinel(temp)) {
    if (temp->is_sentinel(temp->lchild))
      break;
    temp = temp->lchild;
  }
  return temp;
}
node_int_char *maximum_int_char(node_int_char *node) {
  node_int_char *temp = node;
  while (!node->is_sentinel(temp)) {
    if (temp->is_sentinel(temp->rchild))
      break;
    temp = temp->rchild;
  }
  return temp;
}
node_int_char *successor_int_char(node_int_char *node) {
  node_int_char *temp = node;
  node_int_char *p = node->parent;
  if (node->is_sentinel(temp))
    return NULL;
  else if (node->is_sentinel(temp->rchild)) {
    while (p != NULL && temp != p->lchild) {
      temp = p;
      p = p->parent;
    }
    return p;
  } else {
    return minimum_int_char(temp->rchild);
  }
  return NULL;
}
node_int_char *predecessor_int_char(node_int_char *node) {
  node_int_char *temp = node;
  node_int_char *parent = node->parent;
  if (node->is_sentinel(temp))
    return NULL;
  else if (node->is_sentinel(temp->lchild)) {
    while (parent != NULL && temp != parent->rchild) {
      temp = parent;
      parent = parent->parent;
    }
    return parent;
  } else {
    return maximum_int_char(temp->lchild);
  }
  return NULL;
}
static inline void set_node_ptr_int_char(node_int_char *node) {
  node->destroy_node = &destroy_node_int_char;
  node->set_sentinels = &set_sentinels_int_char;
  node->is_sentinel = &is_sentinel_int_char;
  node->sibling = &sibling_int_char;
  node->uncle = &uncle_int_char;
  node->grandparent = &grandparent_int_char;
  node->minimum = &minimum_int_char;
  node->maximum = &maximum_int_char;
  node->successor = &successor_int_char;
  node->predecessor = &predecessor_int_char;
}
node_int_char *new_node_int_char() {
  node_int_char *node = NULL;
  node = (node_int_char *)calloc(1, sizeof(node_int_char));
  if (node == NULL) {
    return NULL;
  }
  node->color = RED;
  set_node_ptr_int_char(node);
  return node;
}
typedef struct rb_tree_int_char {
  node_int_char *root;
  node_int_char *sentinel;
  struct rb_tree_int_char *(*destroy_rbtree)(struct rb_tree_int_char *);
  rbtree_code (*insert)(struct rb_tree_int_char *, int, char);
  void (*inorder_traverse)(struct rb_tree_int_char *, node_int_char *);
  char (*get_value)(struct rb_tree_int_char *, int);
  rbtree_code (*delete_pair)(struct rb_tree_int_char *, int);
} rb_tree_int_char;
rb_tree_int_char *destroy_rbtree_int_char(rb_tree_int_char *tree) {
  if (tree != NULL) {
    if (tree->root != NULL)
      tree->root->destroy_node(tree->root);
    if (tree->sentinel != NULL)
      free(tree->sentinel);
    free(tree);
  }
  return NULL;
}
static inline void print_bytes(const void *key, size_t bytes) {
  unsigned char *temp = (unsigned char *)key;
  for (size_t i = 0; i < bytes; ++i)
    printf("%02x", temp[i]);
  printf("\n");
}
static inline int compare_little_endian(const unsigned char *key,
                                        const unsigned char *nkey,
                                        size_t bytes) {
  for (int i = bytes - 1; i >= 0; --i) {
    if (key[i] > nkey[i]) {
      return 1;
    } else if (key[i] < nkey[i])
      return -1;
  }
  return 0;
}
static inline int compare_big_endian(const unsigned char *key,
                                     const unsigned char *nkey, size_t bytes) {
  for (int i = 0; i < bytes; ++i) {
    if (key[i] > nkey[i])
      return 1;
    else if (key[i] < nkey[i])
      return -1;
  }
  return 0;
}
static inline int compare_bytes(const void *key, const void *nkey,
                                size_t bytes) {
  if (({
        endianness e = 0;
        if (*(const char *)&endiantest == 0xef)
          e = LITTLE;
        else if (*(const char *)&endiantest == 0xde)
          e = BIG;
        e;
      }) == LITTLE)
    return compare_little_endian((unsigned char *)key, (unsigned char *)nkey,
                                 bytes);
  else
    return compare_big_endian((unsigned char *)key, (unsigned char *)nkey,
                              bytes);
}
static inline node_int_char *basic_search_int_char(rb_tree_int_char *tree,
                                                   int key) {
  node_int_char *temp = tree->root;
  int nkey;
  int result = 0;
  while (temp != tree->sentinel) {
    nkey = temp->key;
    result = compare_bytes(&key, &nkey, sizeof(int));
    if (result == 0)
      return temp;
    else if (result < 0)
      temp = temp->lchild;
    else
      temp = temp->rchild;
  }
  return NULL;
}
char get_value_int_char(rb_tree_int_char *tree, int key) {
  int k;
  node_int_char *temp = basic_search_int_char(tree, key);
  if (temp != NULL)
    k = temp->value;
  else
    memset(&k, 0, sizeof(int));
  return k;
}
static inline node_int_char *basic_insert_int_char(rb_tree_int_char *tree,
                                                   int key, char value) {
  node_int_char *node = tree->root;
  node_int_char *temp = tree->root;
  int nkey;
  int result = 0;
  if (node == NULL) {
    temp = new_node_int_char();
    if (temp == NULL)
      return NULL;
    temp->key = key;
    temp->value = value;
    tree->sentinel = temp->set_sentinels(temp, tree->sentinel);
    tree->root = temp;
    return tree->root;
  }
  while (true) {
    node = temp;
    nkey = node->key;
    result = compare_bytes(&key, &nkey, sizeof(int));
    if (result == 0) {
      node->value = value;
      return node;
    } else if (result < 0) {
      temp = node->lchild;
    } else {
      temp = node->rchild;
    }
    if (temp == tree->sentinel) {
      break;
    }
  }
  temp = new_node_int_char();
  if (temp == NULL)
    return NULL;
  temp->set_sentinels(temp, tree->sentinel);
  temp->key = key;
  temp->value = value;
  temp->color = RED;
  temp->parent = node;
  (result > 0) ? (node->rchild = temp) : (node->lchild = temp);
  return temp;
}
static inline color_t uncle_color_int_char(node_int_char *node) {
  node_int_char *u = node->uncle(node);
  return (u->color == RED) ? RED : BLACK;
}
static inline void recolor_int_char(node_int_char *node) {
  node_int_char *temp = node;
  node_int_char *p = temp->parent;
  node_int_char *u = node->uncle(node);
  node_int_char *g = node->grandparent(node);
  p->color = BLACK;
  u->color = BLACK;
  g->color = RED;
}
static inline void rotate_left_int_char(rb_tree_int_char *tree,
                                        node_int_char *node) {
  node_int_char *temp = node;
  node_int_char *pivot = temp->rchild;
  node_int_char *p = temp->parent;
  pivot->parent = p;
  temp->rchild = pivot->lchild;
  pivot->lchild = temp;
  if (temp->rchild != tree->sentinel) {
    temp->rchild->parent = temp;
  }
  temp->parent = pivot;
  if (p != NULL) {
    (p->rchild == temp) ? (p->rchild = pivot) : (p->lchild = pivot);
  }
  if (tree->root == temp) {
    tree->root = pivot;
  }
}
static inline void rotate_right_int_char(rb_tree_int_char *tree,
                                         node_int_char *node) {
  node_int_char *temp = node;
  node_int_char *p = temp->parent;
  node_int_char *pivot = temp->lchild;
  pivot->parent = p;
  temp->lchild = pivot->rchild;
  pivot->rchild = temp;
  if (temp->lchild != tree->sentinel) {
    temp->lchild->parent = temp;
  }
  temp->parent = pivot;
  if (p != NULL) {
    (p->rchild == temp) ? (p->rchild = pivot) : (p->lchild = pivot);
  }
  if (tree->root == temp) {
    tree->root = pivot;
  }
}
static inline void rotate_int_char(rb_tree_int_char *tree,
                                   node_int_char *node) {
  node_int_char *temp = node;
  node_int_char *p = temp->parent;
  node_int_char *g = temp->grandparent(temp);
  if (temp == p->rchild && p == g->lchild) {
    rotate_left_int_char(tree, p);
    temp = temp->lchild;
  } else if (temp == p->lchild && p == g->rchild) {
    rotate_right_int_char(tree, p);
    temp = temp->rchild;
  }
  p = temp->parent;
  g = temp->grandparent(temp);
  if (temp == p->lchild) {
    rotate_right_int_char(tree, g);
  } else {
    rotate_left_int_char(tree, g);
  }
  p->color = BLACK;
  g->color = RED;
}
static inline void repair_tree_insert_int_char(rb_tree_int_char *tree,
                                               node_int_char *node) {
  node_int_char *temp = node;
  while (true) {
    if (temp->parent == NULL) {
      temp->color = BLACK;
      return;
    } else if (temp->parent->color == BLACK) {
      return;
    } else if (temp->uncle(temp) != NULL && uncle_color_int_char(temp) == RED) {
      recolor_int_char(temp);
      temp = temp->grandparent(temp);
    } else if (temp->uncle(temp) != NULL &&
               uncle_color_int_char(temp) == BLACK) {
      rotate_int_char(tree, temp);
      return;
    }
  }
}
rbtree_code insert_int_char(rb_tree_int_char *tree, int key, char value) {
  node_int_char *temp = basic_insert_int_char(tree, key, value);
  if (temp == NULL)
    return basic_insert_failed;
  repair_tree_insert_int_char(tree, temp);
  return 0;
}
static inline void transplant_int_char(node_int_char *node,
                                       node_int_char *child) {
  child->parent = node->parent;
  (node == node->parent->lchild) ? (node->parent->lchild = child)
                                 : (node->parent->rchild = child);
}
static inline node_int_char *basic_delete_int_char(rb_tree_int_char *tree,
                                                   node_int_char *node) {
  node_int_char *temp = node;
  if (!temp->rchild->is_sentinel(temp->rchild) &&
      !temp->lchild->is_sentinel(temp->lchild)) {
    node_int_char *successor = temp->rchild->minimum(temp->rchild);
    temp->key = successor->key;
    temp->value = successor->value;
    if (!successor->lchild->is_sentinel(successor->lchild))
      transplant_int_char(successor, successor->lchild);
    else if (!successor->rchild->is_sentinel(successor->rchild))
      transplant_int_char(successor, successor->rchild);
    else if (successor->rchild->is_sentinel(successor->rchild) &&
             successor->lchild->is_sentinel(successor->lchild)) {
      (successor == successor->parent->lchild)
          ? (successor->parent->lchild = tree->sentinel)
          : (successor->parent->rchild = tree->sentinel);
    }
    free(successor);
    return NULL;
  } else if (temp->rchild->is_sentinel(temp->rchild) &&
             temp->lchild->is_sentinel(temp->lchild)) {
    if (temp == tree->root)
      tree->root = NULL;
    else
      (temp == temp->parent->lchild) ? (temp->parent->lchild = tree->sentinel)
                                     : (temp->parent->rchild = tree->sentinel);
    free(node);
    return NULL;
  } else {
    node_int_char *child =
        (temp->rchild == tree->sentinel) ? temp->lchild : temp->rchild;
    transplant_int_char(temp, child);
    if (temp->color == BLACK) {
      if (child->color == BLACK)
        return child;
      child->color = BLACK;
      free(node);
      return NULL;
    }
  }
  return NULL;
}
void repair_tree_delete_int_char(rb_tree_int_char *tree, node_int_char *node) {
  node_int_char *temp = node;
  while (true) {
    if (temp->parent == NULL) {
      return;
    }
    node_int_char *sibling = temp->sibling(temp);
    if (sibling->color == RED) {
      temp->parent->color = RED;
      sibling->color = BLACK;
      (temp == temp->parent->rchild) ? rotate_right_int_char(tree, temp->parent)
                                     : rotate_left_int_char(tree, temp->parent);
    }
    sibling = temp->sibling(temp);
    if (temp->parent->color == BLACK && sibling->color == BLACK &&
        sibling->rchild->color == BLACK && sibling->lchild->color == BLACK) {
      sibling->color = RED;
      temp = temp->parent;
      continue;
    }
    sibling = temp->sibling(temp);
    if (temp->parent->color == RED && sibling->color == BLACK &&
        sibling->lchild == BLACK && sibling->rchild == BLACK) {
      sibling->color = RED;
      temp->parent->color = BLACK;
      return;
    }
    sibling = temp->sibling(temp);
    if (sibling->color == BLACK) {
      if (temp == temp->parent->lchild && sibling->rchild == BLACK &&
          sibling->lchild->color == RED) {
        sibling->color = RED;
        sibling->lchild->color = BLACK;
        rotate_right_int_char(tree, sibling);
      } else if (temp == temp->parent->rchild &&
                 sibling->lchild->color == BLACK &&
                 sibling->rchild->color == RED) {
        sibling->color = RED;
        sibling->rchild->color = BLACK;
        rotate_left_int_char(tree, sibling);
      }
    }
    sibling = temp->sibling(temp);
    sibling->color = temp->parent->color;
    temp->parent->color = BLACK;
    if (temp == temp->parent->lchild) {
      sibling->rchild->color = BLACK;
      rotate_left_int_char(tree, temp->parent);
    } else {
      sibling->lchild->color = BLACK;
      rotate_right_int_char(tree, temp->parent);
    }
    return;
  }
}
rbtree_code delete_int_char(rb_tree_int_char *tree, int key) {
  node_int_char *temp = basic_search_int_char(tree, key);
  node_int_char *child = NULL;
  if (temp == NULL)
    return key_not_found;
  child = basic_delete_int_char(tree, temp);
  if (child == NULL)
    return 0;
  else {
    repair_tree_delete_int_char(tree, child);
    if (temp == tree->root)
      tree->root = child;
    free(temp);
  }
  return 0;
}
void inorder_traverse_int_char(rb_tree_int_char *tree, node_int_char *node) {
  if (node->is_sentinel(node))
    return;
  inorder_traverse_int_char(tree, node->lchild);
  if (node->parent != NULL)
    fprintf(stderr, "Node is the child of %d\n", node->parent->key);
  fprintf(stderr, "Key: %d, Value: %c\n", node->key, node->value);
  fprintf(stderr, "Color: ");
  fprintf(stderr, "%s", node->color == RED ? "RED" : "BLACK");
  fprintf(stderr, "\n");
  inorder_traverse_int_char(tree, node->rchild);
}
void set_rbtree_ptr_int_char(rb_tree_int_char *tree) {
  tree->destroy_rbtree = &destroy_rbtree_int_char;
  tree->insert = &insert_int_char;
  tree->inorder_traverse = &inorder_traverse_int_char;
  tree->get_value = &get_value_int_char;
  tree->delete_pair = &delete_int_char;
}
rb_tree_int_char *new_rbtree_int_char() {
  rb_tree_int_char *tree = NULL;
  tree = (rb_tree_int_char *)calloc(1, sizeof(rb_tree_int_char));
  if (tree == NULL) {
    return NULL;
  }
  set_rbtree_ptr_int_char(tree);
  return tree;
}
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
  printf("Endianness of machine: %s\n",
         (({
            endianness e = 0;
            if (*(const char *)&endiantest == 0xef)
              e = LITTLE;
            else if (*(const char *)&endiantest == 0xde)
              e = BIG;
            e;
          }) == BIG)
             ? "BIG"
             : "LITTLE");
  rb_tree_int_char *tree = NULL;
  fprintf(stderr, "Testing new_rbtree function\n");
  tree = new_rbtree_int_char();
  if (tree == NULL) {
    fprintf(stderr, "Error creating tree!\n");
    return 1;
  }
  fprintf(stderr, "new_rbtree function testing successful\n");
  fprintf(stderr, "Testing insert function using random key value pairs\n");
  for (int i = 0; i < 10; ++i) {
    key = ({
      int x = rand() % 100000;
      x;
    });
    value = ({
      int x = (rand() % 10) + 48;
      x;
    });
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
  tree->inorder_traverse(tree, tree->root);
  fprintf(stderr, "Value of root node: %d\n\n", tree->root->key);
  fprintf(stderr, "Testing destroy_tree function\n");
  tree = tree->destroy_rbtree(tree);
  fprintf(stderr, "destroy_rbtree function testing successful\n");
  fprintf(stderr, "Size of node: %ld bytes\n", sizeof(node_int_char));
  fprintf(stderr, "Size of rb_tree struct: %ld bytes\n",
          sizeof(rb_tree_int_char));
  return 0;
}
