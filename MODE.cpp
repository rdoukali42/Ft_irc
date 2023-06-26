#include "irc.hpp"

void modeOptions(Channel *channels, Client *clients, std::string channel,std::string args,std::string msg, const int i)
{
	if (args == "+l")
	{
		int new_limit;

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
			channels[searchBychannelname(channel, channels, MAX_CHANNELS)].limit_mode = 1;
			channels[searchBychannelname(channel, channels, MAX_CHANNELS)].limit = new_limit;
		}
		catch (std::runtime_error &e){
			std::string limitErrorPrompt = "Error: " + std::string(e.what());
			sendUser(limitErrorPrompt, clients[i].socket, clients[i].nickname);
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
				std::string channelFullPrompt = "You are now ADMIN in " + channels[searchBychannelname(channel, channels, MAX_CHANNELS)].name;
				sendUser(channelFullPrompt, clients[searchByUsername(msg, clients, MAX_CLIENTS)].socket, clients[searchByUsername(msg, clients, MAX_CLIENTS)].nickname);
			}
		}
		else
			errorUser("User doesn't Exist in the Channel", clients[i].socket);
	}
	else if ( args == "-o")
	{ 
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
	args.erase(args.find_last_not_of(" \t\r\n") + 1);
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
		sendToAdmins(channels, clients, searchBychannelname(channel, channels, MAX_CHANNELS), "User invitation is unrestricted!");
	}
	else if (args == "+i")
	{
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].invite_only = 1;
		sendToAdmins(channels, clients, searchBychannelname(channel, channels, MAX_CHANNELS), "User invitation is restricted!");
	}
	else if (args == "-t")
	{
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].topic_mode = 0;
		sendUser("Topic mode is Unset", clients[i].socket, clients[i].nickname);
	}
	else if (args == "+t")
	{
		channels[searchBychannelname(channel, channels, MAX_CHANNELS)].topic_mode = 1;
		sendUser("Topic mode is Set", clients[i].socket, clients[i].nickname);
	}
	else
		errorUser("/MODE <#channel> <-t|+t|+i|-i|-l|-k>", clients[i].socket);
}
