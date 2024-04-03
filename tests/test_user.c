#include <stdio.h>
#include "../include/user_management/user.h"

int main() {
    // Test case 1: Create a user and verify the username
    User user1 = createUser("JohnDoe");
    if (strcmp(user1.username, "JohnDoe") == 0) {
        printf("Test case 1 passed: User created successfully\n");
    } else {
        printf("Test case 1 failed: Incorrect username\n");
    }

    // Test case 2: Update the user's email and verify the new email
    updateUserEmail(&user1, "john.doe@example.com");
    if (strcmp(user1.email, "john.doe@example.com") == 0) {
        printf("Test case 2 passed: Email updated successfully\n");
    } else {
        printf("Test case 2 failed: Incorrect email\n");
    }

    // Test case 3: Delete the user and verify that it is no longer valid
    deleteUser(&user1);
    if (user1.isValid == 0) {
        printf("Test case 3 passed: User deleted successfully\n");
    } else {
        printf("Test case 3 failed: User is still valid\n");
    }

    return 0;
}