#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

#define MAX_USERS 10000
#define MAX_NAME_LEN 50
#define MAX_EMAIL_LEN 50

//define user struct
typedef struct User{
    int user_id;
    char name[MAX_NAME_LEN];
    char email[MAX_EMAIL_LEN];
    struct User* friends[MAX_USERS]; //see if changing to to linked lists would help memory out later
    int friend_count;
} User;

//globals
User* users[MAX_USERS];
int user_count = 0;
int next_user_id = 1;

// Utility function to test parser
void testingParser(int arg1, char *arg2) {
    printf("The parser was called with arguments: %d and %s\n", arg1, arg2);
}

User* create_user(const char* name, const char* email); // int user_id is auto-generated to be unique
void add_friend(User* user1, User* user2); // users user1 and user2 are now friends
void delete_friend(User* user1, User* user2); // users user1 and user2 are no longer friends
void delete_user(User* user); // user is deleted
void print_users(); // prints all user names in ascending order
void change_user_name(User* user, char* new_name);
void change_user_email(User* user, char* new_email);
void print_friends(User* user); // prints user's friends in ascending order
User* search_user_by_name(const char* name);
User* search_user_by_email(const char* email);
User** mutual_friends(User* user1, User* user2); // returns an array of pointers to the mutual friends
void print_mutual_friends(User** friends); // prints mutual friends' user names in acsending order
Message* create_message(User* sender, User* receiver, const char* content); // int message_id is auto-generated to be unique
void print_message(Message* message);
void display_chat(User* user1, User* user2); // print messages in FIFO
Post* new_post(User* user, const char* content); // post id is auto-generated to be unique
void add_like(Post* post, User* user); // user is the individual who liked the post
void display_feed(User* user1);

// WRITE FUNCTIONS BELOW
int unique_name(const char* name){
    for(int i = 0; i < user_count; i++){
        if(strcmp(users[i]->name, name) == 0 ){
            return 0; // name is not unique
        }
    }
    return 1; // name is unique
}

int unique_email(const char* email){
    for(int i = 0; i < user_count; i++){
        if(strcmp(users[i]->email, email) == 0 ){
            return 0; // email is not unique
        }
    }
    return 1; // email is unique
}

User* create_user(const char* name, const char* email){
    if(user_count >= MAX_USERS){
        printf("Error! Max number of users reached.\n");
        return NULL;
    }

    if(strlen(name) >= MAX_NAME_LEN || strlen(email) >= MAX_EMAIL_LEN){
        printf("Error! Name or Email exceeds maximum length.\n");
        return NULL;
    }

    if(!unique_name(name)){
        printf("Error! Name '%s' is taken.\n", name);
        return NULL;
    }

    if(!unique_email(email)){
        printf("Error! Email '%s' is taken.\n", email);
        return NULL;
    }

    User* new_user = (User*)malloc(sizeof(User));
    if (new_user == NULL){
        printf("Memory allocation failed.\n");
        return NULL;
    }

    new_user->user_id=next_user_id++;
    strncopy(new_user->name, name, MAX_NAME_LEN - 1);
    new_user->name[MAX_NAME_LEN - 1] = '\0';
    strncopy(new_user->email, email, MAX_EMAIL_LEN - 1);
    new_user->email[MAX_EMAIL_LEN - 1] = '\0';
    new_user->friend_count = 0;
    for(int i = 0; i < MAX_USERS; i++){
        new_user->friends[i] = NULL; //make sure new users friends list is empty
    }

    users[user_count++] = new_user; //put that new user into the users array

    printf("User created: ID=%d, Name=%s, Email=%s\n", new_user->user_id, new_user->name, new_user->email);
    return new_user;
}

void add_friend(User* user1, User* user2){
    if(user1 == NULL || user2 == NULL){
        printf("One or both users do not exist\n");
        return;
    }

    if(user1==user2){
        printf("User cannot add oneself\n");
        return;
    }

    for(int i = 0; i< user1->friend_count; i++){//check if users are already firends
        if(user1->friends[i] == user2){
            printf("Users are already friends\n");
            return;
        }
    }

    if(user1->friend_count >= MAX_USERS){
        printf("User '%s' friends list is full\n", user1->name);
        return;
    }

    if(user2->friend_count >= MAX_USERS){
        printf("User '%s' friends list is full\n", user2->name);
        return;
    }

    user1->friends[user1->friend_count++] = user2;
    user2->friends[user2->friend_count++] = user1;
    printf("'%s' and '%s' are now friends\n", user1->name, user2->name);
}

void delete_friend(User* user1, User* user2){
    if(user1 == NULL || user2 == NULL){
        printf("One or both users do not exist\n");
        return;
    }

    if(user1==user2){
        printf("User cannot delete oneself\n");
        return;
    }

    //check if users are actually friends prior to deletion
    int is_friend = 0;
    for (int i = 0; i < user1->friend_count; i++) {
        if (user1->friends[i] == user2) {
            is_friend = 1;
            break;
        }
    }

    if (!is_friend) {
        printf("Error: %s and %s are not friends.\n", user1->name, user2->name);
        return;
    }


    //deletion from user1s friends list
    int index = -1;
    for(int i = 0; i< user1->friend_count; i++){
        if(user1->friends[i] == user2){
            index = i;
            break;
        }
    }

    if(index != -1){
        for (int i = index; i < user1->friend_count - 1; i++) {
            user1->friends[i] = user1->friends[i + 1];
        }
        user1->friends[user1->friend_count - 1] = NULL;
        user1->friend_count--;//clear last pointer and reduce friend count of user1
    }

    //deletion from user2s friends list
    index = -1;
    for (int i = 0; i < user2->friend_count; i++) {
        if (user2->friends[i] == user1) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        for (int i = index; i < user2->friend_count - 1; i++) {//shifts elements of array to left to close gap
            user2->friends[i] = user2->friends[i + 1];
        }
        user2->friends[user2->friend_count - 1] = NULL;
        user2->friend_count--;//clear last pointer and reduce friend count of user2
    }

    printf("'%s' and '%s' are no longer friends\n", user1->name, user2->name);
}

void delete_user(User* user){
    if(user == NULL){
        printf("User does not exist\n");
        return;
    }

    //find user in everyones friends lists and delete them from it
    for(int i = 0; i < user->friend_count; i++){
        User* friend_user = user->friends[i];
        
        int index = -1;
        for(int j = 0; j <friend_user->friend_count;j++){
            if(friend_user->friends[j] == user){
                index = j;
                break;
            }
        }

        if (index != -1) {
            for (int j = index; j < friend_user->friend_count - 1; j++) {
                friend_user->friends[j] = friend_user->friends[j + 1];
            }
            friend_user->friends[friend_user->friend_count - 1] = NULL;
            friend_user->friend_count--;
        }
    }

    

}