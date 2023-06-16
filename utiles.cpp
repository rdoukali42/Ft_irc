/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utiles.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdoukali <rdoukali@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 00:39:33 by rdoukali          #+#    #+#             */
/*   Updated: 2023/06/16 02:44:42 by rdoukali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

void errorUser(const std::string& msg, int clientSocket)
{
	std::string msgError = "Error: " + msg + "\n";
	send(clientSocket, msgError.c_str(), msgError.length(), 0);
	//std::cerr << "Error: " << msg << std::endl;
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
		if (countWords(str) < 2 || countWords(str) > 3)
			errorUser("/TOPIC <#Channel> <Message>/Optional", clientSocket);
		else
			return 1;
	}
	else if (str.substr(0, 6) == "/MODE ")
	{
		if (countWords(str) < 3 || countWords(str) > 4)
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
	else
		errorUser("COMMAND NOT FOUND", clientSocket);
	return -1;
}