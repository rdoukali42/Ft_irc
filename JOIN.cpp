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
	if (channels[channel_index2].invite_only == 1)
	{
		if (!searchIfExist(channels[channel_index2].users_sockets, clients[i].socket))
		{
			sendUser("invite only mode activated for this channel", clientSocket, clients[i].nickname);
		}
		else
		{
			sendUser("User Already In This Channel", clientSocket, clients[i].nickname);
		}
		return 0;
	}
	if (channels[channel_index2].users_sockets.size() < channels[channel_index2].limit || channels[channel_index2].limit_mode == 0)
		{
			if (!searchIfExist(channels[channel_index2].users_sockets, clients[i].socket))
			{
				channels[channel_index2].users_sockets.push_back(clients[i].socket);
				sendUser("Joined sucessfully!", clientSocket, clients[i].nickname);
			}
			else
			{
				sendUser("User Already In This Channel", clientSocket, clients[i].nickname);
			}
		}
	else
	{
		if (searchIfExist(channels[channel_index2].users_sockets, clients[i].socket))
		{
			sendUser("User Already In This Channel", clientSocket, clients[i].nickname);
		}
		std::string channelFullPrompt = "Channel " + channel + " is full\n";
		sendUser(channelFullPrompt, clientSocket, clients[i].nickname);
	}
	return (0);
}

void channelNotExist(const int clientSocket,Channel *channels,const Client *clients, std::string channel, const int i, int *channel_index)
{
	channels[*channel_index].name = channel;
	channels[*channel_index].admin_users.push_back(clients[i].username);
	channels[*channel_index].users_sockets.push_back(clients[i].socket);
	channels[*channel_index].invite_only = 0;
	channels[*channel_index].key_mode = 0;
	channels[*channel_index].topic_mode = 0;
	channels[*channel_index].limit_mode = 0;
	channels[*channel_index].password = "";
	channels[*channel_index].topic = "";
	channels[*channel_index].PRVIMSG_Index = "yes";
	*channel_index += 1;
	std::string msg = "Channel " + channel + " Created";
	sendUser(msg, clients[i].socket, clients[i].nickname);
}
