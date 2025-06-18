// main.cpp
#include "filesystem.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    FileSystem fs;
    string line; 
    while (true) {
        fs.showCurrentDirectory();
        cout << "> "; 
        getline(cin, line); 

        if (line == "exit") break;
        if (line.empty()) continue;

        string command; 
        string arg1, arg2; 
        size_t first_space = line.find(' ');
        command = line.substr(0, first_space);
        
        if (first_space != string::npos) { 
            size_t second_space = line.find(' ', first_space + 1);
            if (second_space != string::npos) { 
                arg1 = line.substr(first_space + 1, second_space - (first_space + 1));
                arg2 = line.substr(second_space + 1);
            } else {
                arg1 = line.substr(first_space + 1);
            }
        }
        
        
        if (command == "touch") {
            if (!arg1.empty()) fs.createFile(arg1);
        } else if (command == "mkdir") {
            if (!arg1.empty()) fs.createDirectory(arg1);
        } else if (command == "mv") {
            if (!arg1.empty() && !arg2.empty()) fs.renameNode(arg1, arg2);
        } else if (command == "rm") {
            if (!arg1.empty()) fs.deleteNode(arg1);
        } else if (command == "chmod") {
            if (!arg1.empty() && !arg2.empty()) fs.changePermissions(arg1, arg2);
        } else if (command == "ls") {
            if (arg1 == "-R") {
                fs.listFilesRecursively(fs.getCurrentDirectory(), 0);
            } else {
                fs.listFiles();
            }
        } else if (command == "find") {
            if (!arg1.empty()) fs.searchFileByName(arg1);
        } else if (command == "cd") {
            if (!arg1.empty()) fs.changeDirectory(arg1);
        } else if (command == "cd..") {
             fs.changeToParentDirectory();
        
        } else if (command == "history") {
            fs.showHistory();
        } else {
            cout << "Comando desconocido: " << line << endl; 
        }
    }

    return 0;
}