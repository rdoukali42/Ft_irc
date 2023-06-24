/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdoukali <rdoukali@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 02:47:34 by rdoukali          #+#    #+#             */
/*   Updated: 2023/06/23 00:18:21 by rdoukali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"
void modeOptions(Channel *channels, Client *clients, std::string channel,std::string args,std::string msg, const int i)
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
			send(clients[i].socket, limitErrorPrompt.c_str(), limitErrorPrompt.length(), 0);
		}
	}
	else if ( args == "+o")
	{
		if (searchByUsername(msg, clients, MAX_CLIENTS) != -1 && isAdmin(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users, clients[i].username))//check if client username exists && if he's already an admin. 
		{
			if (searchIfExist(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].users_sockets, clients[searchByUsername(msg, clients, MAX_CLIENTS)].socket) && isAdmin(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users, msg))
				errorUser("User is Already an Admin", clients[i].socket);
			else if (!searchIfExist(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].users_sockets, clients[searchByUsername(msg, clients, MAX_CLIENTS)].socket))
				errorUser("User doesn't Exist in the Channel", clients[i].socket);
			else
			{
				channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users.push_back(msg);
				std::string channelFullPrompt = "You are now ADMIN in " + channels[searchBychannelname(channel, channels, MAX_CHANNELS)].name +"\n";
				send(clients[searchByUsername(msg, clients, MAX_CLIENTS)].socket, channelFullPrompt.c_str(), channelFullPrompt.length(), 0);
			}
		}
		else
			errorUser("User doesn't Exist in the Channel", clients[i].socket);
	}
	else if ( args == "-o")
	{
		std::cout << searchByUsername(msg, clients, MAX_CLIENTS) << " | " << isAdmin(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users, clients[i].username) << std::endl;//check if client username exists && if he's already an admin. 
		if (searchByUsername(msg, clients, MAX_CLIENTS) != -1 && isAdmin(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users, clients[i].username))//check if client username exists && if he's already an admin. 
		{
			if(clients[i].username != msg && numOfAdmins(channels, clients, searchBychannelname(channel, channels, MAX_CHANNELS)) > 1)
			{
				for (int i = 0 ; i < channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users.size(); i++) {
				
					if (strcmp(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users[i].c_str(), msg.c_str()) == 0)
					{
						channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users.erase(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users.begin() + i);
					}
				}
			}
			else if (!searchIfExist(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].users_sockets, clients[searchByUsername(msg, clients, MAX_CLIENTS)].socket))
				errorUser("User doesn't Exist in the Channel", clients[i].socket);
			else if (searchIfExist(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].users_sockets, clients[searchByUsername(msg, clients, MAX_CLIENTS)].socket) && !isAdmin(channels[searchBychannelname(channel, channels, MAX_CHANNELS)].admin_users, msg))
				errorUser("User is Not an Admin", clients[i].socket);
			else
				errorUser("This User is the only Admin in this Channel", clients[i].socket);
		}
		else if (searchByUsername(msg, clients, MAX_CLIENTS) != -1)
			errorUser("Your Not an Admin", clients[i].socket);
		else
			errorUser("User doesn't Exist", clients[i].socket);
	}
	else if (args == "+k")
	{
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].key_mode = 1;
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].password = msg;
	}
	else
		errorUser("/MODE <#channel> <+l|+k|+o|-o> <limit/user/passwsord>", clients[i].socket);
}

void modeNoOptions(Channel *channels, Client *clients, std::string channel,std::string args, const int i)
{
	args.erase(args.find_last_not_of(" \t\r\n") + 1);// Remove trailing whitespace characters
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