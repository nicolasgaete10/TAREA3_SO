// inode.h
#ifndef INODE_H
#define INODE_H

#include <string>
#include <vector>
#include <memory>
#include <ctime>

// Declaración anticipada para evitar dependencia circular con el puntero 'parent'
struct INode;

struct INode {
    std::string name;
    size_t size;
    std::string permissions;
    unsigned int id;
    bool isDirectory;
    std::time_t creationTime;
    std::vector<std::shared_ptr<INode>> children;
    std::weak_ptr<INode> parent; // Usar weak_ptr para el padre y evitar ciclos de referencia

    INode(std::string name, bool isDirectory, std::shared_ptr<INode> parent);
};

#endif // INODE_H