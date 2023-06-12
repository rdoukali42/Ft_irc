#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>

const int MAX_BUFFER_SIZE = 1024;
const int MAX_CLIENTS = FD_SETSIZE - 1;
const int MAX_CHANNELS = 1023;

struct Client
{
	int socket;
	int indice;
	std::string nickname;
	std::string username;
};
Client clients[MAX_CLIENTS];

struct Channel
{
	int sok; //tmp var to test
	int index;
	int limit;
	std::vector<int> users_sockets;
	std::string name;
	std::string topic;
	std::string PRVIMSG_Index;
};
Channel channels[MAX_CHANNELS];

void error(const std::string& msg)
{
	std::cerr << "Error: " << msg << std::endl;
	exit(1);
}

int searchBySocket(const int &socket, const Client* clients, int numClients)
{
	for (int i = 0; i < numClients; i++)
	{
		if (socket == clients[i].socket)//working!
		{
			return i;
		}
	}
	return -1;
}


int searchByUsername(const std::string& target, const Client* clients, int numClients)
{
	std::string target2 = target;
	for (int i = 0; i < numClients; i++)
	{
		if (strcmp(clients[i].username.c_str(), target2.c_str()) == 0)//working!
		{
			return i;
		}
	}
	return -1;
}

int searchBychannelname(const std::string& target, const Channel* channels, int numChannels)
{
	std::string target2 = target;
	for (int i = 0; i < numChannels; i++)
	{
		if (channels[i].name == target2)
		{
			return i;
		}
	}
	return -1;
}

std::vector<std::string> split_str(std::string str, char delim)
{
	std::vector<std::string> row;
	std::string word;
	std::stringstream s(str);
	while (std::getline(s, word, delim)) {
		row.push_back(word);
	}
	return row;
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
	    std::cout << "Usage: ./a.out <port> <pass>" << std::endl;
	    return 0;
	}

	const int port = std::stoi(argv[1]);
	std::string password = argv[2];

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
	int channel_index = 1;

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
		    } else
		    {
				std::string passPrompt = "Please enter PassWord : ";
			     send(clientSocket, passPrompt.c_str(), passPrompt.length(), 0);
				char passbuffer[MAX_BUFFER_SIZE];
			     ssize_t bytesRead = read(clientSocket, passbuffer, sizeof(passbuffer));
			     if (bytesRead > 0) {
					passbuffer[bytesRead - 1] = '\0';
			         std::string username(passbuffer, bytesRead);
			     }
				if (password == passbuffer)
				{
					// Add the new client socket to the array
					clients[index].socket = clientSocket;
					std::cout << "New client connected: " << inet_ntoa(clientAddress.sin_addr) << std::endl;
					if (clients[index].indice != 1)
					{
						std::string usernamePrompt = "Please enter your username: ";
						send(clientSocket, usernamePrompt.c_str(), usernamePrompt.length(), 0);
						char usernameBuffer[MAX_BUFFER_SIZE];
						ssize_t bytesRead = read(clientSocket, usernameBuffer, sizeof(usernameBuffer));
						if (bytesRead > 0) {
							usernameBuffer[bytesRead - 1] = '\0';
							std::string username(usernameBuffer, bytesRead);
							// Assign the username to the client
							clients[index].username = username;
						}
						std::string nicknamePrompt = "Please enter your nickname: ";
						send(clientSocket, nicknamePrompt.c_str(), nicknamePrompt.length(), 0);
						char nicknameBuffer[MAX_BUFFER_SIZE];
						ssize_t bytesRead2 = read(clientSocket, nicknameBuffer, sizeof(nicknameBuffer));
						if (bytesRead2 > 0) {
							nicknameBuffer[bytesRead2 - 1] = '\0';
							std::string nickname(nicknameBuffer, bytesRead);
							// Assign the username to the client
							clients[index].nickname = nickname;
						}
						clients[index].indice = 1;
					}
				}
				else
				{
					std::string msg = "Wrong Password";
					ssize_t bytesWritten = send(clientSocket, msg.c_str(), msg.length(), 0);
					if (bytesWritten < 0) {
					    error("Sending data failed");
					}
					close(clientSocket);
				}

				// Update the maxFd value
				maxFd = std::max(maxFd, clientSocket);
		    }
		}

		// Check if there is data to be read from the client sockets
		for (int i = 0; i < MAX_CLIENTS; ++i)
		{
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
				// Process the received data
				// ... Add your code here to handle incoming messages, commands, etc.


				
				std::string message(buffer);
				if (message.substr(0, 8) == "/PRIVMSG")
				{
					int tmp = 0;
					// Extract the target and message from the user input
					std::string targetAndMessage = message.substr(9); // Remove the command prefix and space
					if (targetAndMessage.find("#") != std::string::npos)
					{
						targetAndMessage = message.substr(10);
						tmp = 1;
					}
					std::string::size_type pos = targetAndMessage.find(" ");
					std::string target = targetAndMessage.substr(0, pos); // Extract the target (username or channel)
					std::string text = targetAndMessage.substr(pos + 1); // Extract the message text

					// Form the PRIVMSG command to be sent to the server
					std::string privmsgCommand = clients[i].username + " say : " + text + "\r\n";
					if (tmp == 0)
					{
						int mem = searchByUsername(target, clients, MAX_CLIENTS);
						// Send the PRIVMSG command to the client
						ssize_t bytesWritten = send(clients[mem].socket, privmsgCommand.c_str(), privmsgCommand.length(), 0);
						if (bytesWritten < 0) {
							error("Sending data failed");
						}
					}
					else
					{
						/*need to be fixed*/
						int ind = searchBychannelname(target, channels, MAX_CHANNELS);
						if (ind == -1)
							error("Channel not found");
						for (std::size_t i = 0; i < channels[ind].users_sockets.size(); ++i) {
							ssize_t bytesWritten = send(channels[ind].users_sockets[i], privmsgCommand.c_str(), privmsgCommand.length(), 0);
							if (bytesWritten < 0) {
								error("Sending data failed");
							}
						}
						// for (std::vector<int>::const_iterator it = channels[ind].users_sockets.begin(); it <= channels[ind].users_sockets.end(); it++)
						// {
						// 	ssize_t bytesWritten = send(channels[ind].users_sockets[it], privmsgCommand.c_str(), privmsgCommand.length(), 0);
						// 	if (bytesWritten < 0) {
						// 		error("Sending data failed");
						// 	}
						// }
						// ssize_t bytesWritten = send(channels[ind].sok, privmsgCommand.c_str(), privmsgCommand.length(), 0);
						// if (bytesWritten < 0) {
						// 	error("Sending data failed");
						// }
						/*need to be fixed*/
					}
				}
				if (message.substr(0, 5) == "/JOIN")
				{
					// Extract the Channel name and message from the user input
					std::string channelAndMessage = message.substr(6); // Remove the command prefix and space
					std::string::size_type pos = channelAndMessage.find(" ");
					std::string channel = channelAndMessage.substr(0, pos); // Extract the channel name ---room1
					std::string text = channelAndMessage.substr(pos + 1); // Extract the message text

					//Check if the channel exist
					if (searchBychannelname(channel, channels, MAX_CHANNELS) > 0)
					{
						int channel_index2 = searchBychannelname(channel, channels, MAX_CHANNELS);
						if (channels[channel_index2].users_sockets.size() <= channels[channel_index2].limit)
							channels[channel_index2].users_sockets.push_back(clients[i].socket);
						else{
							std::string channelFullPrompt = "Channel " + channel + " is full\n";
							send(clientSocket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
						}
						std::string roomPrompt = "Channel Name is : " + channels[channel_index2].name + "\nTopic : " + channels[channel_index2].topic + "\nLimit is : " + std::to_string(channels[channel_index2].limit) + "\nAllowed Private Msg : " + channels[channel_index2].PRVIMSG_Index + "\n";
						send(clientSocket, roomPrompt.c_str(), roomPrompt.length(), 0);
						for (std::size_t i = 0; i < channels[channel_index2].users_sockets.size(); ++i) {
							std::cout << "USER " + clients[searchBySocket(channels[channel_index2].users_sockets[i], clients, MAX_CLIENTS)].username << std::endl;
						}
						std::cout << "number of users in channel:" << channels[channel_index2].users_sockets.size() << std::endl;
						std::cout << "users_sockets.size:" <<  channels[channel_index2].users_sockets.size() << ". limit:" << channels[channel_index2].limit << std::endl;
					}
					else
					{
						//Create a channel if dosn't exist
						channels[channel_index].name = channel;
						std::string TopicPrompt = "Please enter a TOPIC for the " + channel + " channel : ";
						send(clientSocket, TopicPrompt.c_str(), TopicPrompt.length(), 0);
						char TopicBuffer[MAX_BUFFER_SIZE];
						ssize_t bytesRead = read(clientSocket, TopicBuffer, sizeof(TopicBuffer));
						if (bytesRead > 0) {
							TopicBuffer[bytesRead - 1] = '\0';
							std::string Topic(TopicBuffer, bytesRead);
							// Assign the topic to the channel
							channels[channel_index].topic = Topic;
						}
						std::string limitPrompt = "Please enter a limit for the " + channel + " channel users : ";
						send(clientSocket, limitPrompt.c_str(), limitPrompt.length(), 0);
						char limitBuffer[MAX_BUFFER_SIZE];
						ssize_t bytesRead3 = read(clientSocket, limitBuffer, sizeof(limitBuffer));
						if (bytesRead3 > 0) {
							limitBuffer[bytesRead3 - 1] = '\0';
							int limit = std::stoi(limitBuffer);
							// Assign the topic to the channel
							channels[channel_index].limit= limit;
						}
						std::string indexPrompt = "Do you want to Allow Private Msg in " + channel + " channel (yes/no) : ";
						send(clientSocket, indexPrompt.c_str(), indexPrompt.length(), 0);
						char indexBuffer[MAX_BUFFER_SIZE];
						ssize_t bytesRead4 = read(clientSocket, indexBuffer, sizeof(indexBuffer));
						if (bytesRead4 > 0) {
							indexBuffer[bytesRead4 - 1] = '\0';
							std::string indexx(indexBuffer, bytesRead);
							if (indexx.substr(0, 3) == "yes" || indexx.substr(0, 2) == "no")
							{
								// Assign the index to the channel
								channels[channel_index].PRVIMSG_Index = indexx;
								channels[channel_index].index = channel_index; // using index to check if the channel is created or not (if 0 ....)
								if (channels[channel_index].limit != 0)
								{
									std::cout << "pushed " << clients[i].username << std::endl;
									channels[channel_index].users_sockets.push_back(clients[i].socket);
								}
								std::string iPrompt = "Channel " + channel + " Created\n";
								send(clientSocket, iPrompt.c_str(), iPrompt.length(), 0);
								channel_index++;
							}
							else
							{
								std::string indexPrompt = "Wrong INPUT !! Channel not saved";
								send(clientSocket, indexPrompt.c_str(), indexPrompt.length(), 0);
								channels[channel_index].topic = "";
								channels[channel_index].name = "";
							}
						}
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