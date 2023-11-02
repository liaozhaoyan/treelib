# Introduction

refer to [python treelib](https://treelib.readthedocs.io/en/latest/).
Tree is an important data structure in computer science. Examples are shown in ML algorithm designs such as random forest tree and software engineering such as file system index. treelib is created to provide an efficient implementation of tree data structure in Lua.

# Installation

The rapidest way to install treelib is using the package management tools like luarocks.

```
luarocks install treelib
```

# example

refer to [test.lua]() create function

```
local treelib = require("treelib")
local wrap = require("treewrap")

local function create(start)
    start = start or 0
    local tree = treelib.new()
    local n1 = tree:add_child(tree:root(), "hello")
    local n2 = tree:add_child(n1, "world" .. n1 + start)
    n2 = tree:add_child(n1, "world" .. n2 + start)
    n2 = tree:add_child(n1, "world" .. n2 + start)
    n2 = tree:add_child(n1, "world" .. n2 + start)
    n2 = tree:add_child(n1, "world" .. n2 + start)
    local n3 = tree:add_child(n2, "world" .. n2 + start)
    n3 = tree:add_child(n2, "world" .. n3 + start)
    n3 = tree:add_child(n2, "world" .. n3 + start)
    n3 = tree:add_child(n2, "world" .. n3 + start)
    n3 = tree:add_child(n3, "world" .. n3 + start)
    return tree
end

local tree = create()
wrap.show(tree)
```

# module

lua treelib is composed of two components: treelib (implemented in C) and treewrap (encapsulated in lua).

## treelib

treelib provides the basic API for tree operations.

treelib function:

### new

```
tree = treelib.new(rootValue)
```
create a new tree data structure, By default, a root node is created。

*  param rootValue: root node data value, any type. If not set, the default data value is "root".
*  return: new tree structure.

treelib member functions, call method tree:add_child,

### add_child

Create a child node for given parent node

```
local nodeId = tree:add_child(parentId, parentValue)
```

*  param parentId: parent node id in the tree, lua_number type.
*  param parentValue: parent data value, any type.
*  return: child node unique id, lua_number type.

### root

Gets the root node id from the tree structure.

```
local nodeId = tree:root()
```

*  return: root node unique id, lua_number type.

### depth

Get the maximum level of this tree or the level of the given node.

```
local depth = tree:deepth(nodeId)
```

*  param parentId: given node id, If not set, the default id is root id, lua_number type.
*  return: the maximum level, lua_number type.

### len

Get the number of nodes of the whole tree.  

```
local len = tree:len()
local len = #tree   --Equivalent method
```
*  return: number of nodes of the whole tree, lua_number type.

### get

Get the corresponding value from the given node id.

```
local value = tree:get(nodeId)
local value = tree(nodeId)  --Equivalent method
-- The following method is prohibited, there is a conflict with the member function method
-- local value = tree[nodeId]
```

*  param nodeId: given node id, lua_number type.
*  return: data value, any type.

### set

Update the corresponding value from the given node id.

```
tree:set(nodeId, value)
tree[nodeId] = value  --Equivalent method
```

*  param nodeId: given node id, lua_number type.
*  param value: data value, any type.

### level

Get the node level in this tree. 

```
local level = tree:level(nodeId)
```
*  param nodeId: given node id, lua_number type.
*  return: node level, lua_number type.

### parent

Get parent Node object of given id.

```
local parentId = tree:parent(nodeId)
```

*  param nodeId: given node id, lua_number type.
*  return: node parent id, lua_number type.

### children

Returns all chilren nodes, levels and values in an iterator.

```
for childId, level, value in tree:children(id) do
	print(childId, level, value)
end
```

*  param nodeId: given node id, lua_number type.
*  return iterator.
*  iterator child id, lua_number type.
*  iterator child level, lua_number type.
*  iterator child value, any type.

### remove_subtree

remove subtree with nodeId being the root.

```
tree:remove_subtree(nodeId)
```

*  param nodeId: given node id, lua_number type.
*  return: true if success, false if failed(node id is not in the tree?).

### remove_node

remove a sub node with node id being the root, node children will level up.

```
tree:remove_subtree(nodeId)
```

*  param nodeId: given node id, lua_number type.
*  return: true if success, false if failed(node id is not in the tree?).


## treewap

treewap provides the wrap API for tree operations.

### show

Print the tree structure in hierarchy style.

```
wrap.show(tree, nodeId, fmt)
```

*  param tree: given tree structure, tree type.
*  param nodeId: given node id, If not set, the default id is root id, lua_number type.
*  param fmt: print callback function, If not set, the default function is defaultFmt:

```
local function defaultFmt(id, level, value)
    local head = ""
    if level > 0 then
        head = string.rep(" ", (level - 1) * 4) .. " |__"
    end
    local content = string.format("%s(%d):%s", head, id, tostring(value))
    print(content)
end
```

### contains

Check if the tree contains node of given id

```
wrap.contains(tree, nodeId)
```

*  param tree: given tree structure, tree type.
*  param nodeId: given node id, lua_number type.

### is_branch

Check node of given id is branch

```
wrap.is_branch(tree, nodeId)
```

*  param tree: given tree structure, tree type.
*  param nodeId: given node id, lua_number type.
*  return: true if is branch, otherwise, false.

### leaves

Get leaves of the whole tree or a subtree.

```
wrap.leaves(tree, nodeId)
```

*  param tree: given tree structure, tree type.
*  param nodeId: given node id, If not set, the default id is root id, lua_number type.
*  return: lua table, key is nodeId, value is node value.

### is_ancestor

Check if the @ancestorId the preceding nodes of @childId.

```
wrap.is_ancestor(tree, ancestorId, childId)
```

*  param tree: given tree structure, tree type.
*  param ancestorId: given ancestorId id, lua_number type.
*  param childId: given childId id, lua_number type.
*  return: true or false.

### toList

Convert tree structure to lua list.

```
wrap.toList(tree, id, listFunc)
```

*  param tree: given tree structure, tree type.
*  param id: given node id, If not set, the default id is root id, lua_number type.
*  praam listFunc: callback function to convert node to table list. if not set, The default callback function is as follows:
*  return: lua table.

```
function defaultList(id, level, value)
    return {
        id = id,
        level = level,
        value = value,
    }
end
```

### siblings

Return the siblings of given @id.

If @id is root or there are no siblings, an empty list is returned.

```
wrap.siblings(tree, id)
```

*  param tree: given tree structure, tree type.
*  param id: given node id, lua_number type.
* return: lua table.

### subtree

Return a shallow COPY of subtree with nid being the new root.

```
wrap.subtree(tree, id)
```

*  param tree: given tree structure, tree type.
*  param id: given node id, If not set, the default id is root id, lua_number type.
*  return: tree structure, tree type.

### paste

Paste a @tree to the original one by linking the root of new tree to given node (id).

```
wrap.paste(tree, id, child)
```

*  param tree: given tree structure, tree type.
*  param id: given node id to add, If not set, the default id is root id, lua_number type.
*  param child: given child tree structure, tree type.

### filter_nodes

Filters nodes by function.

```
wrap.filter_nodes(tree, filter, id)
```

*  param tree: given tree structure, tree type.
*  param filter: filter function. eg:

```
function filter_func(id, level, value)
    if string.sub(value, 1, 5) == "world" then
        return true
    end
    return false
end
```
 
*  param id: given node id to add, If not set, the default id is root id, lua_number type.
*  return: lua table

### move_node

Move node @id from its parent to another parent @ newParent.

**Note: The old node ID will be invalid, please use the new node ID**

```
wrap.move_node(tree, id, newParent)
```

*  param tree: given tree structure, tree type.
*  param id: given node id to move, lua_number type.
*  param newParent: given node id to add, lua_number type.
*  return: new node id




