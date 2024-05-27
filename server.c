#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_USERS 100
#define MAX_BOOKS 100
#define BUFFER_SIZE 1024
pthread_mutex_t mutex;
int book_count=10;
int user_count=3;
// Define structures for User and Book
typedef struct {
    char username[50];
    char password[50];
    int books;
    // Add other user-related fields as needed
} User;

typedef struct {
    int id;
    char title[100];
    char author[100];
    int quantity;
    // Add other book-related fields as needed
} Book;

typedef struct{
    char name[100];
    int id_borrow[100];
} Log;

struct AuthenticationResult {
    User user;
    bool authenticated;
};

// Global arrays to store users and books
User users[MAX_USERS];
Book books[MAX_BOOKS];
Log logs[BUFFER_SIZE];
pthread_mutex_t users_lock;
pthread_mutex_t books_lock;

// Function prototypes
void initialize_users();
void initialize_books();
void *handle_client(void *arg);
struct AuthenticationResult authenticate_user(int client_sockfd);
void handle_admin_request(int client_sockfd);
void handle_user_request(int client_sockfd,User user);

int main() {
    int server_sockfd, client_sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pthread_t tid;

    // Initialize users and books arrays
    initialize_users();
    initialize_books();

    // Initialize mutex locks
    pthread_mutex_init(&users_lock, NULL);
    pthread_mutex_init(&books_lock, NULL);

    // Create socket
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Server address configuration
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8082);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the server socket
    if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding to address");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sockfd, 10) < 0) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8080...\n");

    // Accept incoming connections and create threads to handle them
    while (true) {
        addr_size = sizeof(client_addr);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sockfd < 0) {
            perror("Error accepting connection");
            continue;
        }

        if (pthread_create(&tid, NULL, handle_client, (void *)&client_sockfd) != 0) {
            perror("Error creating thread");
            close(client_sockfd);
            continue;
        }
    }

    // Close server socket and destroy mutex locks
    close(server_sockfd);
    pthread_mutex_destroy(&users_lock);
    pthread_mutex_destroy(&books_lock);

    return 0;
}

// Function to initialize users array with some sample data
void initialize_users() {
    // Sample user data
    User user1 = {"admin", "admin123",0};
    User user2 = {"john_doe", "password123",0};
    User user3 = {"jane_doe", "qwerty",0};

    // Add the users to the users array
    users[0] = user1; // Assuming the first user is the admin
    users[1] = user2;
    users[2] = user3;

    // You can add more users as needed
}


// Function to initialize books array with some sample data
void initialize_books() {
    // Sample book data
    Book book1 = {1,"The_Great_Gatsby", "F._Scott_Fitzgerald", 5};
    Book book2 = {2,"To_Kill_a_Mockingbird", "Harper_Lee", 7};
    Book book3 = {3,"1984", "George_Orwell", 10};
    Book book4 = {4,"Pride_and_Prejudice", "Jane_Austen", 8};
    Book book5 = {5,"The_Catcher_in_the_Rye", "J.D._Salinger", 6};
    Book book6 = {6,"The_Hobbit", "J.R.R._Tolkien", 9};
    Book book7 = {7,"Animal_Farm", "George_Orwell", 4};
    Book book8 = {8,"Brave_New_World", "Aldous_Huxley", 3};
    Book book9 = {9,"Lord_of_the_Flies", "William_Golding", 5};
    Book book10 = {10,"The_Hunger_Games", "Suzanne_Collins", 12};

    // Add the books to the books array
    books[0] = book1;
    books[1] = book2;
    books[2] = book3;
    books[3] = book4;
    books[4] = book5;
    books[5] = book6;
    books[6] = book7;
    books[7] = book8;
    books[8] = book9;
    books[9] = book10;

    // You can add more books as needed
}


// Thread function to handle client requests
void *handle_client(void *arg) {
    int client_sockfd = *((int *)arg);
    char buffer[BUFFER_SIZE];

    // Authenticate user
    struct AuthenticationResult auth_result = authenticate_user(client_sockfd);

    // Determine if the client is an admin or regular user
    if (auth_result.authenticated) {
        if (strcmp(auth_result.user.username, "admin") == 0) {
            handle_admin_request(client_sockfd);
        } else {
            handle_user_request(client_sockfd,auth_result.user);
        }
    } else {
        // Authentication failed, handle appropriately (e.g., send error message)
        // For simplicity, let's just close the client socket in this example
        close(client_sockfd);
    }

    return NULL;
}

// Function to authenticate user
struct AuthenticationResult authenticate_user(int client_sockfd) {
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    struct AuthenticationResult auth_result;

    // Receive username from client
    int bytes_received = recv(client_sockfd, username, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
        perror("Error receiving username from client");
        auth_result.authenticated = false;
        return auth_result;
    }
    username[bytes_received] = '\0';

    // Receive password from client
    bytes_received = recv(client_sockfd, password, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
        perror("Error receiving password from client");
        auth_result.authenticated = false;
        return auth_result;
    }
    password[bytes_received] = '\0';

    // Compare received credentials with stored user data
    auth_result.authenticated = false;
    for (int i = 0; i < MAX_USERS; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            auth_result.authenticated = true;
            auth_result.user = users[i];
            break;
        }
    }
    if (auth_result.authenticated==true && strcmp(auth_result.user.username, "admin")==0) {
        strcpy(buffer, "AUTH_SUCCESS_ADMIN");
    }
    else if(auth_result.authenticated==true && strcmp(auth_result.user.username, "admin")!=0){
        strcpy(buffer, "AUTH_SUCCESS");
    } else {
        strcpy(buffer, "AUTH_FAILURE");
    }

    // Send the authentication result to the client
    if (send(client_sockfd, buffer, sizeof(buffer)-1, 0) < 0) {
        perror("Error sending authentication result to client");
        auth_result.authenticated = false;
        return auth_result;
    }
    memset(buffer, 0, BUFFER_SIZE);

    return auth_result;
}

int add_new_book(int id, char *title,  char *author, int quantity) {
    // Lock the mutex before accessing the critical section
    // printf("2 %d %s, %s, %d", id, title, author, quantity);
    pthread_mutex_lock(&mutex);

    // Find an empty slot in the inventory
    // if(id>MAX_BOOKS){
    //     return -1;
    // }
    if(id==0){
        pthread_mutex_unlock(&mutex);
        return -1;
    }
        // Add the new book to the inventory
        for(int i=0;i<book_count;i++){
            if(books[i].id==id){
                pthread_mutex_unlock(&mutex);
                return -1;
            }
        }
        books[book_count].id=id;
        books[book_count].quantity=quantity;
        strcpy(books[book_count].title,title);
        strcpy(books[book_count].author,author);
        
        book_count+=1;
    //     printf("Book added successfully:\nID: %d\nTitle: %s\nAuthor: %s\nQuantity: %d\n",
    //            id, title, author, quantity);
    // }

    // Unlock the mutex after accessing the critical section
    pthread_mutex_unlock(&mutex);
    return 0;

}
int add_user(char *user,char * pass){
    pthread_mutex_lock(&mutex);
    for(int i=0;i<user_count;i++){
            if(strcmp(user,users[i].username)==0){
                pthread_mutex_unlock(&mutex);
                return -1;
            }
    }
        strcpy(users[user_count].username,user);
        strcpy(users[user_count].password,pass);
        users[user_count].books=0;
        
        user_count+=1;
        pthread_mutex_unlock(&mutex);
    return 0;
}

int delete_user(char *username) {
    int i, j;
    for (i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            // User found, shift remaining users to the left
            for (j = i; j < user_count - 1; j++) {
                users[j] = users[j + 1];
            }
            user_count-=1;
            pthread_mutex_unlock(&mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&mutex);
    return -1;
}


int delete_book(int id) {
    // Lock the mutex before accessing the critical section
    pthread_mutex_lock(&mutex);

    // Check if the book ID is valid
    for(int i=0;i<book_count;i++){
            if(books[i].id==id){
                books[i].id=0;
            }
        }

    // Clear the book details to "delete" the book
    // memset(&books[id], 0, sizeof(Book));

    // Unlock the mutex after accessing the critical section
    pthread_mutex_unlock(&mutex);
    return 0; // Book successfully deleted
}
int modify_user(char *user,char *pass,int book_count1){
    pthread_mutex_lock(&mutex);
    for(int i=0;i<user_count;i++){
            if(strcmp(users[i].username,user)==0){
                strcpy(users[i].password,pass);
                users[i].books=book_count1;
                pthread_mutex_unlock(&mutex);
                return 0;
            }
    }
        // strcpy(users[user_count].username,user);
        // strcpy(users[user_count].password,pass);
        // users[user_count].books=book_count;
        
        // user_count+=1;
        pthread_mutex_unlock(&mutex);
    return -1;
}
int modify_book(int id, const char *title, const char *author, int quantity){
    pthread_mutex_lock(&mutex);

    // Find an empty slot in the inventory
    // if(id>MAX_BOOKS){
    //     return -1;
    // }
    if(id==0){
        pthread_mutex_unlock(&mutex);
        return -1;
    }
        // Add the new book to the inventory
        for(int i=0;i<book_count;i++){
            if(books[i].id==id){
                books[i].id=id;
                strcpy(books[i].title,title);
                strcpy(books[i].author,author);
                books[i].quantity=quantity;
                break;
            }
        }
        // books[book_count].id=id;
        // strcpy(books[book_count].title,title);
        // strcpy(books[book_count].author,author);
        // books[book_count].quantity=quantity;
        // book_count+=1;
    //     printf("Book added successfully:\nID: %d\nTitle: %s\nAuthor: %s\nQuantity: %d\n",
    //            id, title, author, quantity);
    // }

    // Unlock the mutex after accessing the critical section
    pthread_mutex_unlock(&mutex);
    return 0;
}

int my_strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}
// Function to handle admin request
void handle_admin_request(int client_sockfd) {
    bool admin_staying = true;

    // Enthusiastically welcome the administrator
    send(client_sockfd, "Welcome, esteemed administrator! How may I assist you today?",sizeof("Welcome, esteemed administrator! How may I assist you today?")-1,0);
        // Prompt the adminrator to select an action
        // send(client_sockfd, "Please select an action:");
        // send(client_sockfd, "1. Add a new book");
        // send(client_sockfd, "2. Delete a book");
        // send(client_sockfd, "3. Modify a book");
        // send(client_sockfd, "4. Search for a book");
        // send(client_sockfd, "5. Exit");
    while(admin_staying){
    char buffer1[BUFFER_SIZE];
    memset(buffer1, 0, BUFFER_SIZE);
    // printf("%s",buffer1);
    int bytes_received = recv(client_sockfd, buffer1, sizeof(buffer1)-1, 0);
    printf("%s",buffer1);
    if (bytes_received <= 0) {
        perror("Error receiving data from client");
        return;
    }
    buffer1[bytes_received] = '\0';

    char command = buffer1[0];
        // Process the administrator's choice
        switch (command) {
            case '1':
            // printf("hello");
                int id, quantity;
                char title[100];
                char author[100];

                // Extract book details from the received message
                sscanf(buffer1, "1 %d %s %s %d", &id, title, author, &quantity);
                // printf("1 %d %s, %s, %d", id, title, author, quantity);
                // printf("hello");
                // Eagerly assist in adding a new book
                int x=add_new_book(id,title,author,quantity);
                // printf("hello2");
                if(x!=0){
                    send(client_sockfd, "Failed to add book",sizeof("Failed to add book")-1,0);
                }
                else{
                    sprintf(buffer1,"%s","Book added successfully");
                    send(client_sockfd, buffer1,sizeof(buffer1)-1,0);
                }
                memset(buffer1, 0, BUFFER_SIZE);
                break;
            case '7':
                char user[100];
                char pass[100];

                // Extract book details from the received message
                sscanf(buffer1, "7 %s %s", user, pass);
                // printf("1 %d %s, %s, %d", id, title, author, quantity);
                // printf("hello");
                // Eagerly assist in adding a new book
                int x11=add_user(user,pass);
                // printf("hello2");
                if(x11!=0){
                    send(client_sockfd, "Failed to add user",sizeof("Failed to add user")-1,0);
                }
                else{
                    sprintf(buffer1,"%s","User added successfully");
                    send(client_sockfd, buffer1,sizeof(buffer1)-1,0);
                }
                memset(buffer1, 0, BUFFER_SIZE);
                break;
            case '8':
                char user1[100];
                sscanf(buffer1, "8 %s", user1);
                int x12=delete_user(user1);
                if(x12!=0){
                    send(client_sockfd, "Failed to delete user",sizeof("Failed to delete user")-1,0);
                }
                else{
                    sprintf(buffer1,"%s","User deleted successfully");
                    send(client_sockfd, buffer1,sizeof(buffer1)-1,0);
                }
                memset(buffer1, 0, BUFFER_SIZE);
                break;
            case '9':
                char user2[100];
                char pass2[100];
                int books1;

                // Extract book details from the received message
                sscanf(buffer1, "9 %s %s %d", user2, pass2, &books1);
                // printf("1 %d %s, %s, %d", id, title, author, quantity);
                // printf("hello");
                // Eagerly assist in adding a new book
                int x13=modify_user(user2,pass2,books1);
                // printf("hello2");
                if(x13!=0){
                    send(client_sockfd, "Failed to modify user",sizeof("Failed to modify user")-1,0);
                }
                else{
                    sprintf(buffer1,"%s","User modified successfully");
                    send(client_sockfd, buffer1,sizeof(buffer1)-1,0);
                }
                memset(buffer1, 0, BUFFER_SIZE);
                break;
            case '2':
                // Passionately help in deleting a book
                int id1;
                sscanf(buffer1, "2 %d", &id1);
                int x1=delete_book(id1);
                if(x1!=0){
                    send(client_sockfd, "Failed to delete book",sizeof( "Failed to delete book")-1,0);
                }
                else{
                    send(client_sockfd, "Book deleted successfully",sizeof("Book deleted successfully")-1,0);
                }
                memset(buffer1, 0, BUFFER_SIZE);
                break;
            case '3':
                int id2, quantity1;
                char title1[100];
                char author1[100];

                // Extract book details from the received message
                sscanf(buffer1, "3 %d %s %s %d", &id2, title1, author1, &quantity1);
                // Enthusiastically guide in modifying a book
                int x2=modify_book(id2,title1,author1,quantity1);
                if(x2!=0){
                    send(client_sockfd, "Failed to modify book",sizeof("Failed to modify book")-1,0);
                }
                else{
                    send(client_sockfd, "Book modified successfully",sizeof("Book modified successfully")-1,0);
                }
                memset(buffer1, 0, BUFFER_SIZE);
                break;
            case '4':
                char search_term[100];
                sscanf(buffer1, "4 %s", search_term);

                // Lock the mutex before accessing the critical section
                pthread_mutex_lock(&mutex);
                memset(buffer1, 0, BUFFER_SIZE);
                

                // Search for the book by title
                // char response[BUFFER_SIZE] = "Search results:\n";
                int found = 0;
                for (int i = 0; i < book_count; i++) {
                    printf("%s",books[i].title);
                    printf("%s",search_term);
                    char search[100];
                    strcpy(search,books[i].title);
                    if (my_strcmp(search, search_term)==0 && books[i].id!=0) {
                        char book_details[400];
                        sprintf(book_details, "ID: %d, Title: %s, Author: %s, Quantity: %d\n",
                                books[i].id, books[i].title, books[i].author, books[i].quantity);
                        strcat(buffer1, book_details);
                        found = 1;
                    }
                }

                // Unlock the mutex after accessing the critical section
                

                if (!found) {
                    strcpy(buffer1, "No books found with the given search term.\n");
                }

                send(client_sockfd, buffer1,sizeof(buffer1)-1,0);
                pthread_mutex_unlock(&mutex);
                memset(buffer1, 0, BUFFER_SIZE);
                break;
            case '0':
                pthread_mutex_lock(&mutex);
                memset(buffer1, 0, BUFFER_SIZE);
                // Prepare a list of all books
                // char response1[BUFFER_SIZE] = "List of all books:\n";
                // sprintf(buffer,"%s","List of all books:\n");
                for (int i = 0; i < user_count; i++) {
                        if(strcmp(users[i].username,"admin")!=0){
                            char book_details[400];
                                sprintf(book_details, "User: %s, Password: %s, Books_Borrowed: %d\n",
                                users[i].username, users[i].password, users[i].books);
                                // printf("ID: %d, Title: %s, Author: %s, Quantity: %d\n",
                                // books[i].id, books[i].title, books[i].author, books[i].quantity);
                                strcat(buffer1, book_details);
                        }
                        
                    
                }
                // sprintf(buffer,"%s",response1);
                // Unlock the mutex after accessing the critical section
                

                send(client_sockfd, buffer1,sizeof(buffer1)-1,0);
                pthread_mutex_unlock(&mutex);
                memset(buffer1, 0, BUFFER_SIZE);
                break;
            case '5':
                // Lock the mutex before accessing the critical section
                pthread_mutex_lock(&mutex);

                // Prepare a list of all books
                // char response1[BUFFER_SIZE] = "List of all books:\n";
                // sprintf(buffer,"%s","List of all books:\n");
                for (int i = 0; i < book_count; i++) {
                        if(books[i].id!=0){
                            char book_details[400];
                                sprintf(book_details, "ID: %d, Title: %s, Author: %s, Quantity: %d\n",
                                books[i].id, books[i].title, books[i].author, books[i].quantity);
                                // printf("ID: %d, Title: %s, Author: %s, Quantity: %d\n",
                                // books[i].id, books[i].title, books[i].author, books[i].quantity);
                                strcat(buffer1, book_details);
                        }
                        
                    
                }
                // sprintf(buffer,"%s",response1);
                // Unlock the mutex after accessing the critical section
                

                send(client_sockfd, buffer1,sizeof(buffer1)-1,0);
                pthread_mutex_unlock(&mutex);
                memset(buffer1, 0, BUFFER_SIZE);
                break;
            case '6':
                // Bid farewell to the administrator and exit the loop
                send(client_sockfd, "Thank you for your administration! Goodbye!",sizeof("Thank you for your administration! Goodbye!")-1,0);
                memset(buffer1, 0, BUFFER_SIZE);
                // admin_staying = false;
                break;
            default:
                // Express disappointment for invalid choice
                send(client_sockfd, "Invalid choice! Please try again.",sizeof("Invalid choice! Please try again.")-1,0);
                memset(buffer1, 0, BUFFER_SIZE);
                break;
        }
        memset(buffer1, 0, BUFFER_SIZE);

    }
}


// Function to handle user request
void handle_user_request(int client_sockfd, User user) {
    char buffer3[BUFFER_SIZE];
    while (1) {
        // Receive the request from the client
        int bytes_received = recv(client_sockfd, buffer3, sizeof(buffer3)-1, 0);
        if (bytes_received <= 0) {
            perror("Error receiving data from client");
            close(client_sockfd);
            return;
        }
        buffer3[bytes_received] = '\0';

        // Determine the request type
        switch (buffer3[0]) {
            case '5': {
                // Handle request to get the list of all books
                pthread_mutex_lock(&mutex);
                strcpy(buffer3, "List of available books:\n");
                for (int i = 0; i < book_count; i++) {
                    if (books[i].id != 0) {
                        char book_info[BUFFER_SIZE];
                        sprintf(book_info, "ID: %d, Title: %s, Author: %s, Quantity: %d\n",
                                books[i].id, books[i].title, books[i].author, books[i].quantity);
                        strcat(buffer3, book_info);
                    }
                }
                pthread_mutex_unlock(&mutex);
                
                if (send(client_sockfd, buffer3, sizeof(buffer3)-1, 0) < 0) {
                    perror("Error sending book list to client");
                }
                memset(buffer3, 0, BUFFER_SIZE);
                break;
            }
            case '2': {
                // Handle request to return a book
                int book_id;
                sscanf(buffer3, "2 %d", &book_id);
                memset(buffer3, 0, BUFFER_SIZE);
                pthread_mutex_lock(&mutex);

                if (book_id < 0 || book_id >= MAX_BOOKS) {
                    // Invalid book ID or book does not exist
                    // send(client_sockfd, "Error: Invalid book ID",size);
                    sprintf(buffer3,"%s","Error: Invalid book ID");
                    send(client_sockfd, buffer3,sizeof(buffer3)-1,0);
                } else {
                    // Return the book (increase the quantity)
                        for(int i=0;i<book_count;i++){
                            if(books[i].id==book_id && books[i].id!=0){
                                // books[i].id=id;
                                // strcpy(books[i].title,title);
                                // strcpy(books[i].author,author);
                                books[i].quantity+=1;
                                for(int i=0;i<user_count;i++){
                                if(strcmp(user.username,users[i].username)==0){
                                    users[i].books-=1;
                                }
                                }
                                
                                break;
                            }
                        }
                    // books[book_id].quantity += 1;
                    // send_message(client_sockfd, "Book returned successfully");
                    sprintf(buffer3,"%s","Book returned successfully");
                    send(client_sockfd, buffer3,sizeof(buffer3)-1,0);
                }

                pthread_mutex_unlock(&mutex);
                memset(buffer3, 0, BUFFER_SIZE);
                break;
            }
            case '3': {
                // Handle request to borrow a book
                int book_id;
                int found=0;
                sscanf(buffer3, "3 %d", &book_id);
                memset(buffer3, 0, BUFFER_SIZE);
                pthread_mutex_lock(&mutex);

                if (book_id < 0 || book_id >= MAX_BOOKS || books[book_id].id == 0) {
                    // Invalid book ID or book does not exist
                    // send_message(client_sockfd, "Error: Invalid book ID");
                    sprintf(buffer3,"%s","Error: Invalid book ID");
                    send(client_sockfd, buffer3,sizeof(buffer3)-1,0);
                    
                } 
                for(int i=0;i<book_count;i++){
                            if(books[i].id==book_id){
                                found=1;
                                if(books[i].quantity<=0){
                                    sprintf(buffer3,"%s","Error: Book not available");
                                    send(client_sockfd, buffer3,sizeof(buffer3)-1,0);
                                    memset(buffer3, 0, BUFFER_SIZE);
                                    break;
                                }
                                // books[i].id=id;
                                // strcpy(books[i].title,title);
                                // strcpy(books[i].author,author);
                                books[i].quantity -= 1;
                                for(int i=0;i<user_count;i++){
                                if(strcmp(user.username,users[i].username)==0){
                                    users[i].books+=1;
                                }
                                }
                                sprintf(buffer3,"%s","Book borrowed successfully");
                                send(client_sockfd, buffer3,sizeof(buffer3)-1,0);
                                memset(buffer3, 0, BUFFER_SIZE);
                                break;
                            }
                }
                
                
                // else if (books[book_id].quantity <= 0) {
                //     // Book not available
                //     // send_message(client_sockfd, "Error: Book not available");
                //     sprintf(buffer3,"%s","Error: Book not available");
                //     send(client_sockfd, buffer3,sizeof(buffer3)-1,0);
                // } else {
                //     // Borrow the book (decrease the quantity)
                //     books[book_id].quantity -= 1;
                //     // send_message(client_sockfd, "Book borrowed successfully");
                //     sprintf(buffer3,"%s","Book borrowed successfully");
                //     send(client_sockfd, buffer3,sizeof(buffer3)-1,0);
                // }

                if(found==0){
                    sprintf(buffer3,"%s","Error: Book doesnt exist");
                    send(client_sockfd, buffer3,sizeof(buffer3)-1,0);
                }

                pthread_mutex_unlock(&mutex);
                memset(buffer3, 0, BUFFER_SIZE);
                break;
            }
            case '0': {
                // Handle exit request
                close(client_sockfd);
                memset(buffer3, 0, BUFFER_SIZE);
                return;
            }
            default: {
                // Handle invalid request
                // send_message(client_sockfd, "Error: Invalid request");
                sprintf(buffer3,"%s","Error: Invalid request");
                send(client_sockfd, buffer3,sizeof(buffer3)-1,0);
                memset(buffer3, 0, BUFFER_SIZE);
                break;
            }
        }
    }
}




