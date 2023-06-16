/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   INVITE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdoukali <rdoukali@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:05:39 by rdoukali          #+#    #+#             */
/*   Updated: 2023/06/16 03:17:55 by rdoukali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

void inviteUser(const int clientSocket,Channel *channels,const Client *clients, std::string channel, const int i)
{
	int channel_index2 = searchBychannelname(channel, channels, MAX_CHANNELS);
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
	for (std::size_t i = 0; i < channels[channel_index2].users_sockets.size(); ++i)
	{
		std::cout << "USER " + clients[searchBySocket(channels[channel_index2].users_sockets[i], clients, MAX_CLIENTS)].username << std::endl;
	}
}