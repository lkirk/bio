#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// https://en.wikipedia.org/wiki/Interval_tree

typedef struct node {
    uint32_t start, end;
    struct node *left, *right;
} node_t;

node_t *new_node(int32_t start, int32_t end) {
    node_t *new;
    new = malloc(sizeof(*new));
    new->start = start;
    new->end = end;
    new->left = NULL;
    new->right = NULL;
    return new;
}

void print_node(node_t *n) {
  fprintf(stdout, "node_t {\n");
  fprintf(stdout, "    start: %d\n", n->start);
  fprintf(stdout, "    end: %d\n", n->end);
  fprintf(stdout, "    left: %p\n", n->left);
  fprintf(stdout, "    right: %p\n", n->right);
  fprintf(stdout, "}\n");
}

int main() {
  node_t *n = new_node(0, 100);
  node_t *n1 = new_node(3, 100);
  node_t *n2 = new_node(50, 1000);
  print_node(n);
  n->left = n1;
  n->right = n2;
  print_node(n);
  free(n);
}
