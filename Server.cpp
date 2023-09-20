#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <vector>

constexpr int PORT = 12345;
constexpr int MAX_CONNECTIONS = 5;

std::vector<int> client_sockets;

void handle_client(int client_socket) {
    char buffer[1024];
    while (true) 
    {
        std::cout << " Sucessfully connected to client " << client_socket << "\n" ;

        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            close(client_socket);
            return;
        }
        std::cout << "Received: " << buffer << std::endl;
        std::cout << "Enter message (or 'q' to quit): ";
        std::cin.getline(buffer, sizeof(buffer));

        if (strcmp(buffer, "q") == 0) {
            break;
        }
        send(client_socket , buffer, bytes_received, 0);

    }
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_CONNECTIONS) == -1) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        sockaddr_in client_address;
        socklen_t client_addr_len = sizeof(client_address);

        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_addr_len);
        if (client_socket == -1) {
            perror("Accepting connection failed");
            continue;
        }

        client_sockets.push_back(client_socket);

        // Create a new thread to handle the client
        std::thread(handle_client, client_socket).detach();
    }

    close(server_socket);
    return 0;
}
