/*
  tabletpos.cpp: Server for Gfxtablet showing the cursor position.
  
  Copyright (c) 2016 Walter Fetter Lages <w.fetter@ieee.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    You can also obtain a copy of the GNU General Public License
    at <http://www.gnu.org/licenses>.

*/

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>

#include <iostream>
#include <iomanip>

#include "protocol.h"

using namespace std;

static int udp_socket;

void quit(int signal)
{
	close(udp_socket);
	exit(0);
}

int main(int argc, char *argv[])
{
	if((udp_socket=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) == -1)
	{
		cerr << "socket():" << strerror(errno) << endl;
		return -EXIT_FAILURE;
	}

	struct sockaddr_in addr;
	bzero(&addr,sizeof(struct sockaddr_in));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(GFXTABLET_PORT);
	addr.sin_addr.s_addr=htonl(INADDR_ANY);

	if(bind(udp_socket,(struct sockaddr *)&addr,sizeof(addr)) == -1)
	{
		cerr << "bind():" << strerror(errno) << endl;
		return -EXIT_FAILURE;
	}

	signal(SIGINT,quit);
	signal(SIGTERM,quit);

	struct event_packet ev_pkt;
	
	struct timeval tv0;
	gettimeofday(&tv0,NULL);
	
	// every packet has at least 9 bytes
	while(recv(udp_socket,&ev_pkt,sizeof(ev_pkt),0) >= 9)
	{		
		if(memcmp(ev_pkt.signature,"GfxTablet",9) != 0)
		{
			cerr << "\nGot unknown packet on port "
				<< GFXTABLET_PORT << ", ignoring\n";
			continue;
		}
		ev_pkt.version=ntohs(ev_pkt.version);
		if(ev_pkt.version != PROTOCOL_VERSION)
		{
			cerr << "\nGfxTablet app speaks protocol version " << ev_pkt.version
				<< " but this program speaks version " << PROTOCOL_VERSION
				<< ", please update\n";
			break;
		}

		ev_pkt.x=ntohs(ev_pkt.x);
		ev_pkt.y=ntohs(ev_pkt.y);
		ev_pkt.pressure=ntohs(ev_pkt.pressure);
		
		cout << "x: " << ev_pkt.x
			<< " y: " << ev_pkt.y
			<< " pressure: " << ev_pkt.pressure;

		switch(ev_pkt.type)
		{
			case EVENT_TYPE_MOTION:
				break;
			case EVENT_TYPE_BUTTON:
				cout << " button: " << (int) ev_pkt.button
					<< " down: " << (int) ev_pkt.down; 
				break;
		}
		struct timeval tv;
		struct timeval tr;
		gettimeofday(&tv,NULL);
		timersub(&tv,&tv0,&tr);
		
		cout << " time: " << tr.tv_sec << "." << setfill('0') << setw(6) << tr.tv_usec << endl;
	}
	close(udp_socket);

	return 0;
}
