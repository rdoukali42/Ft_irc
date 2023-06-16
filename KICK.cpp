/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdoukali <rdoukali@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 01:32:47 by rdoukali          #+#    #+#             */
/*   Updated: 2023/06/16 01:53:07 by rdoukali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

void kickUser(Channel *channels, Client *clients, std::string channelname,std::string user, const int i)
{
	int ch_ind = searchBychannelname(channelname, channels, MAX_CHANNELS);////////////i don't know why this Fuc****g Function return -1 in this case
	int cl_ind = searchByUsername(user, clients, MAX_CLIENTS);
	if (ch_ind == -1)
		std::cout << channelname << " : Channel Not found " << std::endl;
	else if (cl_ind == -1)
		std::cout << user << " : Client Not found " << std::endl;
	else if (isAdmin(channels[ch_ind].admin_users, clients[i].username))
	{
		std::string kickPrompt = "USER : " + clients[cl_ind].username + " is Removed from [" + channels[ch_ind].name + " | " + channelname + "\n";
		send(clients[i].socket, kickPrompt.c_str(), kickPrompt.length(), 0);
		removeClient(channels[ch_ind].users_sockets, clients[cl_ind].socket);
	}
	else
	{
		std::cout << "channel admins :";// << channels[ch_ind].admin << ". username :" << clients[i].username << std::endl;
		for (std::vector<std::string>::const_iterator it = channels[ch_ind].admin_users.begin(); it != channels[ch_ind].admin_users.end(); ++it) {
			std::cout << *it << ", ";
		}
		std::cout << std::endl;
		// std::cout << "channel admin :" << channels[ch_ind].admin << ". username :" << clients[i].username << std::endl;
		std::string Prompt = "You're not Allowed to do this Action \n";
		send(clients[i].socket, Prompt.c_str(), Prompt.length(), 0);
	}
}
