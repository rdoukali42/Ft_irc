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

const int MAX_BUFFER_SIZE = 1024;
const int MAX_CLIENTS = FD_SETSIZE - 1;

struct Client
{
	int socket;
	int indice;
	std::string nickname;
	std::string username;
};
Client clients[MAX_CLIENTS];

void error(const std::string& msg)
{
	std::cerr << "Error: " << msg << std::endl;
	exit(1);
}

int searchByUsername(const std::string& target, const Client* clients, int numClients)
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

int main(int argc, char* argv[])
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
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);

	// Bind the socket to the specified address and port
	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
	    error("Failed to bind socket");
	}

	// Start listening for incoming connections
	if (listen(serverSocket, SOMAXCONN) == -1) {
	    error("Failed to listen for connections");
	}

	std::cout << "Server is listening on port " << port << std::endl;

	char buffer[MAX_BUFFER_SIZE];

	// Create an array of client sockets
	//int clientSockets[MAX_CLIENTS];
	for (int i = 0; i < MAX_CLIENTS; ++i) {
	    clients[i].socket = -1;
	   clients[i].indice = 0;
	}

	fd_set readFds;
	int maxFd = serverSocket;

	while (true)
	{
		// Clear the set and add the server socket to the set
		FD_ZERO(&readFds);
		FD_SET(serverSocket, &readFds);

		// Add the client sockets to the set
		for (int i = 0; i < MAX_CLIENTS; ++i) {
		  //   int clientSocket = clientSockets[i];
		    if (clients[i].socket != -1) {
		        FD_SET(clients[i].socket, &readFds);
		        maxFd = std::max(maxFd, clients[i].socket);
		    }
		}

		// Use select() to handle events
		int numReady = select(maxFd + 1, &readFds, nullptr, nullptr, nullptr);
		if (numReady == -1) {
		    error("select() failed");
		}

		// Check if a new connection is ready to be accepted
		if (FD_ISSET(serverSocket, &readFds)) {
		    struct sockaddr_in clientAddress;
		    socklen_t clientAddressLength = sizeof(clientAddress);
		    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
		    if (clientSocket == -1) {
		        error("Failed to accept connection");
		    }

		    // Find an empty slot in the clientSockets array
		    int index = -1;
		    for (int i = 0; i < MAX_CLIENTS; ++i) {
		        if (clients[i].socket == -1) {
		            index = i;
		            break;
		        }
		    }

		    if (index == -1) {
		        // No empty slot available, close the new connection
		        close(clientSocket);
		        std::cout << "Rejected new connection: Too many clients" << std::endl;
		    } else {
		        // Add the new client socket to the array
		        clients[index].socket = clientSocket;
		        std::cout << "New client connected: " << inet_ntoa(clientAddress.sin_addr) << std::endl;

		        // Update the maxFd value
		        maxFd = std::max(maxFd, clientSocket);
		    }
		}

		// Check if there is data to be read from the client sockets
		for (int i = 0; i < MAX_CLIENTS; ++i) {
			int clientSocket = clients[i].socket;
			if (clientSocket != -1 && FD_ISSET(clientSocket, &readFds)) {
			   // Read data from the client socket
			   ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
			   if (bytesRead < 0) {
			       error("Reading data failed");
			   } else if (bytesRead == 0 || std::strncmp(buffer, "KICK", 4) == 0)
			   {
			       // Connection closed by the client
			       close(clientSocket);
			       clients[i].socket = -1;  // Remove the client socket from the array
			       std::cout << "Client disconnected : "<< clients[i].username << std::endl;
			   }
			else
			 {
			        buffer[bytesRead] = '\0';
				if (clients[i].indice != 1)
			        {
					std::string usernamePrompt = "Please enter your username: ";
			        	send(clientSocket, usernamePrompt.c_str(), usernamePrompt.length(), 0);
			        	char usernameBuffer[MAX_BUFFER_SIZE];
			        	ssize_t bytesRead = read(clientSocket, usernameBuffer, sizeof(usernameBuffer));
			        	if (bytesRead > 0) {
			        	    std::string username(usernameBuffer, bytesRead);
			        	    // Assign the username to the client
			        	    clients[i].username = username;
					    clients[i].indice = 1;
			        	}
				}
			        // Process the received data
			        // ... Add your code here to handle incoming messages, commands, etc.
			
			
			        
			        std::string message(buffer);
			        if (message.substr(0, 8) == "/PRIVMSG")
			        {
			            // Extract the target and message from the user input
			            std::string targetAndMessage = message.substr(9); // Remove the command prefix and space
			            std::string::size_type pos = targetAndMessage.find(" ");
			            std::string target = targetAndMessage.substr(0, pos); // Extract the target (nickname or channel name)
			            std::string text = targetAndMessage.substr(pos + 1); // Extract the message text
			
			            // Form the PRIVMSG command to be sent to the server
			            std::string privmsgCommand = clients[i].username + "say : " + text + "\r\n";
			
					int mem = searchByUsername(target, clients, MAX_CLIENTS);
			            // Send the PRIVMSG command to the server
			            ssize_t bytesWritten = send(clients[mem].socket, privmsgCommand.c_str(), privmsgCommand.length(), 0);
			            if (bytesWritten < 0) {
			                error("Sending data failed");
			            }
			        }
			
			
			        // Example: Print received message to the console
			        // std::cout << "Received from client: " << std::string(buffer, bytesRead) << std::endl;
			
			        // // Example: Echo the received message back to the client
			        // ssize_t bytesSent = send(clientSocket, buffer, bytesRead, 0);
			        // if (bytesSent == -1) {
			        //     error("Sending data failed");
			        // }
			    }
			}
		}
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}