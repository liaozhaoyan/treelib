//
// Created by 廖肇燕 on 2023/10/26.
//

#include <stdio.h>
#include "tree.h"

static void strip(struct tree_node * node, void * dummy) {
    printf("strip node %ld, level%d\n", node->id, node->level);
    free(node);
}

int main() {
    struct tree_root root;

    new_tree(&root);
    struct tree_node *node1 = node_new(1);
    add_child(root.node, node1);

    node1 = node_new(2);
    add_child(root.node, node1);

    struct tree_node *node2 = node_new(3);
    add_child(node1, node2);

    node2 = node_new(4);
    add_child(node1, node2);

    struct tree_node *node3 = node_new(5);
    add_child(node2, node3);

    node3 = node_new(6);
    add_child(node2, node3);

    node3 = node_new(7);
    add_child(node2, node3);

    node_strip(node1, strip, NULL);
}
