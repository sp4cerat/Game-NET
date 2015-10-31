### Game NET - A Simple Network Library with RPC support for Games

#### License : MIT
#### http://opensource.org/licenses/MIT

**Summary** 

The library provides basic client / server network functionalities for games using RPCs. The major strength of the library is the template based RPC class. It allows to register and call RPCs effortless. It further includes a tutorial game server and game client (each around 300  lines of code ) for a very simple multiplayer shooting game as a proof of concept.

![Screenshot1](http://i.imgur.com/jDvemZA.png)

**Features**

* C++11 based
* Register RPCs in one line without specifying the parameters - the program will automatically detect
* Numbers are automatically sent as the smallest possible datatype (byte, short , .. )
* Supports GLM datatypes for use in 3D Games
* Supported Datatypes : char,uchar,short,ushort,int,uint,float,double,vector,map, GLM vec,mat and quat
* Support for nested Datatypes like map<string,vector>
* Reliable and unreliable calls possible
* Based on ENet
* Tested on Cygwin and Windows, should compile in Linux too

**Limitations**

*Byte order will be supported in the future (htons..)


(C) by Sven Forstmann in 2015
