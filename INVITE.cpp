#include "irc.hpp"

void inviteUser(const int clientSocket,Channel *channels,const Client *clients, std::string channel, const int i)
{
	int channel_index2 = searchBychannelname(channel, channels, MAX_CHANNELS);
	if (channels[channel_index2].users_sockets.size() < channels[channel_index2].limit || channels[channel_index2].limit_mode == 0)
		{
			if (!searchIfExist(channels[channel_index2].users_sockets, clientSocket))
			{
				channels[channel_index2].users_sockets.push_back(clientSocket);
				sendUser("You've been invited to channel " + channels[channel_index2].name, clientSocket, clients[i].nickname);
			}
			else
			{
				std::string channelFullPrompt = "User Already In This Channel\n";
				send(clients[i].socket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
			}
		}
	else
	{
		if (searchIfExist(channels[channel_index2].users_sockets, clientSocket))
		{
			std::string channelFullPrompt = "User Already In This Channel\n";
			send(clients[i].socket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
		}
		std::string channelFullPrompt = "Channel " + channel + " is full\n";
		send(clients[i].socket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
	}
}
