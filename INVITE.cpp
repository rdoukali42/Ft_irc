/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   INVITE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdoukali <rdoukali@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:05:39 by rdoukali          #+#    #+#             */
/*   Updated: 2023/06/17 00:23:56 by rdoukali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

void inviteUser(const int clientSocket,Channel *channels,const Client *clients, std::string channel, const int i)
{
	int channel_index2 = searchBychannelname(channel, channels, MAX_CHANNELS);
	if (checkUserChannel(channels, clients, clients[i].username, channel, clientSocket) == 0)
		return ;
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
}