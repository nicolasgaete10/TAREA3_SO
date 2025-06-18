// filesystem.cpp
#include "filesystem.h"
#include "colors.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <deque> 

fs::path get_current_physical_path(const std::shared_ptr<INode>& currentDirectory, const fs::path& rootPath);


// --- Implementación de FileSystem ---

FileSystem::FileSystem() : tree(3), nextId(1) {
    rootPath = fs::current_path() / "root";
    if (!fs::exists(rootPath)) {
        fs::create_directory(rootPath);
    }
    root = std::make_shared<INode>("root", true, nullptr);
    root->id = nextId++;
    currentDirectory = root;
    tree.insert(root);
    mapFileSystem(rootPath, root);
}

// Función auxiliar para obtener la ruta física actual en el disco
fs::path get_current_physical_path(const std::shared_ptr<INode>& currentDirectory, const fs::path& rootPath) {
    std::deque<std::string> path_parts;
    std::shared_ptr<INode> temp = currentDirectory;

    // Recorremos el árbol hacia arriba desde el directorio actual hasta la raíz
    // para construir la ruta relativa.
    while (temp != nullptr && temp->parent.lock() != nullptr) {
        path_parts.push_front(temp->name);
        temp = temp->parent.lock();
    }

    fs::path final_path = rootPath;
    for (const auto& part : path_parts) {
        final_path /= part;
    }
    return final_path;
}

void FileSystem::createFile(const std::string& name) {
    // 1. Crear el nodo lógico
    auto file = std::make_shared<INode>(name, false, currentDirectory);
    file->id = nextId++;
    currentDirectory->children.push_back(file);
    tree.insert(file);

    // 2. Crear el archivo físico en la ruta correcta
    fs::path physical_path = get_current_physical_path(currentDirectory, rootPath);
    std::ofstream ofs(physical_path / name);
    ofs.close();
    std::cout << GREEN << "Archivo '" << name << "' creado." << RESET << std::endl;
}

void FileSystem::createDirectory(const std::string& name) {
    // 1. Crear el nodo lógico
    auto dir = std::make_shared<INode>(name, true, currentDirectory);
    dir->id = nextId++;
    currentDirectory->children.push_back(dir);
    tree.insert(dir);
    
    // 2. Crear el directorio físico en la ruta correcta
    fs::path physical_path = get_current_physical_path(currentDirectory, rootPath);
    fs::create_directory(physical_path / name);
    std::cout << GREEN << "Directorio '" << name << "' creado." << RESET << std::endl;
}

void FileSystem::renameNode(const std::string& oldName, const std::string& newName) {
    for (auto& child : currentDirectory->children) {
        if (child->name == oldName) {
            fs::path physical_path = get_current_physical_path(currentDirectory, rootPath);
            fs::rename(physical_path / oldName, physical_path / newName);
            child->name = newName;
            std::cout << GREEN << "Nodo renombrado a '" << newName << "'." << RESET << std::endl;
            return;
        }
    }
    std::cout << RED << "Nodo no encontrado!" << RESET << std::endl;
}

void FileSystem::deleteNode(const std::string& name) {
    for (auto it = currentDirectory->children.begin(); it != currentDirectory->children.end(); ++it) {
        if ((*it)->name == name) {
            fs::path physical_path = get_current_physical_path(currentDirectory, rootPath);
            fs::remove_all(physical_path / name);
            currentDirectory->children.erase(it);
            std::cout << GREEN << "Nodo '" << name << "' eliminado." << RESET << std::endl;
            return;
        }
    }
    std::cout << RED << "Nodo no encontrado!" << RESET << std::endl;
}

void FileSystem::mapFileSystem(const fs::path& path, std::shared_ptr<INode> parentNode) {
    for (const auto& entry : fs::directory_iterator(path)) {
        auto node = std::make_shared<INode>(entry.path().filename().string(), entry.is_directory(), parentNode);
        node->id = nextId++;
        node->size = entry.is_directory() ? 0 : fs::file_size(entry);
        parentNode->children.push_back(node);
        tree.insert(node);
        if (entry.is_directory()) {
            mapFileSystem(entry.path(), node);
        }
    }
}

void FileSystem::changePermissions(const std::string& name, const std::string& permissions) {
    try {
        for (auto& child : currentDirectory->children) {
            if (child->name == name) {
                // Convertir el string de permisos a un entero
                int perm = std::stoi(permissions);
                if (perm < 0 || perm > 7) {
                    std::cout << RED << "Permisos inválidos!" << RESET << std::endl;
                    return;
                }

                // Convertir el número a permisos
                std::string permStr = "---";
                if (perm & 4) permStr[0] = 'r';
                if (perm & 2) permStr[1] = 'w';
                if (perm & 1) permStr[2] = 'x';

                child->permissions = permStr + "r--r--"; // Permisos para user, group y others
                return;
            }
        }
        std::cout << RED << "Nodo no encontrado!" << RESET << std::endl;
    } catch (const std::invalid_argument&) {
        std::cout << RED << "Permisos inválidos: no es un número!" << RESET << std::endl;
    } catch (const std::out_of_range&) {
        std::cout << RED << "Permisos inválidos: fuera de rango!" << RESET << std::endl;
    }
}

void FileSystem::listFiles() {
    std::cout << GREEN << "Lista de archivos en el directorio actual:" << RESET << std::endl;
    for (const auto& child : currentDirectory->children) {
        char time_buf[100];
        std::strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", std::localtime(&child->creationTime));
        std::cout <<  child->id << " " << (child->isDirectory ? "d" : "-") << child->permissions << " "
                  << child->name << " " << time_buf << std::endl;
    }
}

void FileSystem::listFilesRecursively(std::shared_ptr<INode> node, int depth) {
    std::string indent(depth * 2, ' ');
    char time_buf[100];
    std::strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", std::localtime(&node->creationTime));
    std::cout << indent << (node->isDirectory ? "d" : "-") << node->permissions << " " << node->name << " "
              << time_buf << std::endl;
    for (const auto& child : node->children) {
        listFilesRecursively(child, depth + 1);
    }
}

void FileSystem::searchFileByName(const std::string& name) {
    std::cout << "Buscando '" << name << "' desde la raíz..." << std::endl;
    
    if (root->name == name) {
        std::cout << "Encontrado: /" << root->name << std::endl;
    }

    findRecursive(root, name, "/" + root->name);
}

void FileSystem::findRecursive(std::shared_ptr<INode> currentNode, const std::string& name, const std::string& basePath) {
    for (const auto& child : currentNode->children) {
        std::string childPath = basePath + "/" + child->name;

        if (child->name == name) {
            std::cout << "Encontrado: " << childPath << std::endl;
        }

        if (child->isDirectory) {
            findRecursive(child, name, childPath);
        }
    }
}

void FileSystem::showCurrentDirectory() {
    std::string full_path = "";
    auto temp = currentDirectory;
    if (temp->name == "root" && !temp->parent.lock()) {
        full_path = "/root";
    } else {
        while(temp->parent.lock()){
            full_path = "/" + temp->name + full_path;
            temp = temp->parent.lock();
        }
        full_path = "/root" + full_path;
    }
    
    std::cout << BLUE << "Directorio actual: " << YELLOW << full_path << RESET << std::endl;
}

std::shared_ptr<INode> FileSystem::getCurrentDirectory() {
    return currentDirectory;
}

void FileSystem::changeDirectory(const std::string& name) {
    if (name == "..") {
        changeToParentDirectory();
        return;
    }
    for (auto& child : currentDirectory->children) {
        if (child->name == name && child->isDirectory) {
            currentDirectory = child;
            return;
        }
    }
    std::cout << RED <<"Directorio no encontrado!" << RESET << std::endl;
}

void FileSystem::changeToParentDirectory() {
    if (auto parent_ptr = currentDirectory->parent.lock()) {
        currentDirectory = parent_ptr;
    }
}


void FileSystem::addToHistory(const std::string& command) {
    if (!command.empty()) {
        command_history.push_back(command);
    }
}

void FileSystem::showHistory() const {
    std::cout << GREEN << "--- Historial de Comandos ---" << RESET << std::endl;
    for (size_t i = 0; i < command_history.size(); ++i) {
        std::cout << std::setw(4) << i + 1 << ": " << command_history[i] << std::endl;
    }
    std::cout << GREEN << "-----------------------------" << RESET << std::endl;
}


