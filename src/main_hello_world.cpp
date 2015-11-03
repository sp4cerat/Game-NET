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
#include "net_rpc.h"
#include "net_client.h"
#include "net_server.h"

////////////////////////////////////////////////////////////////////////////////
// Simple Hello World
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Server Part

NetServer server(12345);

// RPC
void hello_server(uint clientid, string s, int i, double d, float f , vec3 p )
{
	cout << "Client " << clientid << " sends " << s << " i:" << i << " d:" << d << " f:" << f << " p:" << Rpc::Any(p).get_data_as_string() << endl;
	server.call(clientid, "hello_client", "Greetings from Server", 10, 12.34, 5.1f, vec3(1, 2, 3));
}

// Main
void start_server()
{
	Rpc &r = server.get_rpc();
	rpc_register_remote(r, hello_client);
	rpc_register_local(r, hello_server);
	server.start();
	//server.stop();
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Client Part

NetClient client;

// Client RPCs
void hello_client(string s, int i, double d, float f, vec3 p)
{
	cout << "Server sends " << s << " i:" << i << " d:" << d << " f:" << f << " p:" << Rpc::Any(p).get_data_as_string() << endl;
	client.call("hello_server", "Greetings from Client", 20, 54.32, 7.8f, vec3(6, 7, 8));
};

// Main
void start_client()
{
	core_sleep(1000);
	Rpc &r = client.get_rpc();
	rpc_register_local(r, hello_client);
	rpc_register_remote(r, hello_server);
	client.connect("localhost", 12345);
	client.call("hello_server", "Greetings from Client", 10, 12.34, 5.1f , vec3 (1,2,3) );

	while (1)
	{
		client.process();
		core_sleep(1000);
	}
	//client.disconnect();
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
