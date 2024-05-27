#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8082
#define BUFFER_SIZE 1024

typedef struct {
    int id;
    char title[100];
    char author[100];
    int quantity;
    // Add other book-related fields as needed
} Book;
void remove_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Server address configuration
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    printf("Enter username: ");
    char username[BUFFER_SIZE];
    fgets(username, BUFFER_SIZE, stdin);
    username[strcspn(username, "\n")] = '\0'; // Remove newline character

    // Send username to server
    if (send(sockfd, username, strlen(username), 0) < 0) {
        perror("Error sending username to server");
        exit(EXIT_FAILURE);
    }

    // Prompt user to enter password
    printf("Enter password: ");
    char password[BUFFER_SIZE];
    fgets(password, BUFFER_SIZE, stdin);
    password[strcspn(password, "\n")] = '\0'; // Remove newline character

    // Send password to server
    if (send(sockfd, password, strlen(password), 0) < 0) {
        perror("Error sending password to server");
        exit(EXIT_FAILURE);
    }

    // Receive authentication result from server
    int bytes_received = recv(sockfd, buffer, sizeof(buffer)-1, 0);
    if (bytes_received < 0) {
        perror("Error receiving authentication result from server");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';

    // Check authentication result
    if (strcmp(buffer, "AUTH_SUCCESS_ADMIN") == 0) {
        printf("Authentication successful\n");
        // Proceed with other interactions with the server
        char welcome_message[BUFFER_SIZE];
        recv(sockfd, welcome_message,sizeof(welcome_message)-1,0);
        printf("Server: %s\n", welcome_message);
        while (true) {
            // memset(buffer, 0, BUFFER_SIZE);
            char buffer[BUFFER_SIZE];

            printf("Please select an action:\n");
            printf( "1. Add a new book\n");
            printf( "2. Delete a book\n");
            printf( "3. Modify a book\n");
            printf( "4. Search for a book\n");
            printf( "5. Display all books\n");
            printf( "6. Exit\n");
            printf( "7. Add user\n");
            printf( "8. Delete user\n");
            printf( "9. Modify user\n");
            printf( "10. Display users\n");

            // Receive message from server

            // Prompt for action
            printf("Enter your choice: ");
            int choice;
            scanf("%d", &choice);
            getchar(); // Consume newline character

            switch(choice){
                case 1:
                    memset(buffer, 0, BUFFER_SIZE);

                    int id, quantity;
                    char title[100];
                    char author[100];
                    printf("Please replace all the spaces with an underscore_\n");

                    // Prompt the user to enter the book ID
                    printf("Enter the Book ID: ");
                    scanf("%d", &id);

                    // Prompt the user to enter the book title
                    printf("Enter the book title: ");
                    scanf("%s", title);

                    // Prompt the user to enter the book author
                    printf("Enter the book author: ");
                    scanf("%s", author);

                    // Prompt the user to enter the quantity
                    printf("Enter the quantity: ");
                    scanf("%d", &quantity);
                    // printf("1 %d %s, %s, %d", id, title, author, quantity);
                    // printf("%s",buffer);    

                    // Prepare the buffer with the input data
                    sprintf(buffer, "1 %d %s %s %d", id, title, author, quantity);
                    break;
                case 7:
                    memset(buffer, 0, BUFFER_SIZE);

                    char title1[100];
                    char pass1[100];
                    printf("Please replace all the spaces with an underscore_\n");

                    
                    // Prompt the user to enter the book title
                    printf("Enter the user name: ");
                    scanf("%s", title1);

                    // Prompt the user to enter the book author
                    printf("Enter the passworc: ");
                    scanf("%s", pass1);

                    
                    // printf("1 %d %s, %s, %d", id, title, author, quantity);
                    // printf("%s",buffer);    

                    // Prepare the buffer with the input data
                    sprintf(buffer, "7 %s %s", title1, pass1);
                    break;
                case 8:
                    memset(buffer, 0, BUFFER_SIZE);

                    char title2[100];
                    printf("Please replace all the spaces with an underscore_\n");

                    
                    // Prompt the user to enter the book title
                    printf("Enter the user name: ");
                    scanf("%s", title2);

                    
                    // printf("1 %d %s, %s, %d", id, title, author, quantity);
                    // printf("%s",buffer);    

                    // Prepare the buffer with the input data
                    sprintf(buffer, "8 %s", title2);
                    break;
                case 9:
                    memset(buffer, 0, BUFFER_SIZE);
                    int books3;
                    char title3[100];
                    char pass3[100];
                    printf("Please replace all the spaces with an underscore_\n");

                    
                    // Prompt the user to enter the book title
                    printf("Enter the user name: ");
                    scanf("%s", title3);

                    // Prompt the user to enter the book author
                    printf("Enter the passworc: ");
                    scanf("%s", pass3);

                    printf("Enter the new no of books borrowed ");
                    scanf("%d", &books3);

                    
                    // printf("1 %d %s, %s, %d", id, title, author, quantity);
                    // printf("%s",buffer);    

                    // Prepare the buffer with the input data
                    sprintf(buffer, "9 %s %s %d", title3, pass3, books3);
                    break;
                case 2:
                    memset(buffer, 0, BUFFER_SIZE);

                    int delete_id;
                    // Prompt the user to enter the ID of the book to delete
                    printf("Enter the ID of the book to delete: ");
                    scanf("%d", &delete_id);

                    // Prepare the buffer with the input data
                    sprintf(buffer, "2 %d", delete_id);
                    break;
                case 3:
                    memset(buffer, 0, BUFFER_SIZE);

                    int modify_id;
                    char new_title[100];
                    char new_author[100];
                    int new_quantity;

                    // Prompt the user to enter the ID of the book to modify
                    printf("Enter the ID of the book to modify: ");
                    scanf("%d", &modify_id);
                    clear_input_buffer();
                    // while (getchar() != '\n');
                    printf("Please replace all the spaces with an underscore_\n");

                    // Prompt the user to enter the new title of the book
                    printf("Enter the new title of the book: ");
                    scanf("%s", new_title);

                    // Prompt the user to enter the new author of the book
                    printf("Enter the new author of the book: ");
                    scanf("%s", new_author);

                    // printf("Enter the new title of the book: \n");
                    // fgets(new_title, sizeof(new_title), stdin);
                    // // strtok(new_title,"\n");
                    // remove_newline(new_title); // Remove the newline character

                    // // Prompt the user to enter the new author of the book
                    // printf("Enter the new author of the book: ");
                    // fgets(new_author, sizeof(new_author), stdin);
                    // // strtok(new_author,"\n");
                    // remove_newline(new_author); 

                    // Prompt the user to enter the new quantity of the book
                    printf("Enter the new quantity of the book: ");
                    scanf("%d", &new_quantity);
                    // while (getchar() != '\n');

                    // Prepare the buffer with the input data
                    sprintf(buffer, "3 %d %s %s %d", modify_id, new_title, new_author, new_quantity);
                    break;
                case 4:
                    memset(buffer, 0, BUFFER_SIZE);
                    char search_term[100];
                    int search_option=1;

                    // Prompt the user to select the search option
                    printf("Search by title\n");
                    // Prompt the user to enter the search term
                    printf("Please replace all the spaces with an underscore_\n");
                    printf("Enter the search term: ");
                    scanf("%s", search_term);

                    // Prepare the buffer with the input data
                    if (search_option == 1) {
                        // Search by title
                        sprintf(buffer, "4 %s", search_term);
                        // printf("%s",search_term);
                    }else {
                        printf("Invalid search option\n");
                        break;
                    }
                    break;
                case 5:
                    memset(buffer, 0, BUFFER_SIZE);

                    sprintf(buffer,"5");
                    break;
                case 10:
                    memset(buffer, 0, BUFFER_SIZE);

                    sprintf(buffer,"0");
                    break;
                case 6:
                    memset(buffer, 0, BUFFER_SIZE);
                    printf("Exiting the program.\n");
                    exit(0);
                    break;
                default:
                    memset(buffer, 0, BUFFER_SIZE);
                    printf("Invalid choice. Please enter a valid option.\n");
                    break;

            }
            if (send(sockfd, buffer, sizeof(buffer)-1, 0) < 0) {
            perror("Error sending data to server");
            break;
            }
            // printf("%s",buffer);
            // printf("%s",buffer);

            // // Send choice to server
            // sprintf(buffer, "%d", choice);
            // send_message(sockfd, buffer);

            // Handle exit condition
            // if (choice == 6) {
            //     receive_message(sockfd, buffer);
            //     printf("Server: %s\n", buffer);
            //     break;
            // }

            // Handle other responses from server
            // char buffer[BUFFER_SIZE];
            int bytes_rec=recv(sockfd, buffer,sizeof(buffer)-1,0);
            buffer[bytes_rec]='\0';
            printf("Server: %s\n", buffer);
        }
    } 
    else if(strcmp(buffer, "AUTH_SUCCESS") == 0){
        printf("Login successful! Welcome, user.\n");

    while (1) {
        char buffer1[BUFFER_SIZE];
        int choice;
        printf("Please choose an option:\n");
        printf("1. Get list of books\n");
        printf("2. Return a book\n");
        printf("3. Borrow a book\n"); // New option for borrowing a book
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        // char buffer[BUFFER_SIZE];
        switch (choice) {
            case 1: {
                memset(buffer1, 0, BUFFER_SIZE);

                // Request to get the list of all books
                sprintf(buffer1, "5");
                if (send(sockfd, buffer1, sizeof(buffer1)-1, 0) < 0) {
                    perror("Error sending request to server");
                }

                // Receive the list of books from the server
                int bytes_received = recv(sockfd, buffer1, sizeof(buffer1)-1, 0);
                if (bytes_received < 0) {
                    perror("Error receiving data from server");
                } else {
                    buffer1[bytes_received] = '\0';
                    printf("%s\n", buffer1);
                }
                break;
            }
            case 2: {
                // Request to return a book
                memset(buffer1, 0, BUFFER_SIZE);

                int book_id;
                printf("Enter the Book ID to return: ");
                scanf("%d", &book_id);

                // Prepare the return book request
                sprintf(buffer1, "2 %d", book_id);
                if (send(sockfd, buffer1, sizeof(buffer1)-1, 0) < 0) {
                    perror("Error sending request to server");
                }

                // Receive the response from the server
                int bytes_received = recv(sockfd, buffer1, sizeof(buffer1)-1, 0);
                if (bytes_received < 0) {
                    perror("Error receiving data from server");
                } else {
                    buffer1[bytes_received] = '\0';
                    printf("%s\n", buffer1);
                }
                break;
            }
            case 3: {
                // Request to borrow a book
                memset(buffer1, 0, BUFFER_SIZE);
                int book_id;
                printf("Enter the Book ID to borrow: ");
                scanf("%d", &book_id);

                // Prepare the borrow book request
                sprintf(buffer1, "3 %d", book_id);
                if (send(sockfd, buffer1, sizeof(buffer1)-1, 0) < 0) {
                    perror("Error sending request to server");
                }

                // Receive the response from the server
                int bytes_received = recv(sockfd, buffer1, sizeof(buffer1) - 1, 0);
                if (bytes_received < 0) {
                    perror("Error receiving data from server");
                } else {
                    buffer1[bytes_received] = '\0';
                    printf("%s\n", buffer1);
                }
                break;
            }
            case 0: {
                // Exit
                // sprintf(buffer, "0");
                // if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
                //     perror("Error sending request to server");
                // }
                memset(buffer1, 0, BUFFER_SIZE);
                    printf("Exiting the program.\n");

                close(sockfd);
                printf("Goodbye!\n");
                exit(0);
            }
            default: {
                printf("Invalid choice. Please try again.\n");
                break;
            }
        }
    }
        }
    else {
        printf("Authentication failed\n");
        exit(EXIT_FAILURE);
        // Handle authentication failure (e.g., exit or retry)
    }

    close(sockfd);

    return 0;
}
