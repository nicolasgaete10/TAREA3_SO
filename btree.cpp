// btree.cpp
#include "btree.h"
#include <iostream>

struct BTree::BTreeNode { 
    std::vector<std::shared_ptr<INode>> keys;
    std::vector<std::shared_ptr<BTreeNode>> children;
    bool leaf;
    int t;

    BTreeNode(int t_val, bool leaf_val);
    void traverse();
    std::shared_ptr<INode> search(unsigned int k);
    void insertNonFull(std::shared_ptr<INode> k);
    void splitChild(int i, std::shared_ptr<BTreeNode> y);
};

// --- Implementación de BTree ---
BTree::BTree(int t_val) : t(t_val), root(nullptr) {}

void BTree::traverse() {
    if (root != nullptr) root->traverse();
}

std::shared_ptr<INode> BTree::search(unsigned int k) {
    return (root == nullptr) ? nullptr : root->search(k);
}

void BTree::insert(std::shared_ptr<INode> k) {
    if (root == nullptr) {
        root = std::make_shared<BTreeNode>(t, true);
        root->keys.push_back(k);
    } else {
        if (root->keys.size() == 2 * t - 1) {
            auto s = std::make_shared<BTreeNode>(t, false);
            s->children.push_back(root);
            s->splitChild(0, root);
            int i = 0;
            if (s->keys[0]->id < k->id) i++;
            s->children[i]->insertNonFull(k);
            root = s;
        } else {
            root->insertNonFull(k);
        }
    }
}

// --- Implementación de BTreeNode ---
BTree::BTreeNode::BTreeNode(int t_val, bool leaf_val) : t(t_val), leaf(leaf_val) {}

void BTree::BTreeNode::traverse() {
    int i;
    for (i = 0; i < keys.size(); i++) {
        if (!leaf) children[i]->traverse();
        std::cout << " " << keys[i]->id;
    }
    if (!leaf) children[i]->traverse();
}

std::shared_ptr<INode> BTree::BTreeNode::search(unsigned int k) {
    int i = 0;
    while (i < keys.size() && k > keys[i]->id) i++;
    if (i < keys.size() && keys[i]->id == k) return keys[i];
    return (leaf) ? nullptr : children[i]->search(k);
}

void BTree::BTreeNode::insertNonFull(std::shared_ptr<INode> k) {
    int i = keys.size() - 1;
    if (leaf) {
        keys.push_back(nullptr); // Hacer espacio
        while (i >= 0 && keys[i]->id > k->id) {
            keys[i + 1] = keys[i];
            i--;
        }
        keys[i + 1] = k;
    } else {
        while (i >= 0 && keys[i]->id > k->id) i--;
        i++;
        if (children[i]->keys.size() == 2 * t - 1) {
            splitChild(i, children[i]);
            if (keys[i]->id < k->id) i++;
        }
        children[i]->insertNonFull(k);
    }
}

void BTree::BTreeNode::splitChild(int i, std::shared_ptr<BTreeNode> y) {
    auto z = std::make_shared<BTreeNode>(y->t, y->leaf);
    
    for (int j = 0; j < t - 1; j++) {
        z->keys.push_back(y->keys[j + t]);
    }
    
    if (!y->leaf) {
        for (int j = 0; j < t; j++) {
            z->children.push_back(y->children[j + t]);
        }
    }
    
    keys.insert(keys.begin() + i, y->keys[t - 1]);
    
    y->keys.resize(t - 1);
    if (!y->leaf) {
        y->children.resize(t);
    }

    children.insert(children.begin() + i + 1, z);
}