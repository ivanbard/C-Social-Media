// functions.h
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define MAX_USERS 10000
#define MAX_NAME_LEN 50
#define MAX_EMAIL_LEN 50
#define HASH_TABLE_SIZE 10007
#define MAX_CHATS 100000 
#define MAX_MESSAGES 50
#define MAX_MES_LEN 256
#define MAX_POST_LEN 256
//post and user structs had to be predefined to fix an error for the two structs depending on one another due to the way i coded their functions
typedef struct Post Post; //forward declaration for error
typedef struct User User; //forward declaration for error
struct User{
    int user_id;
    char name[MAX_NAME_LEN];
    char email[MAX_EMAIL_LEN];
    struct User* friends[MAX_USERS]; //see if changing to to linked lists would help memory out later
    int friend_count;
    //stuff for posts for each user
    Post** posts; //array of pointers to posts
    int post_count; 
    int post_capacity;
};

struct Post{
    int post_id;
    User* creator;
    int like_count;
    char content[MAX_POST_LEN];
    //stuff for likes
    int like_capacity;
    int* likes;
};

typedef struct {
    int message_id;
    User* sender;
    User* receiver;
    char content[MAX_MES_LEN];
} Message;

typedef struct { //manage the actual chat between two users
    User* user1;
    User* user2;
    Message* messages[MAX_MESSAGES];
    int message_count;
    int start; //head and tail of circular buffer
    int end;
} Chat;

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

#endif
