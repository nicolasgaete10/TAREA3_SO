// btree.h
#ifndef BTREE_H
#define BTREE_H

#include "inode.h"
#include <vector>
#include <memory>

class BTree {
public:
    BTree(int t);
    void traverse();
    std::shared_ptr<INode> search(unsigned int k);
    void insert(std::shared_ptr<INode> k);

private:
    struct BTreeNode; 
    std::shared_ptr<BTreeNode> root;
    int t;
};

#endif // BTREE_H