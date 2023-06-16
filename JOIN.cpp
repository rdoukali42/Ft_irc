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
		std::string channelFullPrompt = "MODE INVITE_ONLY ACTIVER\n";
		send(clientSocket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
		return 0;
	}
	if (channels[channel_index2].users_sockets.size() < channels[channel_index2].limit || channels[channel_index2].limit_mode == 0)
		{
			if (!searchIfExist(channels[channel_index2].users_sockets, clients[i].socket))
			{
				channels[channel_index2].users_sockets.push_back(clients[i].socket);
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
	std::cout << "Channel Name is : "<< channels[channel_index2].name <<
	"\nTopic : " << channels[channel_index2].topic <<
	"\nLimit is : " + std::to_string(channels[channel_index2].limit) <<
	"\nAllowed Private Msg : " << channels[channel_index2].PRVIMSG_Index <<
	"\nchannel admins :";
	for (std::vector<std::string>::const_iterator it = channels[channel_index2].admin_users.begin(); it != channels[channel_index2].admin_users.end(); ++it) {
		std::cout << *it << ", ";
	}
	std::cout << std::endl;
	// send(clientSocket, roomPrompt.c_str(), roomPrompt.length(), 0);
	for (std::size_t i = 0; i < channels[channel_index2].users_sockets.size(); ++i)
	{
		std::cout << "USER " + clients[searchBySocket(channels[channel_index2].users_sockets[i], clients, MAX_CLIENTS)].username << std::endl;
	}
	std::cout << "number of users in channel:" << channels[channel_index2].users_sockets.size() << std::endl;
	std::cout << "users_sockets.size:" <<  channels[channel_index2].users_sockets.size() << ". limit:" << channels[channel_index2].limit << std::endl;
	return (0);
}

void channelNotExist(const int clientSocket,Channel *channels,const Client *clients, std::string channel, const int i, int *channel_index)
{
	channels[*channel_index].name = channel;
	channels[*channel_index].admin_users.push_back(clients[i].username);
	// channels[*channel_index].admin = clients[i].username;
	channels[*channel_index].invite_only = 0;
	channels[*channel_index].key_mode = 0;
	channels[*channel_index].limit_mode = 1;
	channels[*channel_index].password = "";
	std::string TopicPrompt = "Please enter a TOPIC for the " + channel + " channel : ";
	send(clientSocket, TopicPrompt.c_str(), TopicPrompt.length(), 0);
	char TopicBuffer[MAX_BUFFER_SIZE];
	ssize_t bytesRead = read(clientSocket, TopicBuffer, sizeof(TopicBuffer));
	if (bytesRead > 0) {
		TopicBuffer[bytesRead - 1] = '\0';
		std::string Topic(TopicBuffer, bytesRead);
		Topic.erase(Topic.find_last_not_of(" \t\r\n") + 1);
		// Assign the topic to the channel
		channels[*channel_index].topic = Topic;
	}
	std::string limitPrompt = "Please enter a limit for the " + channel + " channel users : ";
	send(clientSocket, limitPrompt.c_str(), limitPrompt.length(), 0);
	char limitBuffer[MAX_BUFFER_SIZE];
	ssize_t bytesRead3 = read(clientSocket, limitBuffer, sizeof(limitBuffer));
	if (bytesRead3 > 0) {
		limitBuffer[bytesRead3 - 1] = '\0';
		int limit = std::stoi(limitBuffer);
		// Assign the topic to the channel
		channels[*channel_index].limit= limit;
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
			channels[*channel_index].PRVIMSG_Index = indexx;
			channels[*channel_index].index = *channel_index; // using index to check if the channel is created or not (if 0 ....)
			if (channels[*channel_index].limit != 0)
			{
				std::cout << "pushed " << clients[i].username << std::endl;
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
		{
			std::string indexPrompt = "Wrong INPUT !! Channel not saved\n";
			send(clientSocket, indexPrompt.c_str(), indexPrompt.length(), 0);
			channels[*channel_index].topic = "";
			channels[*channel_index].name = "";
		}
	}
}

