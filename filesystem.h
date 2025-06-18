// filesystem.h
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "btree.h"
#include "inode.h"
#include <string>
#include <filesystem>
namespace fs = std::filesystem;

class FileSystem {
public:
    FileSystem();
    void createFile(const std::string& name);
    void createDirectory(const std::string& name);
    void renameNode(const std::string& oldName, const std::string& newName);
    void deleteNode(const std::string& name);
    void changePermissions(const std::string& name, const std::string& permissions);
    void listFiles();
    void listFilesRecursively(std::shared_ptr<INode> node, int depth);
    void searchFileByName(const std::string& name);
    void showCurrentDirectory();
    std::shared_ptr<INode> getCurrentDirectory();
    void changeDirectory(const std::string& name);
    void changeToParentDirectory();
    void displayHelp();

void addToHistory(const std::string& command);
void showHistory() const;


private:
    void mapFileSystem(const fs::path& path, std::shared_ptr<INode> parentNode);
    void findRecursive(std::shared_ptr<INode> currentNode, const std::string& name, const std::string& currentPath);

    BTree tree;
    std::shared_ptr<INode> root;
    std::shared_ptr<INode> currentDirectory;
    unsigned int nextId;
    fs::path rootPath;
    std::vector<std::string> command_history;

};

#endif // FILESYSTEM_H