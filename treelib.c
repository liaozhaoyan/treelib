//
// Created by 廖肇燕 on 2023/10/26.
//
#include <lua.h>
#include <lauxlib.h>

#include "tree.h"

#define MT_NAME "TREE_HANDLE"

#ifndef lua_setuservalue
#define lua_setuservalue(L, n) lua_setfenv(L, n)
#endif
#ifndef lua_getuservalue
#define lua_getuservalue(L, n) lua_getfenv(L, n)
#endif

static struct tree_node *get_node(lua_State *L, int tree, int id_index)
{
    int top = lua_gettop(L);

    lua_getuservalue(L, tree);
    lua_getfield(L, -1, "node_map");
    lua_pushvalue(L, id_index);
    lua_rawget(L, -2);

    struct tree_node* p = lua_isuserdata(L, -1) ? lua_touserdata(L, -1) : NULL;
    lua_settop(L, top);   // back to call stack.
    return p;
}

static int a_child(lua_State *L) {
    struct tree_root *root = (struct tree_root *)luaL_checkudata(L, 1, MT_NAME);
    struct tree_node *parent, *node;
    id_type id_parent, id_new;

    luaL_argcheck(L, root != NULL, 1, "treelib expected");
    id_parent = luaL_checkinteger(L, 2);
    if (lua_isnoneornil(L, 3)) {
        lua_pushnil(L);
        return 1;
    }

    if (id_parent > 0) {
        parent = get_node(L, 1, 2);
    } else {
        parent = root->node;
    }

    id_new = rtree_inc(root);
    node = node_new(id_new);
    if (node == NULL) {
        luaL_error(L, "malloc node failed.");
    }
    add_child(parent, node);

    lua_getuservalue(L, 1);

    lua_getfield(L, -1, "value_map");
    lua_pushinteger(L, id_new);
    lua_pushvalue(L, 3);
    lua_rawset(L, -3);
    lua_pop(L, 1);

    lua_getfield(L, -1, "node_map");
    lua_pushinteger(L, id_new);
    lua_pushlightuserdata(L, (void*)node);
    lua_rawset(L, -3);
    lua_pop(L, 2);  // should back to stack 3.

    root->len ++;
    lua_pushinteger(L, id_new);
    return 1;
}

static int root(lua_State *L) {
    struct tree_root *root = (struct tree_root *) luaL_checkudata(L, 1, MT_NAME);
    struct tree_node *node;

    luaL_argcheck(L, root != NULL, 1, "treelib expected");
    node = root->node;
    lua_pushinteger(L, node->id);
    return 1;
}

struct node_level_args {
    lua_State *L;
    int level;
};

static void cb_level(struct tree_node * const node, void* arg) {
    struct node_level_args* parg = (struct node_level_args*)arg;

    if (parg->level < node->level) {
        parg->level = node->level;
    }
}

static int depth(lua_State *L) {
    struct tree_root *root = (struct tree_root *) luaL_checkudata(L, 1, MT_NAME);
    struct tree_node *node;
    struct node_level_args arg = {L, 0};

    luaL_argcheck(L, root != NULL, 1, "treelib expected");
    node = root->node->eldest;
    if (node) {
        walk_node(node, cb_level, &arg);
    }
    lua_pushinteger(L, arg.level + 1);
    return 1;
}

static int tree_len(lua_State *L) {
    struct tree_root *root = (struct tree_root *) luaL_checkudata(L, 1, MT_NAME);

    luaL_argcheck(L, root != NULL, 1, "treelib expected");
    lua_pushinteger(L, root->len);
    return 1;
}

static int get(lua_State *L) {
    struct tree_root *root = (struct tree_root *)luaL_checkudata(L, 1, MT_NAME);
    id_type id;

    luaL_argcheck(L, root != NULL, 1, "treelib expected");
    id = luaL_checkinteger(L, 2);
    if (id < 0) {
        lua_pushnil(L);
        return 1;
    }

    lua_getuservalue(L, 1);

    lua_getfield(L, -1, "value_map");
    lua_pushinteger(L, id);
    lua_rawget(L, -2);

    lua_pushvalue(L, -1);
    return 1;
}

static int set(lua_State *L) {
    struct tree_root *root = (struct tree_root *)luaL_checkudata(L, 1, MT_NAME);
    id_type id;

    luaL_argcheck(L, root != NULL, 1, "treelib expected");
    id = luaL_checkinteger(L, 2);
    if (id < 0) {
        lua_pushnil(L);
        return 1;
    }

    lua_getuservalue(L, 1);

    lua_getfield(L, -1, "value_map");
    lua_pushinteger(L, id);
    lua_pushvalue(L, 3);
    lua_rawset(L, -3);
    lua_pop(L, 2);

    lua_pushboolean(L, 1);
    return 1;
}

static int level(lua_State *L) {
    struct tree_root *root = (struct tree_root *) luaL_checkudata(L, 1, MT_NAME);
    struct tree_node *node;
    int level;

    luaL_argcheck(L, root != NULL, 1, "treelib expected");
    node = get_node(L, 1, 2);
    level = node ? node->level + 1: -1;
    lua_pushinteger(L, level);
    return 1;
}

static int parent(lua_State *L) {
    struct tree_root *root = (struct tree_root *) luaL_checkudata(L, 1, MT_NAME);
    struct tree_node *node;
    id_type id;

    luaL_argcheck(L, root != NULL, 1, "treelib expected");
    node = get_node(L, 1, 2);
    if (node && node->parent) {
        lua_pushinteger(L, node->parent->id);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int children_iterator(lua_State *L) {
    struct tree_node *node = (struct tree_node *)lua_touserdata(L, lua_upvalueindex(1));
    if (node) {
        id_type id = node->id;

        lua_pushlightuserdata(L, node->right);
        lua_pushvalue(L, -1);
        lua_replace(L, lua_upvalueindex(1));
        lua_pop(L, 1);

        lua_getuservalue(L, lua_upvalueindex(2));  //self is the second var

        lua_getfield(L, -1, "value_map");
        lua_pushinteger(L, id);
        lua_rawget(L, -2);

        lua_pushinteger(L, id);
        lua_pushinteger(L, node->level + 1);
        lua_pushvalue(L, -3);

        return 3;
    }
    return 0;
}

static int children(lua_State *L) {
    struct tree_root *root = (struct tree_root *) luaL_checkudata(L, 1, MT_NAME);
    struct tree_node *node;
    id_type id;
    luaL_argcheck(L, root != NULL, 1, "treelib expected");
    id = luaL_checkinteger(L, 2);
    node = get_node(L, 1, 2);

    if (node) {
        lua_pushlightuserdata(L, node->eldest);   // eldest children
        lua_pushvalue(L, 1);  //meta table.
    } else {
        lua_pushlightuserdata(L, NULL);
        lua_pushvalue(L, 1);
    }
    lua_pushcclosure(L, &children_iterator, 2);
    return 1;
}

static void cb_strip(struct tree_node * const node, void* arg) {
    lua_State *L = (lua_State *)arg;
    struct tree_root *root = (struct tree_root *) luaL_checkudata(L, 1, MT_NAME);

    lua_getuservalue(L, 1);

    lua_getfield(L, -1, "value_map");
    lua_pushinteger(L, node->id);
    lua_pushnil(L);
    lua_rawset(L, -3);
    lua_pop(L, 1);

    lua_getfield(L, -1, "node_map");  // clear node
    lua_pushinteger(L, node->id);
    lua_pushnil(L);
    lua_rawset(L, -3);
    lua_pop(L, 2);

    root->len --;
//    printf("recycle: %ld, top:%d\n", node->id, lua_gettop(L));
    free(node);   // free memory
}

static int remove_subtree(lua_State *L) {
    struct tree_root *root = (struct tree_root *) luaL_checkudata(L, 1, MT_NAME);
    struct tree_node *node;

    luaL_argcheck(L, root != NULL, 1, "treelib expected");
    node = get_node(L, 1, 2);
    if (node) {
        node_strip(root, node, cb_strip, L);
        lua_pushboolean(L, 1);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int root_count(struct tree_root *root) {
    struct tree_node * nxt = root->node;
    int count = 0;

    while (nxt) {
        count ++;
        nxt = nxt->right;
    }
    return count;
}

static int remove_node(lua_State *L) {
    struct tree_root *root = (struct tree_root *) luaL_checkudata(L, 1, MT_NAME);
    struct tree_node *node;

    luaL_argcheck(L, root != NULL, 1, "treelib expected");
    node = get_node(L, 1, 2);
    if (node) {
        node_remove(root, node, cb_strip, L);
        if (root_count(root) > 1) {  // has multi root node.
            luaL_error(L, "this tree has multi root node.");
        }
        lua_pushboolean(L, 1);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static void showData(struct tree_node * const node, void * dummy){
    printf("show node %ld, level%d\n", node->id, node->level);
}

static int print(lua_State *L) {
    struct tree_root *root = (struct tree_root *)luaL_checkudata(L, 1, MT_NAME);
    struct tree_node *node = root->node;
    if (node) {
        walk_node(node, showData, NULL);
    }
    return 0;
}

static int new(lua_State *L) {
    struct tree_root *root;
    int top = lua_gettop(L);
    if (top == 0) {  //set default value.
        lua_pushstring(L, "root");
    }

    root = (struct tree_root*)lua_newuserdata(L, sizeof(struct tree_root));
    new_tree(root);

    luaL_getmetatable(L, MT_NAME);
    lua_setmetatable(L, -2);

    lua_newtable(L);  //new table for userdata.

    lua_newtable(L);
    lua_setfield(L, -2, "value_map"); /* k = key id, value = value*/

    lua_newtable(L);
    lua_setfield(L, -2, "node_map"); /* k = key id, value = node_ptr*/
    lua_setuservalue(L, -2);

    // set root data.
    lua_getuservalue(L, 2);

    lua_getfield(L, -1, "value_map");
    lua_pushinteger(L, 0);
    lua_pushvalue(L, 1);
    lua_rawset(L, -3);
    lua_pop(L, 1);

    lua_getfield(L, -1, "node_map");
    lua_pushinteger(L, 0);
    lua_pushlightuserdata(L, root->node);
    lua_rawset(L, -3);
    lua_pop(L, 2);

    return 1;
}

static int gc(lua_State *L) {
    struct tree_root *root = (struct tree_root *) luaL_checkudata(L, 1, MT_NAME);
    struct tree_node *node;

    luaL_argcheck(L, root != NULL, 1, "treelib expected");
    node = root->node;
    while (node) {
        node_strip(root, node, cb_strip, L);
        node = root->node;
    }
    return 0;
}

static luaL_Reg module_m[] = {
        {"add_child", a_child},
        {"root", root},
        {"depth", depth},
        {"len", tree_len},
        {"get", get},
        {"set", set},
        {"level", level},
        {"parent", parent},
        {"children", children},
        {"remove_subtree", remove_subtree},
        {"remove_node", remove_node},
        {"print", print},
        {NULL, NULL}
};

static luaL_Reg module_f[] = {
        {"new", new},
        {NULL, NULL}
};

int luaopen_treelib(lua_State *L) {
    luaL_newmetatable(L, MT_NAME);

    lua_createtable(L, 0, sizeof(module_m) / sizeof(luaL_Reg) - 1);
#if LUA_VERSION_NUM > 501
    luaL_setfuncs(L, module_m, 0);
#else
    luaL_register(L, NULL, module_m);
#endif
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, set);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, get);
    lua_setfield(L, -2, "__call");
    lua_pushcfunction(L, gc);
    lua_setfield(L, -2, "__gc");
    lua_pushcfunction(L, tree_len);
    lua_setfield(L, -2, "__len");

    lua_pop(L, 1);

#if LUA_VERSION_NUM > 501
    luaL_newlib(L, module_f);
#else
    luaL_register(L, "treelib", module_f);
#endif

    return 1;
}
