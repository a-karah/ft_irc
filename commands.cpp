#include "irc.hpp"

int privmsg(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data)
{
	std::string channel;
	std::string nick;
	std::string message;
	std::string to_send;
	std::string sub_cmd;
	size_t i;
	size_t cmp;
	i  = 0;

	to_send = "";
	sub_cmd = data;
	while (data[0] == ' ')
		data.erase(data.begin());

	if (data.find('#') == 0)
	{
		data.erase(data.begin());
		if (data.find(' ') == std::string::npos)
		{
				message = ":ircserv 411 " + user[id - 1].nick +" :No recipient given (" + sub_cmd + ")\r\n";
				send_message(message, pfd[id].fd);
				return 0;
		}
		channel = data.substr(0, data.find(' '));
		dell_char(data ,' ');
		while (data[0] == ' ')
			data.erase(data.begin());
		if (data[0] != ':')
		{
			message = ":ircserv 411 " + user[id - 1].nick +" :No recipient given (" + sub_cmd + ")\r\n";
			send_message(message, pfd[id].fd);
			return 0;
		}
		data.erase(data.begin());
		if (user[id - 1].channel != channel)
		{
			message = ":ircserv 404 " + user[id - 1].nick + " " + user[id - 1].channel + " :Cannot send to channel\r\n";
			send_message(message, pfd[id].fd);
			return 0;
		}
		message = data.substr(0, data.find('\n'));
		cmp = id;
		i = 1;
		while(i < pfd.size())
		{
			if (user[i - 1].channel == channel && cmp != i)
			{
				to_send = user[id - 1].reply + " PRIVMSG #" + channel + " :" + message +"\r\n";
				send_message(to_send, pfd[i].fd);
			}
			++i;
		}
		return 0;
	}
	else
	{
		if (data.find(' ') == std::string::npos)
		{
			message = ":ircserv 411 " + user[id - 1].nick + " :No recipient given (" + sub_cmd + ")\r\n";
			send_message(message, pfd[id].fd);
			return 0;
		}
		nick = data.substr(0, data.find(' '));
		dell_char(data ,' ');
		while (data[0] == ' ')
			data.erase(data.begin());
		if (data[0] != ':')
		{
			message = ":ircserv 411 " + user[id - 1].nick + " :No recipient given (" + sub_cmd + ")\r\n";
			send_message(message, pfd[id].fd);
			return 0;
		}
		data.erase(data.begin());
		message = data;
		if (message.length() < 1)
		{
			message = ":ircserv 412 " + user[id - 1].nick + " :No text to send\r\n";
			send_message(message, pfd[id].fd);
			return 0;
		}
		i = 1;
		while (i < pfd.size())
		{
			if (user[i - 1].nick == nick)
			{
				message = user[id - 1].reply + " PRIVMSG " + nick + " :" + message +"\r\n";
				send_message(message, pfd[i].fd);
				return 0;
			}
			++i;
		}
		message = ":ircserv 401 " + user[id - 1].nick + " " + nick + " :No such nick/channel\r\n";
		send_message(message, pfd[id].fd);
		return 0;
	}
	return 0;
}

int notice(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data)
{
	std::string channel;
	std::string nick;
	std::string message;
	std::string to_send;
	std::string sub_cmd;
	size_t i;
	size_t cmp;
	i  = 0;

	to_send = "";
	sub_cmd = data;
	while (data[0] == ' ')
		data.erase(data.begin());

	if (data.find('#') == 0)
	{
		data.erase(data.begin());
		if (data.find(' ') == std::string::npos)
				return 0;
		channel = data.substr(0, data.find(' '));
		dell_char(data ,' ');
		while (data[0] == ' ')
			data.erase(data.begin());
		if (data[0] != ':')
			return 0;
		data.erase(data.begin());
		if (user[id - 1].channel != channel)
			return 0;
		message = data.substr(0, data.find('\n'));
		cmp = id;
		i = 1;
		while(i < pfd.size())
		{
			if (user[i - 1].channel == channel && cmp != i)
			{
				to_send = user[id - 1].reply + " NOTICE #" + channel + " :" + message +"\r\n";
				send_message(to_send, pfd[i].fd);
			}
			++i;
		}
		return 0;
	}
	else
	{
		if (data.find(' ') == std::string::npos)
			return 0;
		nick = data.substr(0, data.find(' '));
		dell_char(data ,' ');
		while (data[0] == ' ')
			data.erase(data.begin());
		if (data[0] != ':')
			return 0;
		data.erase(data.begin());
		message = data;
		if (message.length() < 1)
			return 0;
		i = 1;
		while (i < pfd.size())
		{
			if (user[i - 1].nick == nick)
			{
				message = user[id - 1].reply + " NOTICE " + nick + " :" + message +"\r\n";
				send_message(message, pfd[i].fd);
				return 0;
			}
			++i;
		}
		return 0;
	}
	return 0;
}

int join(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data)
{
	std::string channel;
	std::string nick;
	std::string message;
	std::string message2;
	std::vector<t_user> chan_users;
	size_t i;

	i  = 0;
	while (data[0] == ' ')
		data.erase(data.begin());
	if (data.find('#') != 0 || data.find(' ') != std::string::npos)
	{
		message = "475 " + user[id - 1].nick + user[id - 1].data + " :Cannot join channel\r\n";
		send_message(message, pfd[i].fd);
		return 0;
	}
	data.erase(data.begin());
	channel = data;
	user[id - 1].channel = channel;
	i = 1;
	message = user[id - 1].reply + " JOIN #" + channel + "\r\n";
	while (i < pfd.size())
	{
		if (user[i - 1].channel == channel)
		{
			send_message(message, pfd[i].fd);
			chan_users.push_back(user[i - 1]);
		}
		++i;
	}
	message2 = ":ircserv 331 " + user[id - 1].nick + " #" + channel + " :No topic is set\r\n";
	if (!chan_users.empty())
		message2 += ":ircserv 353 " + user[id - 1].nick + " = #" + channel + " :@";
	if (chan_users.size() == 1)
		user[id - 1].is_oper = 1;
	i = 0;
	while (i < chan_users.size())
	{
		message2 += chan_users[i].nick;
		if (i == chan_users.size() - 1)
			message2 += "\r\n";
		else
			message2 += " ";
		++i;
	}
	message2 += ":ircserv 366 " + user[id - 1].nick + " #" + channel + " :End of /NAMES list\r\n";
	send_message(message2, pfd[id].fd);
	return 0;
}

int ping(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data)
{
	std::string message;

	(void)user;
	(void)data;
	message = ":ircserv PONG localhost\r\n";
	send_message(message, pfd[id].fd);
	return 0;
}

int nickname(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data)
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
				message = ":ircserv 433 ERR_NICKNAMEINUSE\n" + my_pass + " :Nickname is already in use\r\n";
				send_message(message, pfd[id].fd);
				return 0;
			}
			i++;
		}
		user[id - 1].nickstat = 1;
		user[id - 1].nick = my_pass;
		return 0;
	}
	user[id - 1].name = my_pass;
	user[id - 1].namestat = 1;
	user[id - 1].reply = ":" + user[id - 1].nick + "!" + user[id - 1].name + "@127.0.0.1";
	message = ":ircserv 001 " + user[id - 1].nick + " Welcome to the Internet Relay Network " \
					+ user[id - 1].reply + "\r\n";
	send_message(message, pfd[id].fd);
	return 0;
}

int bot(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data)
{
	std::string	message;

	(void)user;
	if (data == "INFO")
	{
		time_t now = time(0);
		char* dt = ctime(&now);
		message = ":bot " + std::string(dt);
		send_message(message, pfd[id].fd);
	}
	else
	{
		message = ":bot USAGE: BOT INFO\n";
		send_message(message, pfd[id].fd);
	}
	return 0;
}

int kick(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data)
{
	std::string	channel;
	std::string	nick;
	std::string	message;
	std::string	sub_cmd = data;
	size_t		i;

	i = 0;
	if (data.find(' ') == std::string::npos)
	{
		message = ":ircserv 411 " + user[id - 1].nick + " :No recipient given (" + sub_cmd + ")\r\n";
		send_message(message, pfd[id].fd);
		return 0;
	}
	channel = data.substr(0, data.find(' '));
	channel.erase(channel.begin());
	dell_char(data ,' ');
	while (data[0] == ' ')
		data.erase(data.begin());
	nick =  data.substr(0, data.find('\n'));
	std::cout << "nick " << nick << std::endl;
	if (nick.length() < 1)
	{
		message = ":ircserv 412 " + user[id - 1].nick + " :No text to send\r\n";
		send_message(message, pfd[id].fd);
		return 0;
	}
	while (i < user.size())
	{
		if (user[i].nick == nick)
			break;
		++i;
	}
	if (i == user.size())
	{
		message = ":ircserv 476 " + user[id - 1].nick + " " + channel + " :Bad Channel Mask\r\n";
		send_message(message, pfd[id].fd);
		return 0;
	}
	if (user[i].channel != channel)
	{
		message = ":ircserv 441 " + user[id - 1].nick + " " + channel + " :They aren't on that channel\r\n";
		send_message(message, pfd[id].fd);
		return 0;
	}
	if (user[id - 1].channel != channel)
	{
		message = ":ircserv 442 " + user[id - 1].nick + " " + channel + " :You're not on that channel\r\n";
		send_message(message, pfd[id].fd);
		return 0;
	}
	if (user[id - 1].is_oper == 0)
	{
		message = ":ircserv 482 " + user[id - 1].nick + " " +channel + " :You're not channel operator\r\n";
		send_message(message, pfd[id].fd);
		return 0;
	}
	else
	{
		user[i].channel = "";
		message = ":ircserv " + user[i - 1].nick + " YOU ARE KICKED\r\n";
		send_message(message, pfd[i].fd);
		return 0;
	}
	message = ":ircserv 401 " + user[id - 1].nick + " " + nick + " :No such nick/channel\r\n";
	send_message(message, pfd[id].fd);
	return 0;
}

int password(int id, std::vector<struct pollfd> &pfd, std::vector<t_user> &user, std::string data)
{
	(void)id;
	(void)pfd;
	(void)user;
	(void)data;
	return 0;
}
