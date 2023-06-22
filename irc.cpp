/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdoukali <rdoukali@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 00:08:32 by rdoukali          #+#    #+#             */
/*   Updated: 2023/06/22 19:58:42 by rdoukali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

Channel channels[MAX_CHANNELS];
Client clients[MAX_CLIENTS];

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
	int channel_index = 0;

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
			char passbuffer[MAX_BUFFER_SIZE];
			for (int i = 3; i >= 0; i--)
			{
				if (i == 3)
				{
					std::string passPrompt = "Please enter PassWord : ";
					send(clientSocket, passPrompt.c_str(), passPrompt.length(), 0);
					ssize_t bytesRead = read(clientSocket, passbuffer, sizeof(passbuffer));
					if (bytesRead > 0) {
						passbuffer[bytesRead - 1] = '\0';
						std::string username(passbuffer, bytesRead);
					}
					if (password == passbuffer)
						break ;
				}
				else if (i > 0 && password != passbuffer)
				{
					std::string passPrompt = "Wrong Password (" + std::to_string(i) + " retry left) : ";
					send(clientSocket, passPrompt.c_str(), passPrompt.length(), 0);
					ssize_t bytesRead = read(clientSocket, passbuffer, sizeof(passbuffer));
					if (bytesRead > 0) {
						passbuffer[bytesRead - 1] = '\0';
						std::string username(passbuffer, bytesRead);
					}
					if (password == passbuffer)
						break ;
				}
				else
				{
					std::string msg = "Wrong Password ! No retry left";
					ssize_t bytesWritten = send(clientSocket, msg.c_str(), msg.length(), 0);
					if (bytesWritten < 0) {
						error("Sending data failed");
					}
					close(clientSocket);
				}
				
			}
				if (password == passbuffer)
				{
					// Add the new client socket to the array
					clients[index].socket = clientSocket;
					std::cout << "New client connected: " << inet_ntoa(clientAddress.sin_addr) << std::endl;
					if (clients[index].indice != 1)
					{
						ssize_t bytesRead;
						while(1){
						std::string usernamePrompt = "Please enter your username: ";
						send(clientSocket, usernamePrompt.c_str(), usernamePrompt.length(), 0);
						char usernameBuffer[MAX_BUFFER_SIZE];
						bytesRead = read(clientSocket, usernameBuffer, sizeof(usernameBuffer));
						if (bytesRead > 0) {
							//usernameBuffer[bytesRead - 1] = '\0';
							std::string username(usernameBuffer, bytesRead);
							for (int i = 0; i < username.length(); i++){
								username.erase(username.find_last_not_of(" \t\r\n") + 1);
							}
							std::string::size_type pos = username.find(" ");
							if (pos != std::string::npos)// The " " character was found in the string.
							{
								sendUser("UserName should be in one word", clientSocket);
							}
							else if (searchByUsername(username, clients, MAX_CLIENTS) != -1 && ifWord(username))
							{
								std::string usernamePrompt = "username already exist !\n";
								send(clientSocket, usernamePrompt.c_str(), usernamePrompt.length(), 0);
							}
							else if (ifWord(username))
							{
								clients[index].username = username;// Assign the username to the client
								break;
							}
						}
						}
						std::string nicknamePrompt = "Please enter your nickname: ";
						send(clientSocket, nicknamePrompt.c_str(), nicknamePrompt.length(), 0);
						char nicknameBuffer[MAX_BUFFER_SIZE];
						ssize_t bytesRead2 = read(clientSocket, nicknameBuffer, sizeof(nicknameBuffer));
						if (bytesRead2 > 0) {
							// nicknameBuffer[bytesRead2 - 1] = '\0';
							std::string nickname(nicknameBuffer, bytesRead2);
							nickname.erase(nickname.find_last_not_of(" \t\r\n") + 1);
							// Assign the username to the client
							clients[index].nickname = nickname;
						}
						clients[index].indice = 1;
					}
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
				} else if (bytesRead == 0)
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

				std::string message(buffer);
				message.erase(message.find_last_not_of(" \t\r\n") + 1);
				spaces_erase(message);
				// sendUser("|" + message + "|", clientSocket);
				if (checkArg(message, clientSocket) == -1)
					continue ;
				else if (message.substr(0, 8) == "/PRIVMSG")
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
					std::string privmsgCommand = clients[i].username + " : " + text;
					if (tmp == 0)
					{
						int mem = searchByUsername(target, clients, MAX_CLIENTS);// Send the PRIVMSG command to the client
						if (mem == -1)
							errorUser(target + ": USER NOT FOUND", clientSocket);
						else
						{
							ssize_t bytesWritten = send(clients[mem].socket, privmsgCommand.c_str(), privmsgCommand.length(), 0);
							if (bytesWritten < 0) {
								error("Sending data failed");
							}
						}
					}
					else
					{
						int ind = searchBychannelname(target, channels, MAX_CHANNELS);
						if (ind == -1)
							errorUser(target + ": CHANNEL NOT FOUND", clientSocket);
						else if (strcmp(channels[ind].PRVIMSG_Index.c_str(), "yes") == 0 && searchIfExist(channels[ind].users_sockets, clientSocket))
						{
							for (std::size_t i = 0; i < channels[ind].users_sockets.size(); ++i)
							{
								ssize_t bytesWritten = send(channels[ind].users_sockets[i], privmsgCommand.c_str(), privmsgCommand.length(), 0);
								if (bytesWritten < 0) {
									error("Sending data failed");
								}
							}
						}
						else
						{
							std::string msgPrompt = "No permission To send PRIVMSG in this Channel \n";
							send(clients[i].socket, msgPrompt.c_str(), msgPrompt.length(), 0);
						}
					}
				}
				else if (message.substr(0, 5) == "/JOIN")
				{
					// Extract the Channel name and message from the user input
					std::string channelAndMessage = message.substr(6); // Remove the command prefix and space
					std::string::size_type pos = channelAndMessage.find(" ");
					std::string channel = channelAndMessage.substr(1); // Extract the channel name ---room1
					channel.erase(channel.find_last_not_of(" \t\r\n") + 1);

					std::string ch = channelAndMessage.substr(0);
					if ((channelAndMessage.find("#") == std::string::npos) || ch[0] != '#')
					{
						errorUser("/JOIN <#channel>", clientSocket);
					}
					//Check if the channel exist
					else if (searchBychannelname(channel, channels, MAX_CHANNELS) != -1)
						channelExist(clientSocket, channels, clients, channel, i);
					else//if the channel not exist
						channelNotExist(clientSocket, channels, clients, channel, i, &channel_index);
				}
				else if (message.substr(0, 5) == "/KICK")
				{
					// Extract the Channel name and username from the user input
					std::string channelAnduser = message.substr(6); // Remove the command prefix and space
					std::string::size_type pos = channelAnduser.find(" ");
					std::string channelname = channelAnduser.substr(1, pos - 1); // Extract the channel starting from 1 to avoid '#'
					std::string userAndmsg = channelAnduser.substr(pos + 1);
					std::string user;
					std::string::size_type poss = userAndmsg.find(" ");//poss is a giant number, probably due to having no space after the username, I changed to it \n so it's getting the correct username
					if (poss != std::string::npos)// The " " character was found in the string.
					{
						user = userAndmsg.substr(0, poss); // Extract the username
						std::string msg = userAndmsg.substr(poss + 1);
					}
					else
					{
						user = userAndmsg.substr(0); // Extract the username
						user.erase(user.find_last_not_of(" \t\r\n") + 1);
					}
					if (clients[i].username != user)
						kickUser(channels, clients, channelname, user, i);
					else
						errorUser("Use /PART to Leave Channel", clientSocket);
				}
				else if (message.substr(0, 6) == "/TOPIC")
				{
					std::string channelAndmsg = message.substr(7); // Remove the command prefix and space
					std::string::size_type pos = channelAndmsg.find(" ");
					try{
					if (pos != std::string::npos)// The " " character was found in the string.
					{
						std::string channelname = channelAndmsg.substr(1, pos - 1); // Extract the channel starting from 1 to avoid '#'
						if (channels[searchBychannelname(channelname, channels, MAX_CHANNELS)].topic_mode == 1)
						{
							if (!isAdmin(channels[searchBychannelname(channelname, channels, MAX_CHANNELS)].admin_users, clients[i].username))
								throw std::runtime_error("Error: Topic mode is set!");
						}
						std::string msg = channelAndmsg.substr(pos + 1); // Extract the message text
						msg.erase(msg.find_last_not_of(" \t\r\n") + 1);
						if (searchBychannelname(channelname, channels, MAX_CHANNELS) == -1)
							errorUser("CHANNEL NOT FOUND", clientSocket);
						else
							channels[searchBychannelname(channelname, channels, MAX_CHANNELS)].topic = msg;
							std::string privmsgCommand = "New TOPIC is set: " + msg + "\r\n";
							ssize_t bytesWritten = send(clients[i].socket, privmsgCommand.c_str(), privmsgCommand.length(), 0);
							if (bytesWritten < 0) {
								error("Sending data failed");
							}
					}
					else // The " " character was not found in the string --> that mean there is no msg
					{
						std::string channelname = channelAndmsg.substr(pos + 2);
						channelname.erase(channelname.find_last_not_of(" \t\r\n") + 1);// Remove trailing whitespace characters
						int ind = searchBychannelname(channelname, channels, MAX_CHANNELS);
						if (ind == -1)
							errorUser("CHANNEL NOT FOUND", clients[i].socket);
						if (channels[searchBychannelname(channelname, channels, MAX_CHANNELS)].topic_mode == 1)
						{
							if (!isAdmin(channels[searchBychannelname(channelname, channels, MAX_CHANNELS)].admin_users, clients[i].username))
								throw std::runtime_error("Error: Topic mode is set!");
						}
						std::string privmsgCommand = "TOPIC is : " + channels[ind].topic + "\r\n";
						ssize_t bytesWritten = send(clients[i].socket, privmsgCommand.c_str(), privmsgCommand.length(), 0);
						if (bytesWritten < 0) {
							error("Sending data failed");
						}
					}
					}
					catch(std::runtime_error &e){
						std::string indexPrompt =  std::string(e.what()) + "\n";
						send(clientSocket, indexPrompt.c_str(), indexPrompt.length(), 0);
					}
				}
				else if (message.substr(0, 5) == "/MODE")/// should check if the client is ADMIN
				{
					std::string channelAndmsg = message.substr(6); // Remove the command prefix and space--> ex : "#room +i testmsg"
					std::string::size_type pos = channelAndmsg.find(" "); //--> ex : 5
					std::string channel = channelAndmsg.substr(1, pos - 1); //--> ex : "room"
					std::string ch = channelAndmsg.substr(0);
					if ((channelAndmsg.find("#") == std::string::npos) || ch[0] != '#')
					{
						errorUser("/MODE <#channel>", clientSocket);
					}
					else if (searchBychannelname(channel, channels, MAX_CHANNELS) == -1)
						errorUser("CHANNEL NOT FOUND", clientSocket); 
					else if (isAdmin(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users, clients[i].username)){
						std::string argsAndmsg = channelAndmsg.substr(pos + 1); //--> ex : "+i testmsg"
						std::string::size_type poss = argsAndmsg.find(" ");// ex : 2
						if (poss != std::string::npos)// The " " character was found in the string.
						{
							std::string args = argsAndmsg.substr(0, poss); // Extract the args with the sign --> ex : "+i"
							std::string msg = argsAndmsg.substr(poss + 1); // Extract the message text --> ex : "testmsg"
							msg.erase(msg.find_last_not_of(" \t\r\n") + 1);// Remove trailing whitespace characters
							modeOptions(channels, clients, channel, args, msg , i);
						}
						else // The " " character was not found in the string --> that mean there is no msg
						{
							std::string args = argsAndmsg.substr(0);
							modeNoOptions(channels, clients, channel, args, i);
						}
					}
					else{
						std::string kickPrompt = "You are not an ADMIN \n";
						send(clients[i].socket, kickPrompt.c_str(), kickPrompt.length(), 0);
					}
				}
				else if (message.substr(0, 7) == "/INVITE")
				{
					// Extract the Channel name and username from the user input
					std::string channelAnduser = message.substr(8); // Remove the command prefix and space
					std::string::size_type pos = channelAnduser.find(" ");
					std::string channelname = channelAnduser.substr(1, pos - 1); // Extract the channel starting from 1 to avoid '#'
					std::string user = channelAnduser.substr(pos + 1);
					user.erase(user.find_last_not_of(" \t\r\n") + 1);
					if (searchBychannelname(channelname, channels, MAX_CHANNELS) != -1)
					{
						if (searchByUsername(user, clients, MAX_CLIENTS) != -1)
							inviteUser(clients[searchByUsername(user, clients, MAX_CLIENTS)].socket, channels, clients, channelname, searchByUsername(user, clients, MAX_CLIENTS));
						else
							errorUser("USER NOT FOUND", clientSocket);
					}
					else
						errorUser("CHANNEL NOT FOUND", clientSocket);
				}
				else if (message.substr(0, 5) == "/PART")
				{
					// Extract the Channel name and username from the user input
					std::string channelname = message.substr(7); // Extract the channel starting from 1 to avoid '#'
					channelname.erase(channelname.find_last_not_of(" \t\r\n") + 1);
					kickUser(channels, clients, channelname, clients[i].username, i);
				}
				else if (message.substr(0, 6) == "/WHOIS")
				{
					// Extract the username from the user input
					std::string username = message.substr(7);
					username.erase(username.find_last_not_of(" \t\r\n") + 1);
					int cl_i = searchByUsername(username, clients, MAX_CLIENTS);
					if (cl_i != -1)
					{
						sendUser("UserName : " + clients[cl_i].username, clients[i].socket);
						sendUser("NickName : " + clients[cl_i].nickname, clients[i].socket);
						sendUser("Channels : ", clients[i].socket);
						user_channels(channels, clients, cl_i, clients[i].socket);
						sendUser("------------------------WHOIS--------------------------", clients[i].socket);
					}
					else
						errorUser("USER NOT FOUND", clientSocket);
				}
				else if (message.substr(0, 5) == "/NICK")
				{
					// Extract the Channel name and username from the user input
					std::string nickname = message.substr(6); // Extract the channel starting from 1 to avoid '#'
					nickname.erase(nickname.find_last_not_of(" \t\r\n") + 1);
					clients[i].nickname = nickname;
				}
				else if (message.substr(0, 5) == "/LIST")
				{
					listChannels(channels, clients, i);
				}
				else if (message.substr(0, 5) == "/QUIT")
				{
					close(clients[i].socket);
					clients[i].socket = -1;
					std::cout << "Client disconnected : "<< clients[i].username << std::endl;
				}
				else if (message.substr(0, 5) == "/EXIT")
				{
					close(serverSocket);
					return 0;
				}
				}
			}
		}
	}
	// Close the server socket
	close(serverSocket);
	return 0;
}