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
				sendUser("User Already In This Channel", clients[i].socket, clients[i].nickname);
		}
	else
	{
		if (searchIfExist(channels[channel_index2].users_sockets, clientSocket))
		{
			sendUser("User Already In This Channel", clients[i].socket, clients[i].nickname);
		}
		std::string channelFullPrompt = "Channel " + channel + " is full\n";
		sendUser(channelFullPrompt, clients[i].socket, clients[i].nickname);
	}
}
