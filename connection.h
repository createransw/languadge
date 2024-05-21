#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include "exc.h"

#define BUF_SIZE 256

class connection {
	pollfd fd;
	int id;

	void halt();
public:
	connection(int, const char*);
	connection();
	connection& operator=(const connection&);

	/*-------- commands ---------*/
	void buy(int, int);
	void sell(int, int);
	void prod(int);
	void build();
	void endturn();
	int my_id();
	int players();
	int supply();
	int raw_price();
	int demand();
	int production_price();
	int money(int);
	int raw(int);
	int production(int);
	int factories(int);

	~connection();
};

extern connection server;
