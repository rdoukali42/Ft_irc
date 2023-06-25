#include "irc.hpp"

Channel channels[MAX_CHANNELS];
Client clients[MAX_CLIENTS];

void init1(int serverSocket, struct sockaddr_in serverAddress, const int port)
{
	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
		error("Failed to bind socket");
	if (listen(serverSocket, SOMAXCONN) == -1)
		error("Failed to listen for connections");
	std::cout << "Server is listening on port " << port << std::endl;
	// char buffer[MAX_BUFFER_SIZE];
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		clients[i].socket = -1;
		clients[i].indice = 0;
	}
}

void init2(int clientSocket, char *passbuffer, std::string password)
{
	for (int i = 3; i >= 0; i--)
	{
		if (i == 3)
		{
			std::string passPrompt = "Please enter PassWord : ";
			send(clientSocket, passPrompt.c_str(), passPrompt.length(), 0);
			ssize_t bytesRead = read(clientSocket, passbuffer, sizeof(passbuffer));
			if (bytesRead > 0)
			{
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
			if (bytesRead > 0)
			{
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
			if (bytesWritten < 0)
			{
				error("Sending data failed");
			}
			close(clientSocket);
		}
	}
}

void init3(int clientSocket, char *passbuffer, std::string password, int index)
{
	ssize_t bytesRead;
	while(1)
	{
		std::string usernamePrompt = "Please enter your username: ";
		send(clientSocket, usernamePrompt.c_str(), usernamePrompt.length(), 0);
		char usernameBuffer[MAX_BUFFER_SIZE];
		bytesRead = read(clientSocket, usernameBuffer, sizeof(usernameBuffer));
		if (bytesRead > 0)
		{
			std::string username(usernameBuffer, bytesRead);
			for (int i = 0; i < username.length(); i++)
			{
				username.erase(username.find_last_not_of(" \t\r\n") + 1);
			}
			std::string::size_type pos = username.find(" ");
			if (pos != std::string::npos)// The " " character was found in the string.
				sendUser("UserName should be in one word", clientSocket);
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
}

void init4(int i, std::vector<std::string> args, std::string message, int clientSocket)
{
// else if (message.substr(0, 8) == "/PRIVMSG")
// {
	int tmp = 0;
	if (args[1][0] == '#')
		tmp = 1;
	// Form the PRIVMSG command to be sent to the server
	std::string privmsgCommand = clients[i].username + " : " + getMsg(message) + "\n";					
	if (tmp == 0)
	{
		int mem = searchByUsername(args[1], clients, MAX_CLIENTS);// Send the PRIVMSG command to the client
		if (mem == -1)
			errorUser(args[1] + ": USER NOT FOUND", clientSocket);
		else
		{
			ssize_t bytesWritten = send(clients[mem].socket, privmsgCommand.c_str(), privmsgCommand.length(), 0);
			if (bytesWritten < 0)
				error("Sending data failed");
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
			for (std::size_t i = 0; i < channels[ind].users_sockets.size(); ++i)
			{
				ssize_t bytesWritten = send(channels[ind].users_sockets[i], privmsgCommand.c_str(), privmsgCommand.length(), 0);
				if (bytesWritten < 0)
					error("Sending data failed");
			}
		}
		else
		{
			std::string msgPrompt = "No permission To send PRIVMSG in this Channel \n";
			send(clients[i].socket, msgPrompt.c_str(), msgPrompt.length(), 0);
		}
	}
}

void init5(int i, std::vector<std::string> args, std::string message, int clientSocket, int channel_index)
{
	if (args[1][0] != '#')
		errorUser("/JOIN <#channel>", clientSocket);
	else{
		args[1] = args[1].substr(1);
		if (searchBychannelname(args[1], channels, MAX_CHANNELS) != -1)//Check if the channel exist
			channelExist(clientSocket, channels, clients, args[1], i);
		else//if the channel not exist
			channelNotExist(clientSocket, channels, clients, args[1], i, &channel_index);
	}
}

void init6(int i, std::vector<std::string> args, std::string message, int clientSocket)
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
	catch(std::runtime_error &e)
	{
		std::string kickErrorPrompt =  std::string(e.what()) + "\n";
		send(clientSocket, kickErrorPrompt.c_str(), kickErrorPrompt.length(), 0);
	}
}

void init7(int i, std::vector<std::string> args, std::string channelAndmsg, std::string message, int clientSocket)
{
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
				errorUser("CHANNEL NOT FOUND", clientSocket);
			else
				channels[searchBychannelname(args[1], channels, MAX_CHANNELS)].topic = msg;
				std::string privmsgCommand = "New TOPIC is set: " + msg + "\r\n";
				ssize_t bytesWritten = send(clients[i].socket, privmsgCommand.c_str(), privmsgCommand.length(), 0);
				if (bytesWritten < 0)
					error("Sending data failed");
		}
		else //there is no msg, only show current topic
		{
			args[1] = args[1].substr(1);
			int ind = searchBychannelname(args[1], channels, MAX_CHANNELS);
			if (ind == -1)
				errorUser("CHANNEL NOT FOUND", clients[i].socket);
			if (channels[searchBychannelname(args[1], channels, MAX_CHANNELS)].topic_mode == 1)
			{
				if (!isAdmin(channels[searchBychannelname(args[1], channels, MAX_CHANNELS)].admin_users, clients[i].username))
					throw std::runtime_error("Error: Topic mode is set!");
			}
			std::string privmsgCommand = "TOPIC is : " + channels[ind].topic + "\r\n";
			ssize_t bytesWritten = send(clients[i].socket, privmsgCommand.c_str(), privmsgCommand.length(), 0);
			if (bytesWritten < 0)
				error("Sending data failed");
	}
	}
	catch(std::runtime_error &e)
	{
		std::string indexPrompt =  std::string(e.what()) + "\n";
		send(clientSocket, indexPrompt.c_str(), indexPrompt.length(), 0);
	}
}

void init8(int i, std::vector<std::string> args, std::string message, std::string channel)
{
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

void init9(int i, std::vector<std::string> args, std::string message, int clientSocket)
{
	std::string channel = args[1].substr(1); //--> ex : "room"
	if (args[1][0] != '#')
		errorUser("/MODE <#channel> <+/-><flag> [<param>]", clientSocket);
	else if (searchBychannelname(channel, channels, MAX_CHANNELS) == -1)
		errorUser("CHANNEL NOT FOUND", clientSocket); 
	else if (isAdmin(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users, clients[i].username))
		init8(i, args, message, channel);
	else
	{
		std::string kickPrompt = "You are not an ADMIN \n";
		send(clients[i].socket, kickPrompt.c_str(), kickPrompt.length(), 0);
	}
}

void init10(int i, std::vector<std::string> args, int clientSocket)
{
	args[1] = args[1].substr(1);
	if (searchBychannelname(args[1], channels, MAX_CHANNELS) != -1)
	{
		if (searchByUsername(args[2], clients, MAX_CLIENTS) != -1)
			inviteUser(clients[searchByUsername(args[2], clients, MAX_CLIENTS)].socket, channels, clients, args[1], i);
		else
			errorUser("USER NOT FOUND", clientSocket);
	}
	else
		errorUser("CHANNEL NOT FOUND", clientSocket);
}

void init11(int i, std::vector<std::string> args, int clientSocket)
{
	if (args[1][0] != '#')
		errorUser("/PART <#channel>", clientSocket);
	else
	{
		args[1] = args[1].substr(1);
		PartUser(channels, clients, args[1], clients[i].username, i);
	}
}

void init12(int i, std::vector<std::string> args, std::string message, int clientSocket)
{
	args[1] = message.substr(7);
	int cl_i = searchByUsername(args[1], clients, MAX_CLIENTS);
	if (cl_i != -1)
	{
		sendUser("UserName : " + clients[cl_i].username, clients[i].socket);
		sendUser("NickName : " + clients[cl_i].nickname, clients[i].socket);
		sendUser("Channels : ", clients[i].socket);
		user_channels(channels, clients, cl_i, clients[i].socket);
		sendUser("------------------------------------------------------", clients[i].socket);
	}
	else
		errorUser("USER NOT FOUND", clientSocket);
}

int init13(int i, int serverSocket, std::vector<std::string> args, std::string message, int clientSocket, int channel_index)
{
	if (checkArg(message, clientSocket) == -1)
		;
	else if (message.substr(0, 8) == "/PRIVMSG")
		init4(i, args, message, clientSocket);
	else if (message.substr(0, 5) == "/JOIN")
		init5(i, args, message, clientSocket, channel_index);
	else if (message.substr(0, 5) == "/KICK")
		init6(i, args, message, clientSocket);
	else if (message.substr(0, 6) == "/TOPIC")
	{
		std::string channelAndmsg = message.substr(7); // Remove the command prefix and space
		std::string::size_type pos = channelAndmsg.find(" ");
		init7(i, args, channelAndmsg, message, clientSocket);
	}
	else if (message.substr(0, 5) == "/MODE")/// should check if the client is ADMIN
		init9(i, args, message, clientSocket);
	else if (message.substr(0, 7) == "/INVITE")
		init10(i, args, clientSocket);
	else if (message.substr(0, 5) == "/PART")
		init11(i, args, clientSocket);
	else if (message.substr(0, 6) == "/WHOIS")
		init12(i, args, message, clientSocket);
	else if (message.substr(0, 5) == "/NICK")
		clients[i].nickname = args[1];
	else if (message.substr(0, 5) == "/LIST")
		listChannels(channels, clients, i);
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
void init14(int serverSocket, char *buffer, std::string message, int clientSocket, int channel_index)
{
	fd_set readFds;
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		int clientSocket = clients[i].socket;
		if (clientSocket != -1 && FD_ISSET(clientSocket, &readFds))
		{
			// Read data from the client socket
			ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
			if (bytesRead < 0)
				error("Reading data failed");
			else if (bytesRead == 0)
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
				erase_spaces(message);
				std::vector<std::string> args = split_str(message, ' ');
				if (checkArg(message, clientSocket) == -1)
					continue ;
				init13(i, serverSocket, args, message, clientSocket, channel_index);
			}
		}
	}
}
void init15(std::string password, int maxFd, char *buffer, int clientSocket)
{
	if (clientSocket == -1)
		error("Failed to accept connection");
	int index = -1;
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clients[i].socket == -1)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
	{
		close(clientSocket);
		std::cout << "Rejected new connection: Too many clients" << std::endl;
	}
	else
	{
		char passbuffer[MAX_BUFFER_SIZE];
		init2(clientSocket, passbuffer, password);
		if (password == passbuffer)
			init3(clientSocket, passbuffer, password, index);
		// Update the maxFd value
		maxFd = std::max(maxFd, clientSocket);
	}
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
	char buffer[MAX_BUFFER_SIZE];
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
		error("Failed to create socket");

	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);
	init1(serverSocket, serverAddress, port);
	fd_set readFds;
	int maxFd = serverSocket;
	int channel_index = 0;

	while (true)
	{
		FD_ZERO(&readFds);
		FD_SET(serverSocket, &readFds);

		for (int i = 0; i < MAX_CLIENTS; ++i)
		{
			if (clients[i].socket != -1) 
			{
				FD_SET(clients[i].socket, &readFds);
				maxFd = std::max(maxFd, clients[i].socket);
			}
		}

		int numReady = select(maxFd + 1, &readFds, nullptr, nullptr, nullptr);
		if (numReady == -1)
			error("select() failed");
		if (FD_ISSET(serverSocket, &readFds))
		{
			struct sockaddr_in clientAddress;
			socklen_t clientAddressLength = sizeof(clientAddress);
			int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
			init15(password, maxFd, buffer, clientSocket);
		}

		// Check if there is data to be read from the client sockets
		int clientSocket;
		std::string message;
		init14(serverSocket, buffer, message, clientSocket, channel_index);
	}
	// Close the server socket
	close(serverSocket);
	return 0;
}