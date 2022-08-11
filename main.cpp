#include "irc.hpp"

std::string pass;

const char *cmd[] =
{
	"PRIVMSG",
	"JOIN",
	"PING",
	"NICK",
	"USER",
	"BOT",
	"KICK",
	"NOTICE",
	"PASS",
	NULL,
};

int (*cmdp[])(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data) =
{
	&privmsg,
	&join,
	&ping,
	&nickname,
	&nickname,
	&bot,
	&kick,
	&notice,
	&password
};

void	dell_char(std::string &str, char c)
{
	str = str.substr(str.find(c) + 1, str.length() - str.find(c));
}

void	erase_char(std::string &str, char c)
{
	str.erase(std::remove(str.begin(), str.end(), c), str.end());
}

int look_error()
{
	return 0;
}

void	send_message(std::string message, int fd)
{
	std::cout << BGRN << "[Server] " << BWHT << "-> " << BBLU << "[Client " << fd << "] " << YEL << message << RST << std::endl;
	send(fd, message.data(),message.length(), 0);
}

int com_init(struct sockaddr_in *sock, unsigned short int port, unsigned int ip, int bind_flag)
{
	int	sock_fd;
	int yes;

	yes = 1;
	sock->sin_family = AF_INET;
	sock->sin_port = htons(port);     // 0 olursa uygun bir port verir
	sock->sin_addr.s_addr = ip; // INADDR_ANY ile kendi pn'ni
	memset(&(sock->sin_zero), '\0', 8); // geriye kalanı sıfırlai
	sock_fd = socket(AF_INET, SOCK_STREAM, 0); //
	if (-1 == sock_fd)
	{
		std::cerr << "socket Error\n";
		return (-1);
	}
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		close(sock_fd);
		std::cerr << "socket Error\n";
		return (-1);
	}
	if (bind_flag)
		if (-1 == bind(sock_fd, (struct sockaddr *)sock, sizeof(struct sockaddr_in)))
		{
			close(sock_fd);
			std::cerr << "bind Error\n";
			return (-1);
		}
	return (sock_fd);
}

void dell_user(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, sockaddr_in *listen_sock)
{
	close(pfd[id].fd);
	pfd.erase(pfd.begin() + id);
	user.erase(user.begin() + id - 1);
	std::cout << BRED << inet_ntoa(listen_sock->sin_addr) << ":" << ntohs(listen_sock->sin_port) << RST <<" disconnected\n";
}

int look_user(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user,std::string data)
{
	std::string my_pass;
	std::string message;
	int i = 0;

	my_pass = data;
	if (my_pass.length() == 0)
		return 0;
	if (user[id - 1].nickstat == 0)
	{
		while (i < id - 1)
		{
			if (user[i].nick == my_pass)
			{
				message = "433 " + my_pass + " :Nickname is already in use\n";
				send_message(message, pfd[id].fd);
				return 0;
			}
		}
		user[id - 1].nickstat = 1;
		user[id - 1].nick = my_pass;
		return 0;
	}
	user[id - 1].name = my_pass;
	user[id - 1].namestat = 1;
	return 0;
}

int look_passw(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data)
{
	std::string my_pass;
	std::string message;

	my_pass = data;
	if (my_pass != pass)
	{
		message = "password error\n";
		send(pfd[id].fd, message.data(),message.length(), 0);
		return (0);
	}
	user[id - 1].pswdstat = 1;
	message = "<nick>!<user>@<host>\n";
	send(pfd[id].fd, message.data(),message.length(), 0);
	std::cerr << my_pass << "\n";
	return 0;
}

int read_data(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user)
{
	int stat;
	char buff[257];

	while (1)
	{
		memset(buff, 0, 257);
		stat = recv(pfd[id].fd, buff, 257, 0);
		if (stat < 0)
		{
			if (-1 == look_error())
				return -1;
			break;
		}
		if (stat == 0)
			return -1;
		buff[stat] = '\0';
		user[id - 1].data += buff;
	}
	return 0;
}

int look_cmd(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data)
{
	std::string com;
	std::string message;
	std::string sub_cmd;

	int	i = 0;

	sub_cmd = data;
	if (data.find(' ') == std::string::npos)
	{
			message = "411 ERR_NORECIPIENT\n:No recipient given (" + sub_cmd + ")\n";
			send(pfd[id].fd, message.data(),message.length(), 0);
			return 0;
	}
	com = data.substr(0, data.find(' '));
	dell_char(data, ' ');
	std::cout << "command: " << com << std::endl;
	while (cmd[i])
	{
		if (com == cmd[i])
			return cmdp[i](id, pfd, user, data);
		++i;
	}
	message = "411 ERR_NORECIPIENT\n:No recipient given (" + sub_cmd + ")\n";
	send_message(message, pfd[id].fd);
	return 0;
}

int proccess_user(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user)
{
	std::string data;

	while (1)
	{
		if (read_data(id, pfd, user) == -1)
			return (-1);
		if (user[id - 1].data.find('\n') == std::string::npos)
			return 0;
		else
		{
			while (user[id - 1].data.find('\r') != std::string::npos)
			{
				erase_char(user[id - 1].data, '\r');
			}
		}
		data =  user[id - 1].data.substr(0, user[id - 1].data.find('\n'));
		dell_char(user[id - 1].data, '\n');
		std::cout << BBLU << "[Client " << pfd[id].fd << "] " << YEL << data << RST << std::endl;
		if (user[id - 1].pswdstat == 0)
		{
			if (-1 == look_passw(id, pfd, user, data))
				return -1;
			else
				continue;
		}
		if (user[id - 1].namestat == 0 && 0)
		{
			if (-1 == look_user(id, pfd, user, data))
				return -1;
			else
				continue;
		}
		if (-1 == look_cmd(id, pfd, user, data))
			return -1;
		break ;
	}
	return 0;
}

int add_new(int listen_fd, sockaddr_in *listen_sock, std::vector<struct pollfd> &pfd, std::vector<t_user> &user)
{
	int sfd;
	struct pollfd pp;
	t_user newuser;

	socklen_t sock_len = sizeof(sockaddr);
	while (1)
	{
		sfd = accept(listen_fd, (sockaddr *)listen_sock, &sock_len);

		if (sfd < 0)
		{
			if (errno != EWOULDBLOCK)
			{
				std::cerr << "accept error\n";
				return -1;
			}
			break;
		}
		fcntl(sfd, F_SETFL, O_NONBLOCK);
		pp.fd = sfd;
		pp.events = POLLIN;
		pp.revents = 0;
		pfd.push_back(pp);
		newuser.channel = "";
		newuser.name = "";
		newuser.data = "";
		newuser.nick= "";
		newuser.reply= "";
		newuser.pswdstat = 0;
		newuser.namestat = 0;
		newuser.nickstat = 0;
		newuser.is_oper = 0;
		user.push_back(newuser);
		std::string message = "enter password\n";
		send(sfd, message.data(),message.length(), 0);
		std::cout << BRED << inet_ntoa(listen_sock->sin_addr) << ":" << ntohs(listen_sock->sin_port) << RST <<" connected\n";
	}
	return (0);
}

int	poll_loop(int listen_fd, struct sockaddr_in *listen_sock)
{
	int stat;
	size_t i;
	struct pollfd pp;
	std::vector<struct pollfd> pfd;
	std::vector<t_user> user;

	pp.fd = listen_fd;
	pp.events = POLLIN;
	pfd.push_back(pp);
	while(1)
	{
		stat = poll(&pfd[0], pfd.size(), -1);
		if (stat < 0)
		{
			std::cerr << "poll error\n";
			break;
		}
		if (stat == 0)
		{
			std::cerr << "time out\n";
			break;
		}
		i = 0;
		while (i < pfd.size())
		{
			if(pfd[i].revents == 0)
			{
				++i;
				continue;
			}
			if (pfd[i].revents != POLLIN)
			{
					dell_user(i, pfd, user, listen_sock);
					continue;
			}
			if (pfd[i].fd == listen_fd)
			{
				if (-1 == add_new(listen_fd, listen_sock, pfd, user))
					return -1;
			}
			else
			{
				if (-1 == proccess_user(i, pfd, user))
				{
					if (look_error() == -1)
					{
						std::cerr<<"Fatal error\n";
						return -1;
					}
					dell_user(i, pfd, user, listen_sock);
					--i;
				}
			}
			++i;
		}
	}
	return 0;
}

int main(int ac, char **av)
{
	struct sockaddr_in listen_sock;
	int listen_fd;
	int port;
	if (ac != 3)
	{
		std::cerr << "arguments error\n";
		return (1);
	}
	pass = av[2];
	port = std::atoi(av[1]);
	if  (port < 1024 || port > 0xffff)
	{
		std::cerr << "port error\n";
		return (1);
	}
	memset(&listen_sock, 0, sizeof(listen_sock));
	listen_fd = com_init(&listen_sock, port, htonl(INADDR_ANY), 1);
	if (listen_fd == -1)
		return (1);
	if (-1 == fcntl(listen_fd, F_SETFL, O_NONBLOCK))
	{
		close(listen_fd);
		std::cerr << "nonblock error\n";
		return (1);
	}
	if (-1 == listen(listen_fd, 20))
	{
		close(listen_fd);
		std::cerr << "listen error\n";
		return (1);
	}
	poll_loop(listen_fd, &listen_sock);
}
