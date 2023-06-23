#include "irc.hpp"

int checkKeyMode(Channel *channels,const Client *clients,std::string channel, int channel_index2, int i)
{
	if (channels[channel_index2].key_mode == 1 && searchIfExist(channels[channel_index2].users_sockets, clients[i].socket) == 0)
	{
		std::string passPrompt = "Please enter PassWord : ";
		send(clients[i].socket, passPrompt.c_str(), passPrompt.length(), 0);
		char passbuffer[MAX_BUFFER_SIZE];
		ssize_t bytesRead = read(clients[i].socket, passbuffer, sizeof(passbuffer));
		if (bytesRead > 0) {
			passbuffer[bytesRead - 1] = '\0';
		}
		if (strcmp(passbuffer, channels[channel_index2].password.c_str()) != 0)
		{
			std::string passPrompt2 = "Wrong Password !\n";
			send(clients[i].socket, passPrompt2.c_str(), passPrompt2.length(), 0);
			return -1;
		}
		else
			return 1;
	}
	return 0;
}

int channelExist(const int clientSocket,Channel *channels,const Client *clients, std::string channel, const int i)
{
	int channel_index2 = searchBychannelname(channel, channels, MAX_CHANNELS);
	if (checkKeyMode(channels, clients, channel, channel_index2, i) == -1)
		return 0;
	if (channels[channel_index2].invite_only == 1)
	{
		if (!searchIfExist(channels[channel_index2].users_sockets, clients[i].socket))
		{
			std::string channelFullPrompt = "invite only mode activated for this channel\n";
			send(clientSocket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
		}
		else
		{
			std::string channelFullPrompt = "User Already In This Channel\n";
			send(clientSocket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
		}
		return 0;
	}
	if (channels[channel_index2].users_sockets.size() < channels[channel_index2].limit || channels[channel_index2].limit_mode == 0)
		{
			if (!searchIfExist(channels[channel_index2].users_sockets, clients[i].socket))
			{
				channels[channel_index2].users_sockets.push_back(clients[i].socket);
				std::string channelFullPrompt = "Joined sucessfully!\n";
				send(clientSocket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
			}
			else
			{
				std::string channelFullPrompt = "User Already In This Channel\n";
				send(clientSocket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
			}
		}
	else
	{
		if (searchIfExist(channels[channel_index2].users_sockets, clients[i].socket))
		{
			std::string channelFullPrompt = "User Already In This Channel\n";
			send(clientSocket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
		}
		std::string channelFullPrompt = "Channel " + channel + " is full\n";
		send(clientSocket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
	}
	return (0);
}

void channelNotExist(const int clientSocket,Channel *channels,const Client *clients, std::string channel, const int i, int *channel_index)
{
	channels[*channel_index].name = channel;
	channels[*channel_index].admin_users.push_back(clients[i].username);
	channels[*channel_index].invite_only = 0;
	channels[*channel_index].key_mode = 0;
	channels[*channel_index].topic_mode = 1;
	channels[*channel_index].limit_mode = 1;
	channels[*channel_index].password = "";
	try{
		std::string TopicPrompt = "Please enter a TOPIC for the " + channel + " channel : ";
		send(clientSocket, TopicPrompt.c_str(), TopicPrompt.length(), 0);
		char TopicBuffer[MAX_BUFFER_SIZE];
		ssize_t bytesRead = read(clientSocket, TopicBuffer, sizeof(TopicBuffer));
		if (bytesRead > 0) {
			TopicBuffer[bytesRead - 1] = '\0';
			std::string Topic(TopicBuffer, bytesRead);
			Topic.erase(Topic.find_last_not_of(" \t\r\n") + 1);
			// Assign the topic to the channel
			if (Topic.size() <= 1 || !ifWord(Topic))
				throw std::runtime_error(" did not set channel topic!");
			channels[*channel_index].topic = Topic;
		}
		std::string limitPrompt = "Please enter a limit for the " + channel + " channel users : ";
		send(clientSocket, limitPrompt.c_str(), limitPrompt.length(), 0);
		char limitBuffer[MAX_BUFFER_SIZE];
		ssize_t bytesRead3 = read(clientSocket, limitBuffer, sizeof(limitBuffer));
		if (bytesRead3 > 0) {
			limitBuffer[bytesRead3 - 1] = '\0';
			char *end;
			errno = 0;
			long limit = (std::strtol(limitBuffer, &end, 10));
			if ((errno == ERANGE && (limit == LONG_MAX || limit == LONG_MIN)) || (errno != 0 && limit == 0)) 
				throw std::runtime_error(" limit is not valid!");
			if (end == limitBuffer || *end != '\0')
				throw std::runtime_error(" limit is not a number!");
			if (*end == '\0')
				channels[*channel_index].limit= static_cast<int>(limit);
			else
				throw std::runtime_error(" limit is not a number!");
			// Assign the topic to the channel
		}
		std::string indexPrompt = "Do you want to Allow Private Msg in " + channel + " channel (yes/no) : ";
		send(clientSocket, indexPrompt.c_str(), indexPrompt.length(), 0);
		char indexBuffer[MAX_BUFFER_SIZE];
		ssize_t bytesRead4 = read(clientSocket, indexBuffer, sizeof(indexBuffer));
		if (bytesRead4 > 0) {
			indexBuffer[bytesRead4 - 1] = '\0';
			std::string indexx(indexBuffer, bytesRead);
			if (strcmp(indexx.c_str(), "yes") == 0 || strcmp(indexx.c_str(), "no") == 0 )
			{
				// Assign the index to the channel
				channels[*channel_index].PRVIMSG_Index = indexx;
				channels[*channel_index].index = *channel_index; // using index to check if the channel is created or not (if 0 ....)
				if (channels[*channel_index].limit != 0)
				{
					channels[*channel_index].users_sockets.push_back(clients[i].socket);
					std::string iPrompt = "Channel " + channel + " Created\n";
					send(clientSocket, iPrompt.c_str(), iPrompt.length(), 0);
				}
				else
				{
					std::string iPrompt = "Channel " + channel + " Created, BUT no one can JOIN\n";
					send(clientSocket, iPrompt.c_str(), iPrompt.length(), 0);
				}
				*channel_index += 1;
			}
			else
				throw std::runtime_error(" incorrect private message setting!");
		}
	}
	catch (std::runtime_error &e){
		std::string indexPrompt = "Input error:" + std::string(e.what()) + " Channel not saved\n";
		send(clientSocket, indexPrompt.c_str(), indexPrompt.length(), 0);
		channels[*channel_index].topic = "";
		channels[*channel_index].name = "";
	}
}



// #include "irc.hpp"

// int checkKeyMode(Channel *channels,const Client *clients,std::string channel, int channel_index2, int i)
// {
// 	if (channels[channel_index2].key_mode == 1 && searchIfExist(channels[channel_index2].users_sockets, clients[i].socket) == 0)
// 	{
// 		std::string passPrompt = "Please enter PassWord : ";
// 		send(clients[i].socket, passPrompt.c_str(), passPrompt.length(), 0);
// 		char passbuffer[MAX_BUFFER_SIZE];
// 		ssize_t bytesRead = read(clients[i].socket, passbuffer, sizeof(passbuffer));
// 		if (bytesRead > 0) {
// 			passbuffer[bytesRead - 1] = '\0';
// 		}
// 		if (strcmp(passbuffer, channels[channel_index2].password.c_str()) != 0)
// 		{
// 			std::string passPrompt2 = "Wrong Password !\n";
// 			send(clients[i].socket, passPrompt2.c_str(), passPrompt2.length(), 0);
// 			return -1;
// 		}
// 		else
// 			return 1;
// 	}
// 	return 0;
// }

// int channelExist(const int clientSocket,Channel *channels,const Client *clients, std::string channel, const int i)
// {
// 	int channel_index2 = searchBychannelname(channel, channels, MAX_CHANNELS);
// 	if (checkKeyMode(channels, clients, channel, channel_index2, i) == -1)
// 		return 0;
// 	if (channels[channel_index2].invite_only == 1)
// 	{
// 		if (!searchIfExist(channels[channel_index2].users_sockets, clients[i].socket))
// 		{
// 			std::string channelFullPrompt = "MODE INVITE_ONLY ACTIVER\n";
// 			send(clientSocket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
// 		}
// 		else
// 		{
// 			std::string channelFullPrompt = "User Already In This Channel\n";
// 			send(clientSocket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
// 		}
// 		return 0;
// 	}
// 	if (channels[channel_index2].users_sockets.size() < channels[channel_index2].limit || channels[channel_index2].limit_mode == 0)
// 		{
// 			if (!searchIfExist(channels[channel_index2].users_sockets, clients[i].socket))
// 			{
// 				channels[channel_index2].users_sockets.push_back(clients[i].socket);
// 				std::string channelFullPrompt = "Joined sucessfully!\n";
// 				send(clientSocket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
// 			}
// 			else
// 			{
// 				std::string channelFullPrompt = "User Already In This Channel\n";
// 				send(clientSocket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
// 			}
// 		}
// 	else
// 	{
// 		if (searchIfExist(channels[channel_index2].users_sockets, clients[i].socket))
// 		{
// 			std::string channelFullPrompt = "User Already In This Channel\n";
// 			send(clientSocket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
// 		}
// 		std::string channelFullPrompt = "Channel " + channel + " is full\n";
// 		send(clientSocket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
// 	}
// 	return (0);
// }

// void channelNotExist(const int clientSocket,Channel *channels,const Client *clients, std::string channel, const int i, int *channel_index)
// {
// 	channels[*channel_index].name = channel;
// 	channels[*channel_index].admin_users.push_back(clients[i].username);
// 	channels[*channel_index].invite_only = 0;
// 	channels[*channel_index].key_mode = 0;
// 	channels[*channel_index].limit_mode = 1;
// 	channels[*channel_index].password = "";
// 	std::string TopicPrompt = "Please enter a TOPIC for the " + channel + " channel : ";
// 	send(clientSocket, TopicPrompt.c_str(), TopicPrompt.length(), 0);
// 	char TopicBuffer[MAX_BUFFER_SIZE];
// 	ssize_t bytesRead = 0;
// 	while(1){
// 	bytesRead = read(clientSocket, TopicBuffer, sizeof(TopicBuffer));
// 	if (bytesRead > 0) {
// 		TopicBuffer[bytesRead - 1] = '\0';
// 		std::string Topic(TopicBuffer, bytesRead);
// 		Topic.erase(Topic.find_last_not_of(" \t\r\n") + 1);
// 		if (ifWord(Topic)){
// 		// Assign the topic to the channel
// 		channels[*channel_index].topic = Topic;
// 		break;
// 		}
// 	}
// 	}
// 	std::string limitPrompt = "Please enter a limit for the " + channel + " channel users : ";
// 	send(clientSocket, limitPrompt.c_str(), limitPrompt.length(), 0);
// 	char limitBuffer[MAX_BUFFER_SIZE];
// 	ssize_t bytesRead3 = read(clientSocket, limitBuffer, sizeof(limitBuffer));
// 	if (bytesRead3 > 0) {
// 		limitBuffer[bytesRead3 - 1] = '\0';
// 		int limit = std::stoi(limitBuffer);
		// Assign the topic to the channel
	// 	channels[*channel_index].limit= limit;
	// }
	// std::string indexPrompt = "Do you want to Allow Private Msg in " + channel + " channel (yes/no) : ";
	// send(clientSocket, indexPrompt.c_str(), indexPrompt.length(), 0);
	// char indexBuffer[MAX_BUFFER_SIZE];
	// ssize_t bytesRead4 = read(clientSocket, indexBuffer, sizeof(indexBuffer));
	// if (bytesRead4 > 0) {
	// 	indexBuffer[bytesRead4 - 1] = '\0';
	// 	std::string indexx(indexBuffer, bytesRead);
	// 	if (indexx.substr(0, 3) == "yes" || indexx.substr(0, 2) == "no")
	// 	{
	// 		// Assign the index to the channel
	// 		channels[*channel_index].PRVIMSG_Index = indexx;
	// 		channels[*channel_index].index = *channel_index; // using index to check if the channel is created or not (if 0 ....)
	// 		if (channels[*channel_index].limit != 0)
	// 		{
	// 			channels[*channel_index].users_sockets.push_back(clients[i].socket);
	// 			std::string iPrompt = "Channel " + channel + " Created\n";
	// 			send(clientSocket, iPrompt.c_str(), iPrompt.length(), 0);
	// 		}
	// 		else
	// 		{
	// 			std::string iPrompt = "Channel " + channel + " Created, BUT no one can JOIN\n";
	// 			send(clientSocket, iPrompt.c_str(), iPrompt.length(), 0);
	// 		}
	// 		*channel_index += 1;
	// 	}
	// 	else
// 		{
// 			std::string indexPrompt = "Wrong INPUT !! Channel not saved\n";
// 			send(clientSocket, indexPrompt.c_str(), indexPrompt.length(), 0);
// 			channels[*channel_index].topic = "";
// 			channels[*channel_index].name = "";
// 		}
// 	}
// }

