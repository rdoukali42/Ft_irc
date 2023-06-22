/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utiles.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adinari <adinari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 00:39:33 by rdoukali          #+#    #+#             */
/*   Updated: 2023/06/21 20:17:55 by adinari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

void error(const std::string& msg)
{
	std::cerr << "Error: " << msg << std::endl;
	return ;
}

int ifWord(const std::string str)
{
	for(int i = 0; i < str.length(); i++){
		if ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z'))
			return 1;
		else if (str[i] >= '0' && str[i] <= '9')
			return 1;
	}
	return 0;
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
		if (word.size())
			row.push_back(word);
		// std::cout << "word =" + word + "." << std::endl;
	}
	return row;
}

std::string getMsg(std::string& str){
    std::string result = str;
    std::string::size_type pos = str.find(" ");
	if (pos != std::string::npos) 
		result = str.substr(pos + 1);
	pos = result.find_first_not_of(" \t\r\n");
	if (pos != std::string::npos) 
		result = result.substr(pos + 1);
    if (pos != std::string::npos) {
        pos = result.find(" ");
        if (pos != std::string::npos)
            result = result.substr(pos);
	pos = result.find_first_not_of(" \t\r\n");
	if (pos != std::string::npos) 
		result = result.substr(pos);
    }
    return result;
}
int countWords(const std::string& str)
{
	// std::cout << "XXXXXXXXXXXXXX" << std::endl;
	std::istringstream iss(str);
	int count = 0;
	std::string word;
	
	while (iss >> word)
		count++;
	// std::cout << "str = " << str << std::endl;
	// std::cout << "w_count = " << count << std::endl;
	// std::cout << "YYYYYYYYYYYYYY" << std::endl;
	return count;
}

void erase_spaces(std::string& str)
{
	int fw = 0;
    std::string res;
	std::vector<std::string> row;
	std::string word;
	std::stringstream s(str);
	while (std::getline(s, word, ' ') && fw != 2) {
		if (word.size())
			fw++;
		row.push_back(word);
	}
	for (std::vector<std::string>::const_iterator it = row.begin(); it != row.end(); ++it)
	{
        res += *it;
		if (fw)
		{
			res += " ";
			fw = 0;
		}
	}
	if (countWords(str) <= 2)
	{
		str = res;
		return ;
	}
    std::string remaining = getMsg(str);
	str = res + " " + remaining;
}

void errorUser(const std::string& msg, int clientSocket)
{
	std::string msgError = "Error: " + msg + "\n";
	send(clientSocket, msgError.c_str(), msgError.length(), 0);
	return ;
}

void sendUser(const std::string& msg, int clientSocket)
{
	std::string msgError = msg + "\n";
	send(clientSocket, msgError.c_str(), msgError.length(), 0);
	return ;
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

int numOfAdmins(Channel *channels, Client *clients, int ch_in)
{
	int j = 0;
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if (isAdmin(channels[ch_in].admin_users, clients[i].username))
			j++;
	}
	return j;
}

void listAdmins(Channel *channels, Client *clients, int ind, int ch_in)
{
	int j = 1;
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if (isAdmin(channels[ch_in].admin_users, clients[i].username))
		{
			sendUser("    -> ADMIN (" + std::to_string(j) + ") : " + clients[i].username, clients[ind].socket);
			j++;
		}
	}
}

void listUsers(Channel *channels, Client *clients, int ind, int ch_in)
{
	int j = 1;
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if (searchIfExist(channels[ch_in].users_sockets, clients[i].socket))
		{
			if (!isAdmin(channels[ch_in].admin_users, clients[i].username))
			{
				sendUser("     -> USER (" + std::to_string(j) + ") : " + clients[i].username, clients[ind].socket);
				j++;
			}
		}
	}
}

void listChannels(Channel *channels, Client *clients, int ind)
{
	int j = 1;
	for(int i = 0; i < MAX_CHANNELS; i++)
	{
		if (channels[i].name != "")
		{
			sendUser("		CHANNEL (" + std::to_string(j) + ")" , clients[ind].socket);
			sendUser("CHANNEL NAME        : " + channels[i].name, clients[ind].socket);
			sendUser("CHANNEL TOPIC       : " + channels[i].topic, clients[ind].socket);
			if (channels[i].limit_mode == 1)
				sendUser("CHANNEL LIMIT       : " + std::to_string(channels[i].limit), clients[ind].socket);
			else
				sendUser("CHANNEL LIMIT       : NOT SET", clients[ind].socket);
			if (channels[i].key_mode == 0)
				sendUser("CHANNEL PASSWORD    : NOT ACTIVATED", clients[ind].socket);
			else
				sendUser("CHANNEL PASSWORD    : ACTIVATED", clients[ind].socket);
			if (channels[i].invite_only == 0)
				sendUser("CHANNEL INVITE MODE : NOT ACTIVATED", clients[ind].socket);
			else
				sendUser("CHANNEL INVITE MODE : ACTIVATED", clients[ind].socket);
			if (strcmp(channels[i].PRVIMSG_Index.c_str(), "yes") == 0)
				sendUser("CHANNEL PRVMSG MODE : ACTIVATED", clients[ind].socket);
			else
				sendUser("CHANNEL PRVMSG MODE : NOT ACTIVATED", clients[ind].socket);
			sendUser("ADMINS :", clients[ind].socket);
			listAdmins(channels, clients, ind, i);
			sendUser("USERS :", clients[ind].socket);
			listUsers(channels, clients, ind, i);
			sendUser("--------------------------------------------------------", clients[ind].socket);
			j++;
		}
	}
}

int checkArg(const std::string str, int clientSocket)
{
	// std::string word;
	// std::stringstream s(str);
	// std::getline(s, word, ' ');
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
			std::cout << "join error!" << std::endl;
	}
	else if (str.substr(0, 9) == "/PRIVMSG ")
	{
		if (countWords(str) < 3)
			errorUser("/PRIVMSG <#Channel/USER> <New_Topic>", clientSocket);
		else
			return 1;
	}
	else if (str.substr(0, 6) == "/PART ")
	{
		if (countWords(str) != 2)
			errorUser("/PART <#Channel>", clientSocket);
		else
			return 1;
	}
	else if (str.substr(0, 7) == "/WHOIS ")
	{
		if (countWords(str) != 2)
			errorUser("/WHOIS <user>", clientSocket);
		else
			return 1;
	}
	else if (str.substr(0, 6) == "/NICK ")
	{
		if (countWords(str) != 2)
			errorUser("/NICK <new_nickname>", clientSocket);
		else
			return 1;
	}
	else if (str.substr(0, 6) == "/LIST " || str.substr(0, 6) == "/LIST\0")
	{
		if (countWords(str) != 1)
			errorUser("/LIST", clientSocket);
		else
			return 1;
	}
	else if (str.substr(0, 6) == "/EXIT " || str.substr(0, 6) == "/EXIT\0")
		return 1;
	else if (str.substr(0, 6) == "/QUIT " || str.substr(0, 6) == "/QUIT\0")
		return 1;
	else
		errorUser("INVALID COMMAND!!", clientSocket);
	return -1;
}