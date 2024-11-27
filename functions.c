#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include <stdbool.h>

#define MAX_USERS 10000
#define MAX_NAME_LEN 50
#define MAX_EMAIL_LEN 50
#define HASH_TABLE_SIZE 10007

//create node struct for hashing and separate chaining
typedef struct Node{
    User* user;
    struct Node* next;
}Node;

typedef struct chatNode{ //fucking more nodes but now for chat linked list
    Chat* chat;
    struct chatNode* next;
}chatNode;

//create hash tables for storing info
Node* nameTable[HASH_TABLE_SIZE];
Node* emailTable[HASH_TABLE_SIZE];
chatNode* chatTable[HASH_TABLE_SIZE];

//array for ordered traversal in prints
User* users[MAX_USERS];
int user_count = 0;
int next_user_id = 1;
Chat* chats[MAX_CHATS];
int chat_count = 0;
int next_message_id = 1;
int next_post_id = 1;

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
unsigned int hashFunc(const char* str){
    unsigned int hash = 0;
    while (*str){
        hash = (hash * 31) + (*str++); //prime number to reduce collisions
    }
    return hash % HASH_TABLE_SIZE;
}

unsigned int chatHashFunc(int user1_id, int user2_id){
    if(user1_id > user2_id){ //this is to make sure that no matter if user1 or user2 is first the hash will be the same
        int temp = user1_id;
        user1_id = user2_id;
        user2_id = temp;
    }

    unsigned long hash = (unsigned long)user1_id * 31 + user2_id;
    return hash % HASH_TABLE_SIZE;
}

void insert(Node** hashTable, User* user, const char* key){
    unsigned int index = hashFunc(key);
    Node* new_node = (Node*)malloc(sizeof(Node));
    if(!new_node){
        printf("Memory allocation failed\n");
        exit(1);
    }
    new_node->user = user;
    new_node->next = hashTable[index];
    hashTable[index] = new_node;
}

//implement search by user and email to simplify user creation using hastables
User* search_user_by_name(const char* name){
    unsigned int index = hashFunc(name);
    Node* current = nameTable[index];
    while(current){
        if(strcmp(current->user->name, name) == 0){
            return current->user;
        }
        current  = current->next;
    }
    return NULL;
}

User* search_user_by_email(const char* email){
    unsigned int index = hashFunc(email);
    Node* current = emailTable[index];
    while(current){
        if(strcmp(current->user->email, email) == 0){
            return current->user;
        }
        current  = current->next;
    }
    return NULL; //if not found
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

    if(search_user_by_name(name)){
        printf("Error! Name '%s' is taken.\n", name);
        return NULL;
    }

    if(search_user_by_email(email)){
        printf("Error! Email '%s' is taken.\n", email);
        return NULL;
    }

    User* new_user = (User*)malloc(sizeof(User));
    if (new_user == NULL){
        printf("Memory allocation failed.\n");
        return NULL;
    }

    new_user->user_id=next_user_id++;
    strncpy(new_user->name, name, MAX_NAME_LEN - 1);
    new_user->name[MAX_NAME_LEN - 1] = '\0';
    strncpy(new_user->email, email, MAX_EMAIL_LEN - 1);
    new_user->email[MAX_EMAIL_LEN - 1] = '\0';
    new_user->friend_count = 0;
    for(int i = 0; i < MAX_USERS; i++){
        new_user->friends[i] = NULL; //make sure new users friends list is empty
    }

    insert(nameTable, new_user, name); //store new users details in the hash tables
    insert(emailTable, new_user, email); 
    users[user_count++] = new_user; //add the new user to the global array and add new spot in array for them

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

    //remove user from hash tables
    //for name table
    unsigned int name_index = hashFunc(user->name);
    Node* current = nameTable[name_index];
    Node* prev = NULL;
    while(current){
        if(current->user == user){
            if(prev){
                prev->next = current->next;
            }
            else{
                nameTable[name_index] = current->next;
            }
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }
    
    //for email table
    unsigned int email_index = hashFunc(user->email);
    current = emailTable[email_index];
    prev = NULL;
    while(current){
        if(current->user == user){
            if(prev){
                prev->next = current->next;
            }
            else{
                emailTable[email_index] = current->next;
            }
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }

    //remove user from the global array
    int index = -1;
    for(int i = 0; i < user_count; i++){
        if(users[i] == user){
            index = i;
            break;
        }
    }

    if(index != -1){
        for(int i = index; i < user_count - 1; i++){
            users[i] = users[i+1];
        }
        users[user_count - 1] = NULL;
        user_count--;
    }

    printf("'%s' is no longer a user\n", user->name);
    free(user); //free pointer taken up by deleted user
}

//helper for quick sort 
int compareUsers(const void* a, const void* b){
    User* userA = *(User**)a;
    User* userB = *(User**)b;
    return strcmp(userA->name, userB->name);
}

void print_users(){
    if(user_count == 0){
        printf("No users exist\n");
        return;
    }

    User** sorted = (User**)malloc(user_count * sizeof(User*));
    if(sorted == NULL){
        printf("Memory allocation failed\n");
        return;
    }
    memcpy(sorted, users, user_count * sizeof(User*));

    //alphabetically sort the copied array for printing
    qsort(sorted, user_count, sizeof(User*), compareUsers);
    for(int i = 0; i < user_count; i++){
        printf("%s", sorted[i]->name);
        if(i<user_count - 1){
            printf(",");
        }
    }
    printf("\n");
    free(sorted); //empty sorted array pointer for memory optimization
}

void change_user_name(User* user, char* new_name){
    if(user == NULL || new_name == NULL){
        printf("Error! User or new name do not exist\n");
        return;
    }

    if(strlen(new_name) >= MAX_NAME_LEN){
        printf("Error! New name exceeds length limit\n");
        return;
    }

    if(search_user_by_name(new_name)){
        printf("Error! '%s' is already taken\n", new_name);
        return;
    }

    unsigned int oldNameIndex = hashFunc(user->name);
    Node* current = nameTable[oldNameIndex];
    Node* prev = NULL;

    while(current){
        if(current->user == user){
            if(prev){
                prev->next = current->next;
            }
            else{
                nameTable[oldNameIndex] = current->next;
            }
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }

    strncpy(user->name, new_name, MAX_NAME_LEN-1);
    user->name[MAX_NAME_LEN - 1] = '\0';
    insert(nameTable, user, new_name);

    printf("User changed their name to '%s'\n", user->name);
}

void change_user_email(User* user, char* new_email){
    if(user == NULL || new_email == NULL){
        printf("Error! User or new email do not exist\n");
        return;
    }

    if(strlen(new_email) >= MAX_EMAIL_LEN){
        printf("Error! New email exceeds length limit\n");
        return;
    }

    if(search_user_by_email(new_email)){
        printf("Error! '%s' is already taken\n", new_email);
        return;
    }

    unsigned int oldEmailIndex = hashFunc(user->email);
    Node* current = emailTable[oldEmailIndex];
    Node* prev = NULL;    

    while(current){
        if(current->user == user){
            if(prev){
                prev->next = current->next;
            }
            else{
                emailTable[oldEmailIndex] = current->next;
            }
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }

    strncpy(user->email, new_email, MAX_EMAIL_LEN-1);
    user->email[MAX_EMAIL_LEN - 1] = '\0';
    insert(emailTable, user, new_email);

    printf("User changed their email to '%s'\n", user->email);
}

void print_friends(User* user){
    if(user == NULL){
        printf("Error! User does not exist\n");
        return;
    }

    if(user->friend_count == 0){
        printf("'%s' has no friends", user->name);
        return;
    }

    User** sorted = (User**)malloc(user->friend_count * sizeof(User*));
    if(sorted == NULL){
        printf("Memory allocation failed\n");
        return;
    }
    memcpy(sorted, user->friends, user->friend_count * sizeof(User*));

    //alphabetically sort the copied array for printing
    qsort(sorted, user->friend_count, sizeof(User*), compareUsers);
    for(int i = 0; i < user->friend_count; i++){
        printf("%s", sorted[i]->name);
        if(i<user->friend_count - 1){
            printf(",");
        }
    }
    printf("\n");
    free(sorted); //empty sorted array pointer for memory optimization
}

User** mutual_friends(User* user1, User* user2){
    if(user1 == NULL || user2 == NULL){
        printf("One or both users do not exist\n");
        return NULL;
    }

    //find max amount of mutual friends to allocate array
    int maxFriends = (user1->friend_count < user2->friend_count) ? user1->friend_count : user2->friend_count;
    User** mutual = (User**)malloc((maxFriends + 1) * sizeof(User*));
    if (mutual == NULL){
        printf("Memory allocation failed\n");
        return NULL;
    }

    int mutual_count = 0;
    for(int i = 0; i < user1->friend_count; i++){
        User* friend1 = user1->friends[i]; //take x person on user 1 friends list
        for(int j = 0; j < user2->friend_count; j++){
            if(user2->friends[j] == friend1){ //check if x person is also on user 2 friends list
                mutual[mutual_count++] = friend1; //if they are, add them to the mutual friends array
                break;
            }
        }
    }

    mutual[mutual_count] = NULL;
    return mutual;
}

void print_mutual_friends(User** friends){
    if(friends == NULL){
        printf("No mutual friends\n");
        return;
    }

    int count = 0;
    while(friends[count] != NULL){
        count++;
    }

    if(count == 0){
        printf("No mutual friends\n");
        return;
    }

    User** sorted = (User**)malloc(count * sizeof(User*));
    if (sorted==NULL){
        printf("Memory allocation failed\n");
        return;
    }

    memcpy(sorted, friends, count*sizeof(User*));
    qsort(sorted, count, sizeof(User*), compareUsers);
    for(int i = 0; i < count; i++){
        printf("%s", sorted[i]->name);
        if(i < count-1){
            printf(", ");
        }
    }
    printf("\n");
    free(sorted);
}

Chat* findChat(User* user1, User* user2){ //helper function to find chat between x and y users
    if(user1 == NULL || user2 == NULL){
        printf("One or both users do not exist\n");
        return NULL;
    }
    unsigned int index = chatHashFunc(user1->user_id, user2->user_id);
    chatNode* current = chatTable[index];
    while(current){ //scroll through cursedly long linked list to find chat between those users
        if((current->chat->user1 == user1 && current->chat->user2 == user2) || (current->chat->user1 == user2 && current->chat->user2 == user1)){
            return current->chat; //most definitely not ideal in terms of time complexity
        }
        current = current->next;
    }

    if(chat_count > MAX_CHATS){
        printf("Max number of chats reached\n");
        return NULL;
    }

    Chat* new_chat = (Chat*)malloc(sizeof(Chat));
    if(new_chat == NULL){
        printf("Memory allocation failed\n");
        return NULL;
    }

    new_chat->user1 = user1;
    new_chat->user2 = user2;
    new_chat->message_count = 0;
    new_chat->start = 0;
    new_chat->end = 0;
    for(int i = 0; i < MAX_MESSAGES; i++){
        new_chat->messages[i] = NULL;
    }

    chatNode* new_node = (chatNode*)malloc(sizeof(chatNode));
    if(new_node == NULL){
        printf("Memory allocation failed\n");
        return NULL;
    }
    new_node->chat = new_chat;
    new_node->next = chatTable[index]; //adding chat to hash table
    chatTable[index] = new_node;
    chats[chat_count++] = new_chat; //adding chat to global arry
    return new_chat;
}

void add_message(Chat* chat, Message* message){ //helper function to add messages to the circular buffer
    if(chat ==NULL||message == NULL){
        printf("Chat or message does not exist\n");
        return;
    }
    if(chat->message_count < MAX_MESSAGES){ //if sub-50 messages in chat, simply add to chat
        chat->messages[chat->end]=message;
        chat->end = (chat->end +1) % MAX_MESSAGES;
        chat->message_count++;
    }
    else{
        free(chat->messages[chat->start]); //delete oldest message to make space for new
        chat->messages[chat->end]=message;
        chat->end = (chat->end+1) % MAX_MESSAGES;
        chat->start = (chat->start+1) % MAX_MESSAGES;
    }
}

Message* create_message(User* sender, User* receiver, const char* content){
    if(sender==NULL || receiver==NULL || content==NULL){
        printf("Sender/receiver/content does not exist\n");
        return NULL;
    }

    if(sender == receiver){
        printf("Sender and receiver cannto be the same\n");
        return NULL;
    }

    int are_friends = 0;
    for(int i = 0; i < sender->friend_count; i++){ //verify friendship first
        if(sender->friends[i] == receiver){
            are_friends = 1;
            break;
        }
    }
    if(strlen(content) >= MAX_MES_LEN){
        printf("Message exceeds maximum length\n");
        return NULL;
    }
    
    Message* new_message = (Message*)malloc(sizeof(Message));
    if(new_message == NULL){
        printf("Memory allocation failed\n");
        return NULL;
    }
    new_message->message_id = next_message_id++;
    new_message->sender = sender;
    new_message->receiver = receiver;
    strncpy(new_message->content, content, MAX_MES_LEN-1);
    new_message->content[MAX_MES_LEN-1] = '\0';
    Chat* chat = findChat(sender, receiver);
    if(chat == NULL){
        free(new_message);
        return NULL;
    }

    add_message(chat, new_message); //add prints for debugging?
    return new_message;
}

void print_message(Message* message){
    if(message ==NULL){
        printf("Message does not exist\n");
        return;
    }
    printf("[%s:]", message->sender->name);
    printf("%s", message->content);
}

void display_chat(User* user1, User* user2){
    if(user1 == NULL || user2 == NULL){
        printf("One or both users do not exist\n");
        return;
    }
    Chat* chat = findChat(user1, user2);
    if(chat == NULL){
        printf("Chat does not exist\n");
        return;
    }

    if(chat->message_count == 0){
        printf("No messages in chat\n");
        return;
    }

    int index = chat->start;
    for(int i=0; i<chat->message_count; i++){
        Message* msg = chat->messages[index];
        print_message(msg);
        if(i<chat->message_count-1){
            printf(",");
        }
        index = (index + 1) % MAX_MESSAGES;
    }
    printf("\n");
}

//since max posts per user isnt specified, have to make dynamic arrayt resizable if needed(helper if needed, cut out if post max is specified)
bool resizePosts(User* user){
    if (user==NULL){
        printf("User does not exist\n");
        return false;
    }
    int new = user->post_capacity*2; //just double it if needed
    Post** temp = (Post**)realloc(user->posts, new*sizeof(Post*));
    if(temp==NULL){
        printf("Memory allocation failed\n");
        return false;   
    }
    user->posts = temp;
    user->post_capacity = new;
    return true;
}

Post* new_post(User* user, const char* content){
    if(user ==NULL){
        printf("User does not exist\n");
        return NULL;
    }
    if(content==NULL){
        printf("Content does not exist\n");
        return NULL;
    }
    if(strlen(content) >= MAX_POST_LEN){
        printf("Post exceeds maximum length\n");
        return NULL;
    }

    Post* new_post = (Post*)malloc(sizeof(Post));
    if(new_post == NULL){
        printf("Memory allocation failed\n");
        return NULL;
    }
    new_post->post_id = next_post_id++;
    new_post->creator = user;
    new_post->like_count = 0;
    strncpy(new_post->content, content, MAX_POST_LEN-1);
    new_post->content[MAX_POST_LEN-1] = '\0';

    if(user->post_count >= user->post_capacity){
        if(!resizePosts(user)){
            free(new_post);
            return NULL;
        }
    }
    user->posts[user->post_count++] = new_post; //add post to users posts array after necessary array resizing+increment user post count
    return new_post;
}