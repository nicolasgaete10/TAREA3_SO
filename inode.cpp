// inode.cpp
#include "inode.h"

INode::INode(std::string name, bool isDirectory, std::shared_ptr<INode> parent)
    : name(std::move(name)), size(0), permissions("rwxr--r--"), id(0), 
      isDirectory(isDirectory), parent(parent) {
    creationTime = std::time(nullptr);
}