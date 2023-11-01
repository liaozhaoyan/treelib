//
// Created by 廖肇燕 on 2023/10/26.
//

#include "tree.h"

void new_tree(struct tree_root * root){
    root->cnt = 0;  // counter start at 0
    root->len = 1;  // has a default root value.
    root->node = node_new(0);
    root->node->level = 0;
}

