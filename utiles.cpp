/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utiles.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdoukali <rdoukali@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 00:39:33 by rdoukali          #+#    #+#             */
/*   Updated: 2023/06/18 18:55:51 by rdoukali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

void error(const std::string& msg)
{
	std::cerr << "Error: " << msg << std::endl;
	return ;
}


bool searchIfExist(const std::vector<int>& sockets, const int& clientSocket)
{
	for (std::vector<int>::const_iterator it = sockets.begin(); it != sockets.end(); ++it) {
		if (*it == clientSocket) {
			return true;
		}
	}
	return false;
}

int isAdmin(const std::vector<std::string>& admin_users, const std::string& user)
{
	for (std::vector<std::string>::const_iterator it = admin_users.begin(); it != admin_users.end(); ++it) {
		std::string c_it = *it;
		if (strcmp(c_it.c_str(), user.c_str()) == 0) {
			return 1;
		}
	}
	return 0;
}

int searchBySocket(const int &socket, const Client* clients, int numClients)
{
	for (int i = 0; i < numClients; i++)
	{
		if (socket == clients[i].socket)//working!
		{
			return i;
		}
	}
	return -1;
}


int searchByUsername(const std::string& target, const Client* clients, int numClients)
{
	std::string target2 = target;
	for (int i = 0; i < numClients; i++)
	{
		if (strcmp(clients[i].username.c_str(), target2.c_str()) == 0)//working!
		{
			return i;
		}
	}
	return -1;
}

int searchBychannelname(const std::string& target, const Channel* channels, int numChannels)
{
	std::string target2 = target;
	for (int i = 0; i < numChannels; i++)
	{
		if (strcmp(channels[i].name.c_str(), target2.c_str()) == 0)
		{
			return i;
		}
	}
	return -1;
}

std::vector<std::string> split_str(std::string str, char delim)
{
	std::vector<std::string> row;
	std::string word;
	std::stringstream s(str);
	while (std::getline(s, word, delim)) {
		row.push_back(word);
	}
	return row;
}

void errorUser(const std::string& msg, int clientSocket)
{
	std::string msgError = "Error: " + msg + "\n";
	send(clientSocket, msgError.c_str(), msgError.length(), 0);
	return ;
}

int countWords(const std::string& str)
{
	std::istringstream iss(str);
	int count = 0;
	std::string word;
	
	while (iss >> word)
		count++;
	return count;
}

int checkUserChannel(Channel *channels,const Client *clients, std::string user, std::string channel, int clientSocket)
{
	if (searchByUsername(user, clients, MAX_CLIENTS) == -1)
	{
		errorUser("USER NOT FOUND", clientSocket);
		return 0;
	}
	if (searchBychannelname(channel, channels, MAX_CHANNELS) == -1)
	{
		errorUser("CHANNEL NOT FOUND", clientSocket);
		return 0;
	}
	return 1;
}

int checkArg(const std::string str, int clientSocket)
{
	if (str.substr(0, 6) == "/KICK ")
	{
		if (countWords(str) < 3 || countWords(str) > 4)
			errorUser("/KICK <#Channel> <user> <:Message>/Optional", clientSocket);
		else
			return 1;
	}
	else if (str.substr(0, 8) == "/INVITE ")
	{
		if (countWords(str) != 3)
			errorUser("/INVITE <#Channel> <user>", clientSocket);
		else
			return 1;
	}
	else if (str.substr(0, 7) == "/TOPIC ")
	{
		if (countWords(str) < 2)
			errorUser("/TOPIC <#Channel> <Message>/Optional", clientSocket);
		else
			return 1;
	}
	else if (str.substr(0, 6) == "/MODE ")
	{
		if (countWords(str) < 3)
			errorUser("/MODE <#Channel> <arg> <Options>", clientSocket);
		else
			return 1;
	}
	else if (str.substr(0, 6) == "/JOIN ")
	{
		if (countWords(str) != 2)
			errorUser("/JOIN <#Channel>", clientSocket);
		else
			return 1;
	}
	else if (str.substr(0, 9) == "/PRIVMSG ")
	{
		if (countWords(str) < 3)
			errorUser("/PRIVMSG <#Channel/USER> <New_Topic>", clientSocket);
		else
			return 1;
	}
	if (str.substr(0, 5) == "/EXIT")
	{
		return 1;
	}
	else
		errorUser("COMMAND NOT FOUND", clientSocket);
	return -1;
}