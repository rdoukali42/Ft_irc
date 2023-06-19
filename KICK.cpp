/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdoukali <rdoukali@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 01:32:47 by rdoukali          #+#    #+#             */
/*   Updated: 2023/06/19 04:48:05 by rdoukali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

void removeClient(std::vector<int>& users_sockets, int clientSocket)
{
	std::vector<int>::iterator it = std::find(users_sockets.begin(), users_sockets.end(), clientSocket);
	if (it != users_sockets.end()) {
		users_sockets.erase(it);
	}
}

void removeAdmin(Channel *channels, Client *clients, int client_index, int channel_index)
{
	for (std::vector<int>::const_iterator it = channels[channel_index].users_sockets.begin(); it != channels[channel_index].users_sockets.end(); ++it) {
		if (*it != clients[client_index].socket) {
			if(isAdmin(channels[channel_index].admin_users, clients[searchBySocket(*it, clients, MAX_CLIENTS)].username) == 0)
			{
				if (numOfAdmins(channels, clients, channel_index) == 1)
				{
					channels[channel_index].admin_users.push_back(clients[searchBySocket(*it, clients, MAX_CLIENTS)].username);
					std::string channelFullPrompt = "You are now ADMIN in " + channels[channel_index].name +"\n";
					send(clients[searchBySocket(*it, clients, MAX_CLIENTS)].socket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
				}
				for (int i = 0 ; i < channels[channel_index].admin_users.size(); i++) {
					if (strcmp(channels[channel_index].admin_users[i].c_str(), clients[client_index].username.c_str()) == 0)
					{
						channels[channel_index].admin_users.erase(channels[channel_index].admin_users.begin() + i);
					}
					i++;
				}
				return ;
			}
		}
	}
}


void kickUser(Channel *channels, Client *clients, std::string channelname, std::string user, const int i)
{
	int ch_ind = searchBychannelname(channelname, channels, MAX_CHANNELS);////////////i don't know why this Fuc****g Function return -1 in this case
	int cl_ind = searchByUsername(user, clients, MAX_CLIENTS);
	if (ch_ind == -1)
		errorUser(channelname + ": CHANNEL NOT FOUND", clients[i].socket);
	else if (cl_ind == -1)
		errorUser(user + ": USER NOT FOUND", clients[i].socket);
	else if (isAdmin(channels[ch_ind].admin_users, clients[i].username))
	{
		std::string msgPrompt;
		if (user == clients[i].username)
			msgPrompt = "You Quit " + channels[ch_ind].name + "\n";
		else
			msgPrompt = "You've been Kicked OUT by " + clients[i].username + " From " + channels[ch_ind].name + "\n";
		send(clients[cl_ind].socket, msgPrompt.c_str(), msgPrompt.length(), 0);
		if(isAdmin(channels[ch_ind].admin_users, clients[cl_ind].username))
			removeAdmin(channels, clients, cl_ind, ch_ind);
		removeClient(channels[ch_ind].users_sockets, clients[cl_ind].socket);
		if (channels[ch_ind].users_sockets.size() == 0)
		{
			std::cout << "Channel " << channels[ch_ind].name << " Deleted" << std::endl;
			channels[ch_ind].name = "";
		}
	}
	else
	{
		std::string Prompt = "You're not Allowed to do this Action \n";
		send(clients[i].socket, Prompt.c_str(), Prompt.length(), 0);
	}
}
