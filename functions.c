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
    User* new_user=(User*)malloc(sizeof(User));
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

    //for postst
    new_user->post_capacity = 10;
    new_user->post_count = 0;
    new_user->posts = (Post**)malloc(new_user->post_capacity * sizeof(Post*));
    if(new_user->posts == NULL){
        printf("Memory allocation failed for user's posts.\n");
        free(new_user);
        return NULL;
    }

    insert(nameTable, new_user, name); //store new users details in the hash tables
    insert(emailTable, new_user, email); 
    users[user_count++] = new_user; //add the new user to the global array and add new spot in array for them

    //printf("User created: id=%d, name=%s, email=%s\n", new_user->user_id, new_user->name, new_user->email);
    return new_user;
}

void add_friend(User* user1, User* user2){
    user1->friends[user1->friend_count++] = user2;
    user2->friends[user2->friend_count++] = user1;
    //printf("'%s' and '%s' are  friends\n", user1->name, user2->name);
}

void delete_friend(User* user1, User* user2){
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
            user1->friends[i] =user1->friends[i + 1];
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
            user2->friends[i] =user2->friends[i + 1];
        }
        user2->friends[user2->friend_count - 1] = NULL;
        user2->friend_count--;//clear last pointer and reduce friend count of user2
    }

    //printf("'%s' and '%s' are no longer friends\n", user1->name, user2->name);
}

void delete_user(User* user){
    //find user in everyones friends lists and delete them from it
    for(int i = 0; i < user->friend_count; i++){
        User* friend_user = user->friends[i];
        
        int index = -1;
        for(int j = 0; j <friend_user->friend_count;j++){
            if(friend_user-> friends[j] == user){
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
            users[i] =users[i+1];
        }
        users[user_count - 1] = NULL;
        user_count--;
    }

    //printf("'%s' is no longer a user\n", user->name);
    free(user); //free pointer taken up by deleted user
}

//helper for quick sort 
int compareUsers(const void* a, const void* b){
    User* userA = *(User**)a;
    User* userB = *(User**)b;
    return strcmp(userA->name, userB->name);
}

void print_users(){
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
    unsigned int oldNameIndex = hashFunc(user->name);
    Node* current = nameTable[oldNameIndex];
    Node* prev = NULL;

    while(current){
        if(current->user == user){
            if(prev){
                prev->next = current->next;
            }
            else{
                nameTable[oldNameIndex]= current->next;
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

    //printf("User changed their name to '%s'\n", user->name);
}

void change_user_email(User* user, char* new_email){
    unsigned int oldEmailIndex = hashFunc(user->email);
    Node* current = emailTable[oldEmailIndex];
    Node* prev = NULL;    

    while(current){
        if(current->user == user){
            if(prev){
                prev->next = current->next;
            }
            else{
                emailTable[oldEmailIndex]= current->next;
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

    //printf("User changed their email to '%s'\n", user->email);
}

void print_friends(User* user){
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
        if(i<user->friend_count-1){
            printf(",");
        }
    }
    printf("\n");
    free(sorted);
}

User** mutual_friends(User* user1, User* user2){
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
            if(user2->friends[j] ==friend1){ //check if x person is also on user 2 friends list
                mutual[mutual_count++] = friend1; //if they are, add them to the mutual friends array
                break;
            }
        }
    }

    mutual[mutual_count] = NULL;
    return mutual;
}

void print_mutual_friends(User** friends){
    int count=0;
    while(friends[count] != NULL){
        count++;
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
    unsigned int index = chatHashFunc(user1->user_id, user2->user_id);
    chatNode* current = chatTable[index];
    while(current){ //scroll through cursedly long linked list to find chat between those users
        if((current->chat->user1 == user1 && current->chat->user2 == user2) || (current->chat->user1 == user2 && current->chat->user2 == user1)){
            return current->chat; //most definitely not ideal in terms of time complexity
        }
        current = current->next;
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
    int are_friends=0;
    for(int i = 0; i < sender->friend_count; i++){ //verify friendship first
        if(sender->friends[i]  == receiver){
            are_friends = 1;
            break;
        }
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
    printf("[%s:]",message->sender->name);
    printf("%s",message->content);
}

void display_chat(User* user1, User* user2){
    Chat* chat = findChat(user1, user2);
    if(chat == NULL){
        printf("Chat does not exist\n");
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
    int new = user->post_capacity*2; //just double it if needed
    Post** temp = (Post**)realloc(user->posts, new*sizeof(Post*));
    if(temp==NULL){
        printf("Memory allocation failedx\n");
        return false;   
    }
    user->posts =temp;
    user->post_capacity = new;
    return true;
}

Post* new_post(User* user, const char* content){
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
    //stuff for initializing the likes array when a post is created
    new_post->like_capacity = 10; //what is the limit on likes given?? if its small can i just leave it to not be resized, 10 for now
    new_post->like_count = 0;
    new_post->likes = (int*)malloc(new_post->like_capacity*sizeof(int));
    if(new_post->likes == NULL){
        printf("Memory allocation failed\n");
        free(new_post);
        return NULL;
    }

    user->posts[user->post_count++] = new_post; //add post to users posts array after necessary array resizing+increment user post count
    return new_post;
}

bool resizeLikes(Post* post){ //see if this will still be needed if like count is limited
    int new = post->like_capacity*2;
    int* temp = realloc(post->likes, new*sizeof(int));
    if(temp == NULL){
        printf("Memory allocation failed\n");
        return false;
    }
    post->likes = temp;
    post->like_capacity = new;
    return true;
}

void add_like(Post* post, User* user){
    int user_id = user->user_id;

    bool has_liked = false;
    for(int i = 0; i <post->like_count; i++){
        if(post->likes[i]==user_id){
            has_liked = true;
            break; //break if x user has already liked the post
        }
    }
    if(has_liked){
        printf("User has already liked this post\n");
        return;
    }

    if(post->like_count >= post->like_capacity){
        if(!resizeLikes(post)){
            return; //failed to resize of not needed, just continue
        }
    }
    post->likes[post->like_count++] = user_id;
}

int compare_posts(const void* a, const void* b){
    Post* A=*(Post**)a;
    Post* B=*(Post**)b;

    if(A->like_count < B->like_count){ //compare by most likes first
        return 1;
    } else if(A->like_count > B->like_count){
        return -1;
    } else{
        if(A->post_id < B->post_id){ //compare by post id(older post->higher id) second
            return -1;
        } else if(A->post_id > B->post_id){
            return 1;
        } else{
            return 0;
        }
    }
}

void add_post(Post*** feed, int* count, int* capacity, Post* post) {
    if (*count >= *capacity) {
        *capacity *= 2;
        Post** temp = (Post**)realloc(*feed, *capacity * sizeof(Post*));
        if (temp == NULL) {
            printf("Memory allocation failed\n");
            return;
        }
        *feed =temp;
    }
    (*feed)[(*count)++] = post;
}

void display_feed(User* user1){
    int capacity = 100; //what is the limit on posts to be obtained, if only 20 to be displayed? 100 for now
    int count = 0;
    Post** feed = (Post**)malloc(capacity*sizeof(Post*));
    if(feed == NULL){
        printf("Memory allocation failed\n");
        return;
    }

    for(int i = 0; i < user1->post_count; i++){
        add_post(&feed, &count, &capacity, user1->posts[i]); //users posts
    }
    for(int i=0; i < user1->friend_count; i++){
        User* friend_user = user1->friends[i];
        if(friend_user != NULL){
            for(int j = 0; j < friend_user->post_count; j++){
                add_post(&feed, &count, &capacity, friend_user->posts[j]); //users firedns posts
            }
        }
    }

    qsort(feed, count, sizeof(Post*), compare_posts);
    int feed_size = (count < 20) ? count : 20; //hard limit to 20

    for(int i =0; i<feed_size; i++){
        Post* current_post = feed[i];
        printf("[%s:]",current_post->creator->name);
        printf("%s", current_post->content);
        if(i<feed_size-1){
            printf(",");
        }
    }
    printf("\n");
    free(feed);
}