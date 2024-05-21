#include "connection.h"
#include <string>
#include <sstream>

connection server;

using namespace std;

void connection::halt() {
	char cs[BUF_SIZE];
	string s;
	RunError err = RunError("connection faild");
	do {
		s = "market\n";
		write(fd.fd, s.c_str(), s.size());
		poll(&fd, 1, -1);
		if (fd.revents & POLLIN) {
			fd.revents = 0;
			int n_read = read(fd.fd, cs, BUF_SIZE);
			if (n_read == 0)
				throw RunError("disconnected from server");
			if (n_read == -1)
				throw err;
			cs[n_read] = '\0';
			s = cs;
			if (s == "err")
				throw err;
			sleep(3);
		} else
			throw err;
	} while (s == "halt");
}


connection::connection(int port, const char* srv) {
	int sock;
	RunError err = RunError("connection failed");
	sockaddr_in addr;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) 
		throw err;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (inet_aton(srv, &addr.sin_addr) == 0)
		throw err;
	if (connect(sock, (sockaddr*)&addr, sizeof(sockaddr_in)) == -1) {
		throw err;
	}
	fd.fd = sock;
	fd.events = POLLIN | POLLPRI;
	fd.revents = 0;
	char cs[BUF_SIZE];
	poll(&fd, 1, -1);
	if (fd.revents & POLLIN) {
		fd.revents = 0;
		int n_read = read(fd.fd, cs, BUF_SIZE);
		if (n_read == 0)
			throw RunError("disconnected from server");
		if (n_read == -1)
			throw err;
		cs[n_read] = '\0';
		id = atoi(cs);
	} else
		throw err;
	halt();
}

connection::connection() {}

connection& connection::operator=(const connection& a) {
	fd.fd = dup(a.fd.fd);
	fd.events = a.fd.events;
	id = a.id;
	fd.revents = 0;
	return *this;
}

void connection::buy(int number, int price) {
	string s = string("buy") + " " + to_string(number) + " " + to_string(price)
		+ "\n";
	char cs[BUF_SIZE];
	write(fd.fd, s.c_str(), s.size());
	RunError err = RunError("unsuccessful buy");
	poll(&fd, 1, -1);
	if (fd.revents & POLLIN) {
		fd.revents = 0;
		int n_read = read(fd.fd, cs, BUF_SIZE);
		if (n_read == 0)
			throw RunError("disconnected from server");
		if (n_read == -1)
			throw err;
		cs[n_read] = '\0';
		s = cs;
		if (s == "err")
			throw err;
	} else
		throw err;
}

void connection::sell(int number, int price) {
	string s = string("sell") + " " + to_string(number) + " " + to_string(price)
		+ "\n";
	char cs[BUF_SIZE];
	write(fd.fd, s.c_str(), s.size());
	RunError err = RunError("unsuccessful sell");
	poll(&fd, 1, -1);
	if (fd.revents & POLLIN) {
		fd.revents = 0;
		int n_read = read(fd.fd, cs, BUF_SIZE);
		if (n_read == 0)
			throw RunError("disconnected from server");
		if (n_read == -1)
			throw err;
		cs[n_read] = '\0';
		s = cs;
		if (s == "err")
			throw err;
	} else
		throw err;
}

void connection::prod(int number) {
	string s = string("produce") + " " + to_string(number) + "\n";
	char cs[BUF_SIZE];
	write(fd.fd, s.c_str(), s.size());
	RunError err = RunError("unsuccessful prod");
	poll(&fd, 1, -1);
	if (fd.revents & POLLIN) {
		fd.revents = 0;
		int n_read = read(fd.fd, cs, BUF_SIZE);
		if (n_read == 0)
			throw RunError("disconnected from server");
		if (n_read == -1)
			throw err;
		cs[n_read] = '\0';
		s = cs;
		if (s == "err")
			throw err;
	} else
		throw err;
}

void connection::build() {
	string s = string("newplant\n");
	char cs[BUF_SIZE];
	write(fd.fd, s.c_str(), s.size());
	RunError err = RunError("unsuccessful build");
	poll(&fd, 1, -1);
	if (fd.revents & POLLIN) {
		fd.revents = 0;
		int n_read = read(fd.fd, cs, BUF_SIZE);
		if (n_read == 0)
			throw RunError("disconnected from server");
		if (n_read == -1)
			throw err;
		cs[n_read] = '\0';
		s = cs;
		if (s == "err")
			throw err;
	} else
		throw err;
}

void connection::endturn() {
	string s = string("end\n");
	char cs[BUF_SIZE];
	write(fd.fd, s.c_str(), s.size());
	RunError err = RunError("unsuccessful build");
	poll(&fd, 1, -1);
	if (fd.revents & POLLIN) {
		fd.revents = 0;
		int n_read = read(fd.fd, cs, BUF_SIZE);
		if (n_read == 0)
			throw RunError("disconnected from server");
		if (n_read == -1)
			throw err;
		cs[n_read] = '\0';
		s = cs;
		if (s == "err")
			throw err;
	} else
		throw err;
	poll(&fd, 1, -1);
	if (fd.revents & POLLIN) {
		fd.revents = 0;
		int n_read = read(fd.fd, cs, BUF_SIZE);
		if (n_read == 0)
			throw RunError("disconnected from server");
		if (n_read == -1)
			throw err;
		cs[n_read] = '\0';
		s = cs;
		if (s != "new")
			throw err;
	} else
		throw err;
}

int connection::my_id() {
	return id;
}

int connection::players() {
	string s = string("market\n");
	char cs[BUF_SIZE];
	write(fd.fd, s.c_str(), s.size());
	RunError err = RunError("unsuccessful players");
	poll(&fd, 1, -1);
	if (fd.revents & POLLIN) {
		fd.revents = 0;
		int n_read = read(fd.fd, cs, BUF_SIZE);
		if (n_read == 0)
			throw RunError("disconnected from server");
		if (n_read == -1)
			throw err;
		cs[n_read] = '\0';
		s = cs;
		if (s == "err")
			throw err;
	} else 
		throw err;
	istringstream in(s);
	int res;
	in >> res >> res >> res >> res >> res;
	return res;
}

int connection::supply() {
	string s = string("market\n");
	char cs[BUF_SIZE];
	write(fd.fd, s.c_str(), s.size());
	RunError err = RunError("unsuccessful supply");
	poll(&fd, 1, -1);
	if (fd.revents & POLLIN) {
		fd.revents = 0;
		int n_read = read(fd.fd, cs, BUF_SIZE);
		if (n_read == 0)
			throw RunError("disconnected from server");
		if (n_read == -1)
			throw err;
		cs[n_read] = '\0';
		s = cs;
		if (s == "err")
			throw err;
	} else
		throw err;
	istringstream in(s);
	int res;
	in >> res;
	return res;
}

int connection::raw_price() {
	string s = string("market\n");
	char cs[BUF_SIZE];
	write(fd.fd, s.c_str(), s.size());
	RunError err = RunError("unsuccessful raw_price");
	poll(&fd, 1, -1);
	if (fd.revents & POLLIN) {
		fd.revents = 0;
		int n_read = read(fd.fd, cs, BUF_SIZE);
		if (n_read == 0)
			throw RunError("disconnected from server");
		if (n_read == -1)
			throw err;
		cs[n_read] = '\0';
		s = cs;
		if (s == "err")
			throw err;
	} else
		throw err;
	istringstream in(s);
	int res;
	in >> res >> res;
	return res;
}

int connection::demand() {
	string s = string("market\n");
	char cs[BUF_SIZE];
	write(fd.fd, s.c_str(), s.size());
	RunError err = RunError("unsuccessful demand");
	poll(&fd, 1, -1);
	if (fd.revents & POLLIN) {
		fd.revents = 0;
		int n_read = read(fd.fd, cs, BUF_SIZE);
		if (n_read == 0)
			throw RunError("disconnected from server");
		if (n_read == -1)
			throw err;
		cs[n_read] = '\0';
		s = cs;
		if (s == "err")
			throw err;
	} else
		throw err;
	istringstream in(s);
	int res;
	in >> res >> res >> res;
	return res;
}

int connection::production_price() {
	string s = string("market\n");
	char cs[BUF_SIZE];
	write(fd.fd, s.c_str(), s.size());
	RunError err = RunError("unsuccessful production_price");
	poll(&fd, 1, -1);
	if (fd.revents & POLLIN) {
		fd.revents = 0;
		int n_read = read(fd.fd, cs, BUF_SIZE);
		if (n_read == 0)
			throw RunError("disconnected from server");
		if (n_read == -1)
			throw err;
		cs[n_read] = '\0';
		s = cs;
		if (s == "err")
			throw err;
	} else
		throw err;
	istringstream in(s);
	int res;
	in >> res >> res >> res >> res;
	return res;
}

int connection::money(int n) {
	string s = string("inf") + " " + to_string(n) + "\n";
	char cs[BUF_SIZE];
	write(fd.fd, s.c_str(), s.size());
	RunError err = RunError("unsuccessful money");
	poll(&fd, 1, -1);
	if (fd.revents & POLLIN) {
		fd.revents = 0;
		int n_read = read(fd.fd, cs, BUF_SIZE);
		if (n_read == 0)
			throw RunError("disconnected from server");
		if (n_read == -1)
			throw err;
		cs[n_read] = '\0';
		s = cs;
		if (s == "err")
			throw err;
	} else
		throw err;
	istringstream in(s);
	int res;
	in >> res >> res >> res >> res >> res;
	return res;
}

int connection::raw(int n) {
	string s = string("inf") + " " + to_string(n) + "\n";
	char cs[BUF_SIZE];
	write(fd.fd, s.c_str(), s.size());
	RunError err = RunError("unsuccessful raw");
	poll(&fd, 1, -1);
	if (fd.revents & POLLIN) {
		fd.revents = 0;
		int n_read = read(fd.fd, cs, BUF_SIZE);
		if (n_read == 0)
			throw RunError("disconnected from server");
		if (n_read == -1)
			throw err;
		cs[n_read] = '\0';
		s = cs;
		if (s == "err")
			throw err;
	} else
		throw err;
	istringstream in(s);
	int res;
	in >> res >> res;
	return res;
}

int connection::production(int n) {
	string s = string("inf") + " " + to_string(n) + "\n";
	char cs[BUF_SIZE];
	write(fd.fd, s.c_str(), s.size());
	RunError err = RunError("unsuccessful production");
	poll(&fd, 1, -1);
	if (fd.revents & POLLIN) {
		fd.revents = 0;
		int n_read = read(fd.fd, cs, BUF_SIZE);
		if (n_read == 0)
			throw RunError("disconnected from server");
		if (n_read == -1)
			throw err;
		cs[n_read] = '\0';
		s = cs;
		if (s == "err")
			throw err;
	} else
		throw err;
	istringstream in(s);
	int res;
	in >> res >> res >> res;
	return res;
}

int connection::factories(int n) {
	string s = string("inf") + " " + to_string(n) + "\n";
	char cs[BUF_SIZE];
	write(fd.fd, s.c_str(), s.size());
	RunError err = RunError("unsuccessful factories");
	poll(&fd, 1, -1);
	if (fd.revents & POLLIN) {
		fd.revents = 0;
		int n_read = read(fd.fd, cs, BUF_SIZE);
		if (n_read == 0)
			throw RunError("disconnected from server");
		if (n_read == -1)
			throw err;
		cs[n_read] = '\0';
		s = cs;
		if (s == "err")
			throw err;
	} else
		throw err;
	istringstream in(s);
	int res;
	in >> res >> res >> res >> res;
	return res;
}

connection::~connection() {
	if (id)
		close(fd.fd);
}
