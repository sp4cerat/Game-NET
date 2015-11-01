### Game NET - A Simple Network Lib with RPC support

***Write a simple server / client RPC system in just 10+20 lines of code.***

***Write a multiplayer game with client / server in 300+300 lines of code.***

#### License : MIT
#### http://opensource.org/licenses/MIT

**Summary** 

The library provides basic client / server network functionalities for games using RPCs. The major strength of the library is the template based RPC class. It allows to register and call RPCs effortless. It further includes a tutorial game server and game client (each around 300  lines of code ) for a very simple multiplayer shooting game as a proof of concept.

**Network RPC Lib Features**

* Register RPCs in one line. The RPC class autodetects all paramters from the function pointer
* RPC Data-type mismatch or wrong number of parameters are stated as error to ease debugging
* Numbers are automatically sent as the smallest possible datatype (byte, short , .. )
* Supports GLM datatypes for use in 3D Games
* Supported Datatypes : (u)char,(u)short,(u)int,float,double,vector,map, GLM vec,mat and quat
* Support for nested Datatypes like map [ string , vector ]
* Reliable and unreliable calls possible
* Function pointers of remote functions are not required
* Based on ENet
* Tested on Cygwin and Windows, should compile in Linux too
* C++11 based 

**Network RPC Lib Limitations**

* RPCs cannot be class member functions
* No compression
* No encryption
* Byte order will be supported in the future (htons..)
* Only void functions supported. Non-void functions were tested but complicated everything.
* Client to Client connections are not supported

**Example Game Features**

* Lobby 
* Multiple Games
* Handle spwaning/removing of game objects
* Simple Shooting functionality

**Example Usage**

Call Server Function:

Client Side:

    NetClient client;
    
    void set_pos(vec3 pos)
    {
        // do something
        exit(0);
    }
    
    int main()
    {
        rpc_register_remote(client.get_rpc(), login);
        rpc_register_local(client.get_rpc(), set_pos);
        client.connect("localhost", 12345);
        client.call("login", "myname", "pass");
        while(1) client.process();
        //client.disconnect();
    }

Server Side:

    NetServer server;
    
    void login(uint clientid, string name, string password)
    {
        // clientid is the first parameter for server functions
        server.call(clientid, "set_pos", vec3(1,2,3));    
    }
    
    int main()
    {
        rpc_register_local(server.get_rpc(), login);
        rpc_register_remote(server.get_rpc(), set_pos);    
        server.start();
        core_sleep(10000) ; // wait client to do stuff
        server.stop();
    }
    
    

![Screenshot1](https://github.com/sp4cerat/Game-NET/blob/master/screenshots/game.png?raw=true)
![Screenshot2](https://github.com/sp4cerat/Game-NET/blob/master/screenshots/lobby.png?raw=true)

RPC System, how its done: http://cpp.sh/9jc5

(C) by Sven Forstmann in 2015
