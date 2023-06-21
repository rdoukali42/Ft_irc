/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adinari <adinari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 02:47:34 by rdoukali          #+#    #+#             */
/*   Updated: 2023/06/20 01:57:34 by adinari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

void modeOptions(Channel *channels,const Client *clients, std::string channel,std::string args,std::string msg, const int i)
{
	if (args == "+l")
	{
		int new_limit;// = std::stoi(msg);

		try{
			char *end;
			errno = 0;
			long limit = (std::strtol(msg.c_str(), &end, 10));
			if ((errno == ERANGE && (limit == LONG_MAX || limit == LONG_MIN)) || (errno != 0 && limit == 0)) 
				throw std::runtime_error("new limit is invalid!");
			if (end == msg.c_str() || *end != '\0')
				throw std::runtime_error("new limit is not a number!");
			if (*end == '\0')
				new_limit = static_cast<int>(limit);
			else
				throw std::runtime_error("new limit is not a number!");
			std::cout << "new_limit is :: " << new_limit << "|" << std::endl;
			channels[searchBychannelname(channel, channels, MAX_CHANNELS)].limit_mode = 1;
			channels[searchBychannelname(channel, channels, MAX_CHANNELS)].limit = new_limit;
		}
		catch (std::runtime_error &e){
			std::string limitErrorPrompt = "Error: " + std::string(e.what()) +"\n";
			send(clients[i].socket, limitErrorPrompt.c_str(), limitErrorPrompt.length(), 0);//<<
		}
	}
	else if ( args == "+o")
	{
		if (searchByUsername(msg, clients, MAX_CLIENTS) != -1 && !isAdmin(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users, msg))//check if client username exists && if he's already an admin. 
		{
			channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users.push_back(msg);
			std::string channelFullPrompt = "You are now ADMIN in " + channels[searchBychannelname(channel, channels, MAX_CHANNELS)].name +"\n";
			send(clients[searchByUsername(msg, clients, MAX_CLIENTS)].socket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
		}
	}
	else if ( args == "-o")
	{
		if (searchByUsername(msg, clients, MAX_CLIENTS) != -1 && isAdmin(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users, msg))//check if client username exists && if he's already an admin. 
		{
			for (int i = 0 ; i < channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users.size(); i++) {
			
				if (strcmp(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users[i].c_str(), msg.c_str()) == 0)
				{
					channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users.erase(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users.begin() + i);
				}
				i++;
			}
		}
	}
	else if (args == "+k")
	{
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].key_mode = 1;
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].password = msg;
	}
	else
		errorUser("/MODE <#channel> <+l|+k|+o|-o> <limit/user/passwsord>", clients[i].socket);
}

void modeNoOptions(Channel *channels,const Client *clients, std::string channel,std::string args, const int i)
{
	args.erase(args.find_last_not_of(" \t\r\n") + 1);// Remove trailing whitespace characters
	std::cout << args << std::endl;
	if (args == "-l")
	{
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].limit_mode = 0;
	}
	else if (args == "-k")
	{
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].key_mode = 0;
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].password = "";
	}
	else if (args == "-i")
	{
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].invite_only = 0;
	}
	else if (args == "+i")
	{
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].invite_only = 1;
	}
	else if (args == "-t")
	{
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].topic_mode = 0;
		std::string limitPrompt = "Topic mode is Unset";
		send(clients[i].socket, limitPrompt.c_str(), limitPrompt.length(), 0);;
	}
	else if (args == "+t")
	{
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].topic_mode = 1;
		std::string limitPrompt = "Topic mode is Set";
		send(clients[i].socket, limitPrompt.c_str(), limitPrompt.length(), 0);
	}
	else
		errorUser("/MODE <#channel> <-t|+t|+i|-i|-l|-k>", clients[i].socket);
 }