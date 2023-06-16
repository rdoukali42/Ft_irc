/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdoukali <rdoukali@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 00:07:45 by rdoukali          #+#    #+#             */
/*   Updated: 2023/06/16 02:53:04 by rdoukali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef IRC_HPP
# define IRC_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>

const int MAX_BUFFER_SIZE = 1024;
const int MAX_CLIENTS = FD_SETSIZE - 1;
const int MAX_CHANNELS = 1023;

struct Client
{
	int socket;
	int indice;
	std::string nickname;
	std::string username;
};

struct Channel
{
	std::string admin; //Who creat Channel should have if default
	int index;
	int limit;
	int invite_only;
	int key_mode;
	int limit_mode;
	std::vector<int> users_sockets;
	std::vector<std::string> admin_users;
	std::string name;
	std::string topic;
	std::string PRVIMSG_Index;
	std::string password;
};

int channelExist(const int clientSocket,Channel *channels,const Client *clients, std::string channel, const int i);
void channelNotExist(const int clientSocket,Channel *channels,const Client *clients, std::string channel, const int i, int *channel_index);
void kickUser(Channel *channels,Client *clients, std::string channelname, std::string user, const int i);
void modeOptions(Channel *channels,const Client *clients, std::string channel,std::string args,std::string msg, const int i);
void modeNoOptions(Channel *channels,const Client *clients, std::string channel,std::string args, const int i);

void removeClient(std::vector<int>& users_sockets, int clientSocket);
bool searchIfExist(const std::vector<int>& sockets, const int& clientSocket);
int isAdmin(const std::vector<std::string>& admin_users, const std::string& user);
int searchBySocket(const int &socket, const Client* clients, int numClients);
int searchByUsername(const std::string& target, const Client* clients, int numClients);
int searchBychannelname(const std::string& target, const Channel* channels, int numChannels);
int checkArg(const std::string str,int clientSocket);
void errorUser(const std::string& msg,int  clientSocket);
void error(const std::string& msg);


#endif