////////////////////////////////////////////////////////////////////////////////
//
// Game NET - A Simple Network Tutorial for OpenGL Games
//
// (C) by Sven Forstmann in 2015
//
// License : MIT
// http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include "core.h"

namespace net
{
	using namespace std;
	using namespace glm;

	#include "net_rpc.h"
	#include "net_client.h"
	#include "net_server.h"
};

////////////////////////////////////////////////////////////////////////////////
// Simple Hello World
////////////////////////////////////////////////////////////////////////////////

uint network_port    = 12345;
bool compress_data	 = false;
uint time_out		 = 2000;
uint max_connections = 32;
uint update_delay    = 0;
uint bps_up          = 0; // unlimited
uint bps_down        = 0; // unlimited

////////////////////////////////////////////////////////////////////////////////
// Server Part

net::Server server(network_port, update_delay, max_connections, time_out, compress_data);

// RPC
void hello_server(uint clientid, std::string s)
{
	//cout << "Client " << clientid << " sends " << s << endl;
	//server.call(clientid, "hello_client",  "Greetings from Server");

	static uint t = core_time() % 1000;
	uint		t_now = core_time()%1000;
	static uint bench = 0; bench=bench+1;	

	if (t_now<t)
	{
		std::cout << bench << " RPCs/s " << std::endl;
		bench = 0;
	}
	t = t_now;
}

// Main
void start_server()
{
	net::Rpc &r = server.get_rpc();
	rpc_register_remote(r, hello_client);
	rpc_register_local (r, hello_server);
	server.start();
	//server.stop();
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Client Part

net::Client client;

// Client RPCs
void hello_client(std::string s)
{
	//cout << "Server sends " << s << endl;
};

// Main
void start_client()
{
	core_sleep(1000);
	net::Rpc &r = client.get_rpc();
	rpc_register_local (r, hello_client);
	rpc_register_remote(r, hello_server);
	client.connect("localhost", network_port, bps_down, bps_up, time_out, compress_data);
	
	uint c;

	while (client.connected())
	{
		loopi(0, 20) client.call("hello_server", "Greetings");

		client.process();
	}
	//client.disconnect();
	core_sleep(1000);
	server.stop();
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Main
int main()
{
	start_server();
	start_client();
	return 0;
}
////////////////////////////////////////////////////////////////////////////////

