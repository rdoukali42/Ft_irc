/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdoukali <rdoukali@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 02:47:34 by rdoukali          #+#    #+#             */
/*   Updated: 2023/06/16 03:27:08 by rdoukali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

void modeOptions(Channel *channels,const Client *clients, std::string channel,std::string args,std::string msg, const int i)
{
	if (args == "+l")
	{
		int new_limit = std::stoi(msg);
		std::cout << "new_limit is :: " << new_limit << "|" << std::endl;
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].limit_mode = 1;
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].limit = new_limit;
	}
	else if ( args == "+o")
	{
		if (searchByUsername(msg, clients, MAX_CLIENTS) != -1 && !isAdmin(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users, msg))//check if client username exists && if he's already an admin. 
			channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users.push_back(msg);
	}
	else if ( args == "-o")
	{
		if (searchByUsername(msg, clients, MAX_CLIENTS) != -1 && isAdmin(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users, msg))//check if client username exists && if he's already an admin. 
		{
			for (int i = 0 ; i < channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users.size(); i++) {
			
				if (strcmp(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users[i].c_str(), msg.c_str()) == 0)
				{
					std::cout << "found it : " << channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users[i] + "." + msg + "." << std::endl;
					channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users.erase(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users.begin() + i);
				}
				i++;
			}
		}
	}
	if (args == "+k")
	{
		std::cout << "new_Password is :: " << msg << "|" << std::endl;
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].key_mode = 1;
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].password = msg;
	}
}

void modeNoOptions(Channel *channels,const Client *clients, std::string channel,std::string args, const int i)
{
	args.erase(args.find_last_not_of(" \t\r\n") + 1);// Remove trailing whitespace characters
	std::cout << args << std::endl;
	if (args == "-l")
	{
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].limit_mode = 0;
	}
	if (args == "-k")
	{
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].key_mode = 0;
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].password = "";
	}
	if (args == "-i")
	{
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].invite_only = 0;
	}
	if (args == "+i")
	{
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].invite_only = 1;
	}
 }