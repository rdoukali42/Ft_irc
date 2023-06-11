#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>
#include <vector> //added vector header library

const int MAX_BUFFER_SIZE = 1024;
const int MAX_CLIENTS = FD_SETSIZE - 1;

struct Client
{
    int socket;
    int indice;
    std::string nickname;
    std::string username;
    std::vector<char> inputBuffer;  //added
    std::vector<char> outputBuffer; //added
};
Client clients[MAX_CLIENTS];

void error(const std::string &msg)
{
    std::cerr << "Error: " << msg << std::endl;
    exit(1);
}

int searchByUsername(const std::string &target, const Client *clients, int numClients)
{
    std::string target2 = target + "\n";
    for (int i = 0; i < numClients; i++)
    {
        if (clients[i].username == target2)
        {
            return i;
        }
    }
    return 0;
}

void setNonBlocking(int socket)
{
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        error("Failed to get socket flags");
    }
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        error("Failed to set socket to non-blocking mode");
    }
}

ssize_t readFromSocket(int socket, std::vector<char> &buffer)
{
    ssize_t bytesRead = recv(socket, buffer.data(), buffer.size(), 0);
    if (bytesRead < 0) {
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            error("Failed to read from socket");
        }
    }
    return bytesRead;
}

ssize_t writeToSocket(int socket, const std::vector<char> &buffer)
{
    ssize_t bytesWritten = send(socket, buffer.data(), buffer.size(), 0);
    if (bytesWritten < 0) {
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            error("Failed to write to socket");
        }
    }
    return bytesWritten;
}

void processReceivedData(Client &client)
{
    std::vector<char> &inputBuffer = client.inputBuffer;

    // Process the received data in the input buffer
    // ... Add your code here to handle incoming messages, commands, etc.
    // Example: Echo the received message back to the client
    // client.outputBuffer.insert(client.outputBuffer.end(), inputBuffer.begin(), inputBuffer.end());
    // inputBuffer.clear();

    // Example: Print received message to the console
    std::cout << "Received from client " << client.indice << ": " << std::string(inputBuffer.data(), inputBuffer.size()) << std::endl;
}


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: ./a.out <port>" << std::endl;
        return 0;
    }

    const int port = std::stoi(argv[1]);

    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        error("Failed to create socket");
    }

    // Prepare server address structure
    // struct sockaddr_in serverAddress{};
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
        error("Failed to bind socket");
    }

    // Listen for client connections
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        error("Failed to listen for connections");
    }

    // Set the server socket to non-blocking mode
    setNonBlocking(serverSocket);

    // Set up the file descriptor sets
    fd_set readFds;
    fd_set writeFds;
    int maxFd = serverSocket;

    // Main server loop
    while (true) {
        // Clear the file descriptor sets
        FD_ZERO(&readFds);
        FD_ZERO(&writeFds);

        // Add server socket to the read set
        FD_SET(serverSocket, &readFds);

        // Add client sockets to the read and write sets
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].socket > 0) {
                int clientSocket = clients[i].socket;
                FD_SET(clientSocket, &readFds);
                FD_SET(clientSocket, &writeFds);
                if (clientSocket > maxFd) {
                    maxFd = clientSocket;
                }
            }
        }

        // Wait for activity on any of the file descriptors
        if (select(maxFd + 1, &readFds, &writeFds, nullptr, nullptr) == -1) {
            error("Failed to select");
        }

        // Check if there is a new client connection
        if (FD_ISSET(serverSocket, &readFds)) {
            // Accept the new connection
            int clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == -1) {
                error("Failed to accept client connection");
            }

            // Find an available slot for the new client
            int clientIndex = -1;
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].socket == 0) {
                    clientIndex = i;
                    break;
                }
            }

            if (clientIndex >= 0) {
                // Set the client socket to non-blocking mode
                setNonBlocking(clientSocket);

                // Add the new client to the client array
                clients[clientIndex].socket = clientSocket;
                clients[clientIndex].indice = clientIndex;
                clients[clientIndex].inputBuffer.resize(MAX_BUFFER_SIZE);
                clients[clientIndex].outputBuffer.resize(MAX_BUFFER_SIZE);

                std::cout << "New client connected. Client index: " << clientIndex << std::endl;
            } else {
                // No available slot for the new client, so close the connection
                close(clientSocket);
                std::cout << "Maximum number of clients reached. Connection closed." << std::endl;
            }
        }

        // Process client activity
        for (int i = 0; i < MAX_CLIENTS; i++) {
            Client& client = clients[i];
            int clientSocket = client.socket;

            if (FD_ISSET(clientSocket, &readFds)) {
                ssize_t bytesRead = readFromSocket(clientSocket, client.inputBuffer);
                if (bytesRead > 0) {
                    // Process the received data
                    processReceivedData(client);
                } else if (bytesRead == 0) {
                    // Connection closed by the client
                    std::cout << "Client disconnected. Client index: " << client.indice << std::endl;

                    // Close the socket and clear the client data
                    close(clientSocket);
                    client.socket = 0;
                    client.nickname.clear();
                    client.username.clear();
                    client.inputBuffer.clear();
                    client.outputBuffer.clear();
                }
            }

            if (FD_ISSET(clientSocket, &writeFds)) {
                // Check if there is data to send in the output buffer
                if (!client.outputBuffer.empty()) {
                    ssize_t bytesWritten = writeToSocket(clientSocket, client.outputBuffer);
                    if (bytesWritten > 0) {
                        // Remove the sent data from the output buffer
                        client.outputBuffer.erase(client.outputBuffer.begin(), client.outputBuffer.begin() + bytesWritten);
                    }
                }
            }
        }
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}

