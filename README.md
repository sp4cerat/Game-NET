### Game NET - A Simple Network Library with RPC support for Games

#### License : MIT
#### http://opensource.org/licenses/MIT

**Summary** 

The library provides basic client / server network functionalities for games using RPCs. The major strength of the library is the template based RPC class. It allows to register and call RPCs effortless. It further includes a tutorial game server and game client (each around 300  lines of code ) for a very simple multiplayer shooting game as a proof of concept.

**Features**

* C++11 based 
* Register RPCs in one line without specifying the parameters - the program will automatically detect
* Numbers are automatically sent as the smallest possible datatype (byte, short , .. )
* Supports GLM datatypes for use in 3D Games
* Supported Datatypes : (u)char,(u)short,(u)int,float,double,vector,map, GLM vec,mat and quat
* Support for nested Datatypes like map [ string , vector ]
* Reliable and unreliable calls possible
* Based on ENet
* Tested on Cygwin and Windows, should compile in Linux too

**Limitations**

* Byte order will be supported in the future (htons..)
* only void functions are supported

**Example Usage**

Call Server Function:

Client Side:

    NetClient client;
    rpc_register_remote(client.get_rpc(), login);
    client.connect("localhost", 12345);
    client.call("login", "myname", "pass");

Server Side:

    void login(uint clientid, string name, string password)
    {
        // clientid is the first parameter for server functions
    }
    
    NetServer server;
    rpc_register_local(server.get_rpc(), login);
    thread *t=new thread(NetServer::start, &server);
    

![Screenshot1](https://github.com/sp4cerat/Game-NET/blob/master/screenshots/game.png?raw=true)
![Screenshot2](https://github.com/sp4cerat/Game-NET/blob/master/screenshots/lobby.png?raw=true)

RPC System, how its done: http://cpp.sh/9jc5

(C) by Sven Forstmann in 2015
