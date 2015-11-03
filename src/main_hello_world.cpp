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
}
////////////////////////////////////////////////////////////////////////////////
// Simple Hello World
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Server Part

net::Server server(12345);

// RPC
void hello_server(uint clientid, std::string s, int i, double d, float f, glm::vec3 p)
{
	std::cout << "Client " << clientid << " sends " << s << " i:" << i << " d:" << d << " f:" << f << " p:" << net::Any(p).get_data_as_string() << std::endl;
	server.call(clientid, "hello_client", "Greetings from Server", 10, 12.34, 5.1f, glm::vec3(1, 2, 3));
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
void hello_client(std::string s, int i, double d, float f, glm::vec3 p)
{
	std::cout << "Server sends " << s << " i:" << i << " d:" << d << " f:" << f << " p:" << net::Any(p).get_data_as_string() << std::endl;
	client.call("hello_server", "Greetings from Client", 20, 54.32, 7.8f, glm::vec3(6, 7, 8));
};

// Main
void start_client()
{
	core_sleep(1000);
	net::Rpc &r = client.get_rpc();
	rpc_register_local (r, hello_client);
	rpc_register_remote(r, hello_server);
	client.connect("localhost", 12345);
	client.call("hello_server", "Greetings from Client", 10, 12.34, 5.1f , glm::vec3 (1,2,3) );

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
