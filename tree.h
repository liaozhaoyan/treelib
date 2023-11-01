//
// Created by 廖肇燕 on 2023/10/26.
//

#ifndef TREELIB_TREE_H
#define TREELIB_TREE_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef long id_type;

struct tree_node {
    struct tree_node *parent;
    struct tree_node *right;
    struct tree_node *left;
    struct tree_node *eldest;
    id_type id;
    int level;
};

struct tree_root {
    struct tree_node * node;
    id_type cnt;
    int len;
};

static inline int rtree_inc(struct tree_root* root) {
    return ++ root->cnt;
}

static inline struct tree_node *  node_new(id_type id) {
    struct tree_node *node = (struct tree_node *) malloc(sizeof(struct tree_node));
    if (node == NULL) {
        return node;
    }
    memset(node, 0, sizeof (struct tree_node));
    node->id = id;
    return node;
}

static inline void add_child(struct tree_node * parent, struct tree_node *node) {
    struct tree_node * last = parent->eldest;

    node->parent = parent;
    parent->eldest = node;
    node->level = parent->level + 1;

    node->right = last;
    if (last) {
        last->left = node;
    }
}

static inline void uplevel(struct tree_node * const node, void *arg) {
    node->level --;
}

static inline void walk_node(struct tree_node * const node, void (*func)(struct tree_node * const n, void *a), void* arg) {
    struct tree_node * child, *next;
    child = node->eldest;
    if (child) {
        walk_node(child, func, arg);
    }

    next = node->right;
    if (next) {
        walk_node(next, func, arg);
    }
    func(node, arg);
}

// confirm right is not null.
static inline void node_move_child2peer(struct tree_node * right, struct tree_node * const eldest) {
    struct tree_node *left;
    while (right) {  // to last child, should confirm right is not null.
        left = right;
        right = right->right;
    }
    left->right = eldest;
    if (eldest) {
        eldest->left = left;
    }
}

static inline void node_remove(struct tree_root * const root,
                struct tree_node * const node,
                void (*strip)(struct tree_node * const n, void *a),
                void *arg) {
    struct tree_node * const left = node->left;
    struct tree_node * const right = node->right;
    struct tree_node * const parent = node->parent;
    struct tree_node * const eldest = node->eldest;
    struct tree_node *nxt;

    if (parent) {  // parent is not root.
        if (parent->eldest == node) {  // conform eldest
            parent->eldest = right;
        }
    } else {  // is root.
        if (root->node == node) {
            root->node = right;
        }
    }

    if (left) {
        left->right = right;
    }
    if (right) {
        right->left = left;
    }

    if (parent) {
        nxt = parent->eldest;
        if (nxt == NULL) {
            parent->eldest = eldest;
        } else {
            node_move_child2peer(nxt, eldest);
        }
    } else {
        nxt = root->node;
        if (nxt == NULL) {
            root->node = eldest;
        } else {
            node_move_child2peer(nxt, eldest);
        }
    }

    // child node up level
    if (eldest) {
        nxt = eldest;
        do {
            nxt->parent = node->parent;
            uplevel(nxt, NULL);
            if (nxt->eldest) {
                walk_node(nxt->eldest, uplevel, NULL);
            }
            nxt = nxt->right;
        } while (nxt);
    }

    strip(node, arg);
}

static inline void node_strip(struct tree_root * const root,
                struct tree_node * const node,
                void (*strip)(struct tree_node * const n, void *a),
                void *arg) {
    struct tree_node * const left = node->left;
    struct tree_node * const right = node->right;
    struct tree_node * const parent = node->parent;

    if (parent) {  // node is not root.
        if (parent->eldest == node) {
            parent->eldest = right;
        }
    } else {  // for root.
        if (root->node == node) {
            root->node = right;
        }
    }

    if (left) {
        left->right = right;
    }
    if (right) {
        right->left = left;
    }

    node->right = NULL;
    walk_node(node, strip, arg);
}

void new_tree(struct tree_root *root);

#endif //TREELIB_TREE_H
