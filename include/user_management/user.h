#include <stdio.h>
#include <string>
#include <vector>

struct User {
    std::string username;
    std::string password;
    int age;
};

std::vector<User> users;

void addUser(const std::string& username, const std::string& password, int age) ;

void deleteUser(const std::string& username);

void printUsers();

void manageUser();