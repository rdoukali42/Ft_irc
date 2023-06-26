#include "irc.hpp"

Channel channels[MAX_CHANNELS];
Client clients[MAX_CLIENTS];

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cout << "Usage: ./a.out <port> <pass>" << std::endl;
		return 1;
	}

	int port;
	try{
		char *end;
		errno = 0;
		long portConv = (std::strtol(argv[1], &end, 10));
		if ((errno == ERANGE && (portConv == LONG_MAX || portConv == LONG_MIN)) || (errno != 0 && portConv == 0)) 
			throw std::runtime_error(" Port is not valid!");
		if (end == argv[1] || *end != '\0')
			throw std::runtime_error(" Port is not a number!");
		if (*end == '\0')
			port= static_cast<int>(portConv);
		else
			throw std::runtime_error(" Port is not a number!");
	}
	catch (std::runtime_error &e){
		std::cerr << "Input error:" + std::string(e.what()) << std::endl;
		exit(1);
	}

	// const int port = std::stoi(argv[1]);
	std::string password = argv[2];

	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		error("Failed to create socket");
	}

	struct sockaddr_in serv_add;
	memset(&serv_add, 0, sizeof(serv_add));
	serv_add.sin_family = AF_INET;
	serv_add.sin_addr.s_addr = INADDR_ANY;
	serv_add.sin_port = htons(port);

	if (bind(serverSocket, (struct sockaddr*)&serv_add, sizeof(serv_add)) == -1) {
		error("Failed to bind socket");
	}

	if (listen(serverSocket, SOMAXCONN) == -1) {
		error("Failed to listen for connections");
	}

	std::cout << "Server is listening on port " << port << std::endl;

	char buffer[MAX_BUFFER_SIZE];

	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		clients[i].socket = -1;
		clients[i].indice = 0;
		clients[i].temp_pass = 0;
	}

	fd_set fds;
	int fds_max = serverSocket;
	int channel_index = 0;

	while (true)
	{
		FD_ZERO(&fds);
		FD_SET(serverSocket, &fds);

		for (int i = 0; i < MAX_CLIENTS; ++i)
		{
			if (clients[i].socket != -1) 
			{
				FD_SET(clients[i].socket, &fds);
				fds_max = std::max(fds_max, clients[i].socket);
			}
		}

		int nb_read = select(fds_max + 1, &fds, nullptr, nullptr, nullptr);
		if (nb_read == -1) {
			error("select() failed");
		}

		if (FD_ISSET(serverSocket, &fds)) {
			struct sockaddr_in cl_addr;
			socklen_t cl_addr_len = sizeof(cl_addr);
			int clientSocket = accept(serverSocket, (struct sockaddr*)&cl_addr, &cl_addr_len);
			if (clientSocket == -1) {
				error("Failed to accept connection");
			}

			int index = -1;
			for (int i = 0; i < MAX_CLIENTS; ++i) {
				if (clients[i].socket == -1) {
					index = i;
					break;
				}
			}

			if (index == -1) {
				close(clientSocket);
				std::cout << "Rejected new connection: Too many clients" << std::endl;
			} else
			{
				char passbuffer[MAX_BUFFER_SIZE];
				clients[index].socket = clientSocket;
				std::cout << "New client connected: " << inet_ntoa(cl_addr.sin_addr) << std::endl;
				clients[index].indice = 1;
				clients[index].username = "user" + std::to_string(index);
				clients[index].nickname = "nick" + std::to_string(index);
				fds_max = std::max(fds_max, clientSocket);
			}
		}

		// Check if there is data to be read from the client sockets
		for (int i = 0; i < MAX_CLIENTS; ++i)
		{
			int clientSocket = clients[i].socket;
			if (clientSocket != -1 && FD_ISSET(clientSocket, &fds)) {
				// Read data from the client socket
				ssize_t nb_read = recv(clientSocket, buffer, sizeof(buffer), 0);
				if (nb_read < 0) {
					error("Reading data failed");
				} else if (nb_read == 0)
				{
					// Connection closed by the client
					close(clientSocket);
					clients[i].socket = -1;  // Remove the client socket from the array
					std::cout << "Client disconnected : "<< clients[i].username << std::endl;
				}
			else
			{
				buffer[nb_read] = '\0';
				std::string message(buffer);
				message.erase(message.find_last_not_of(" \t\r\n") + 1);
				erase_spaces(message);
				// std::cout<< "Received --> [" << message << "]" << std::endl;
				std::vector<std::string> args = split_str(message, ' ');

				if (checkArg(message, clientSocket) == -1)
					continue ;
				if (clients[i].temp_pass == 0)
				{
					if (message.substr(0, 4) != "PASS" || args[1] != password)
					{
						errorUser("Wrong Password! disconecting", clientSocket);
						close(clientSocket);
						clients[i].username = "";
						clients[i].nickname = "";
						clients[i].socket = -1;
						clients[i].temp_pass = 1;
						continue ;
					}
				}
				else if (message.substr(0, 7) == "PRIVMSG")
				{
					int tmp = 0;
					if (args[1][0] == '#')
						tmp = 1;
					// Form the PRIVMSG command to be sent to the server
					std::string privmsgCommand = clients[i].username + " : " + getMsg(message);
					if (tmp == 0)
					{
						int mem = searchByUsername(args[1], clients, MAX_CLIENTS);// Send the PRIVMSG command to the client
						if (mem == -1)
							errorUser(args[1] + ": USER NOT FOUND", clientSocket);
						else
						{
							sendUser(privmsgCommand, clients[mem].socket, clients[mem].nickname);
						}
					}
					else
					{
						args[1] = args[1].substr(1);
						int ind = searchBychannelname(args[1], channels, MAX_CHANNELS);
						if (ind == -1)
							errorUser(args[1] + ": CHANNEL NOT FOUND", clientSocket);
						else if (strcmp(channels[ind].PRVIMSG_Index.c_str(), "yes") == 0 && searchIfExist(channels[ind].users_sockets, clientSocket))
						{
							for (std::size_t i = 0; i < channels[ind].users_sockets.size(); i++)
							{
								if (channels[ind].users_sockets[i] != clientSocket)
									sendUser(privmsgCommand, channels[ind].users_sockets[i], "#" + channels[ind].name);
							}
						}
						else
						{
							sendUser("No permission To send PRIVMSG in this Channel", clients[i].socket, clients[i].nickname);
						}
					}
				}
				else if (message.substr(0, 4) == "JOIN")
				{
					if (args[1][0] != '#')
						errorUser("/JOIN <#channel>", clientSocket);
					else if (channels[searchBychannelname(args[1].substr(1), channels, MAX_CHANNELS)].key_mode == 1)
					{
						if (args.size() == 3 && args[2] == channels[searchBychannelname(args[1].substr(1), channels, MAX_CHANNELS)].password)
						{
							args[1] = args[1].substr(1);
							if (searchBychannelname(args[1], channels, MAX_CHANNELS) != -1)
								channelExist(clientSocket, channels, clients, args[1], i);
							else//if the channel not exist
								channelNotExist(clientSocket, channels, clients, args[1], i, &channel_index);
						}
						else
							errorUser("Wrong Password!!", clientSocket);
					}
					else{
						args[1] = args[1].substr(1);
						if (searchBychannelname(args[1], channels, MAX_CHANNELS) != -1)
							channelExist(clientSocket, channels, clients, args[1], i);
						else//if the channel not exist
							channelNotExist(clientSocket, channels, clients, args[1], i, &channel_index);
					}
				}
				else if (message.substr(0, 4) == "KICK")
				{
					try{
						if (args[1][0] != '#')
							throw std::runtime_error("Error: /KICK <#channel> <user>");
						args[1] = args[1].substr(1);
						if (clients[i].username != args[2])
							kickUser(channels, clients, args[1], args[2], i);
						else
							errorUser("Use /PART to Leave Channel", clientSocket);
					}
					catch(std::runtime_error &e){
						std::string kickErrorPrompt =  std::string(e.what());
						sendUser(kickErrorPrompt, clients[i].socket, clients[i].nickname);
					}
				}
				else if (message.substr(0, 5) == "TOPIC")
				{
					std::string channelAndmsg = message.substr(6); // Remove the command prefix and space
					std::string::size_type pos = channelAndmsg.find(" ");
					try{
						std::string ch = channelAndmsg.substr(0);
						if (args[1][0] != '#')
							throw std::runtime_error("Error: /TOPIC <#channel> <new_topic>");
						else if (args.size() > 2)//there is a msg/topic to set
						{
							args[1] = args[1].substr(1); // Extract the channel starting from 1 to avoid '#'
							if (channels[searchBychannelname(args[1], channels, MAX_CHANNELS)].topic_mode == 1)
							{
								if (!isAdmin(channels[searchBychannelname(args[1], channels, MAX_CHANNELS)].admin_users, clients[i].username))
									throw std::runtime_error("Error: Topic mode is set!");
							}
							std::string msg = getMsg(message); // Extract the message text
							if (searchBychannelname(args[1], channels, MAX_CHANNELS) == -1)
								errorUser(args[1] + ": CHANNEL NOT FOUND", clientSocket);
							else
								channels[searchBychannelname(args[1], channels, MAX_CHANNELS)].topic = msg;
								std::string privmsgCommand = "New TOPIC is set: " + msg + "\r\n";
								sendUser(privmsgCommand, clients[i].socket, clients[i].nickname);
						}
						else //there is no msg, only show current topic
						{
							args[1] = args[1].substr(1);
							int ind = searchBychannelname(args[1], channels, MAX_CHANNELS);
							if (ind == -1)
								errorUser(args[1] + ": CHANNEL NOT FOUND", clients[i].socket);
							if (channels[searchBychannelname(args[1], channels, MAX_CHANNELS)].topic_mode == 1)
							{
								if (!isAdmin(channels[searchBychannelname(args[1], channels, MAX_CHANNELS)].admin_users, clients[i].username))
									throw std::runtime_error("Error: Topic mode is set!");
							}
							if (channels[ind].topic != "")
							{
								std::string privmsgCommand = "TOPIC is : " + channels[ind].topic + "\r\n";
								sendUser(privmsgCommand, clients[i].socket, clients[i].nickname);
							}
							else
								sendUser("Topic is not Set", clients[i].socket, clients[i].nickname);
					}
					}
					catch(std::runtime_error &e){
						std::string indexPrompt =  std::string(e.what());
						sendUser(indexPrompt, clients[i].socket, clients[i].nickname);
					}
				}
				else if (message.substr(0, 4) == "MODE")/// should check if the client is ADMIN
				{
					std::string channel = args[1].substr(1); //--> ex : "room"
					if (args[1][0] != '#')
					{
						errorUser("/MODE <#channel> <+/-><flag> [<param>]", clientSocket);
					}
					else if (searchBychannelname(channel, channels, MAX_CHANNELS) == -1)
						errorUser(channel + ": CHANNEL NOT FOUND", clientSocket); 
					else if (isAdmin(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users, clients[i].username)){
						std::string argsAndmsg = getMsg(message); //--> ex : "+i testmsg"
						std::string::size_type poss = argsAndmsg.find(" ");// ex : 2
						if (args.size() > 3)// The " " character was found in the string.
						{
							std::string arg = argsAndmsg.substr(0, poss); // Extract the args with the sign --> ex : "+i"
							std::string msg = argsAndmsg.substr(poss + 1); // Extract the message text --> ex : "testmsg"
							modeOptions(channels, clients, channel, arg, msg , i);
						}
						else // The " " character was not found in the string --> that mean there is no msg
						{
							std::string args = argsAndmsg.substr(0);
							modeNoOptions(channels, clients, channel, args, i);
						}
					}
					else{
						sendUser("You are not an ADMIN", clients[i].socket, clients[i].nickname);
					}
				}
				else if (message.substr(0, 6) == "INVITE")
				{
					args[1] = args[1].substr(1);
					if (searchBychannelname(args[1], channels, MAX_CHANNELS) != -1)
					{
						if (searchByUsername(args[2], clients, MAX_CLIENTS) != -1)
							inviteUser(clients[searchByUsername(args[2], clients, MAX_CLIENTS)].socket, channels, clients, args[1], i);
						else
							errorUser(args[2] + ": USER NOT FOUND", clientSocket);
					}
					else
						errorUser(args[1] + ": CHANNEL NOT FOUND", clientSocket);
				}
				else if (message.substr(0, 4) == "PART")
				{
					if (args[1][0] != '#')
					{
						errorUser("/PART <#channel>", clientSocket);
					}
					else
					{
						args[1] = args[1].substr(1);
						PartUser(channels, clients, args[1], clients[i].username, i);
					}
				}
				else if (message.substr(0, 5) == "WHOIS")
				{
					std::vector<std::string> args = split_str(message, ' ');
					// args[1] = message.substr(6);
					int cl_i = searchByUsername(args[1], clients, MAX_CLIENTS);
					if (cl_i != -1)
					{
						sendUser("UserName : " + clients[cl_i].username, clients[i].socket, clients[i].nickname);
						sendUser("NickName : " + clients[cl_i].nickname, clients[i].socket, clients[i].nickname);
						sendUser("Channels : ", clients[i].socket, clients[i].nickname);
						user_channels(channels, clients, cl_i, clients[i].socket);
						sendUser("------------------------------------------------------", clients[i].socket, clients[i].nickname);
					}
					else
						errorUser(args[1] + ": USER NOT FOUND", clientSocket);
				}
				else if (message.substr(0, 4) == "NICK")
					clients[i].nickname = args[1];
				else if (message.substr(0, 4) == "USER")
					{
						if(searchByUsername(args[1], clients, MAX_CLIENTS) == -1)
							clients[i].username = args[1];
						else
						{
							errorUser("UserName Already Exist", clients[i].socket);
							sendUser("Username Set is " + clients[i].username, clients[i].socket, clients[i].nickname);
						}
					}
				else if (message.substr(0, 4) == "LIST")
				{
					listChannels(channels, clients, i);
				}
				else if (message.substr(0, 4) == "QUIT")
				{
					close(clients[i].socket);
					clients[i].socket = -1;
					std::cout << "Client disconnected : "<< clients[i].username << std::endl;
				}
				else if (message.substr(0, 4) == "EXIT")
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
