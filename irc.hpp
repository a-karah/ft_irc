#ifndef IRC_HPP
# define IRC_HPP

#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>

#include <iostream>
#include <iterator>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>

#include "colors.hpp"

typedef struct s_user
{
	int 		pswdstat;
	int 		namestat;
	int			nickstat;
	int			is_oper;
	std::string nick;
	std::string name;
	std::string data;
	std::string channel;
	std::string	reply;
}			t_user;

void	dell_char(std::string &str, char c);
void	send_message(std::string message, int fd);

int privmsg(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data);
int notice(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data);
int join(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data);
int ping(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data);
int nickname(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data);
int bot(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data);
int kick(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data);
int password(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data);

#endif
