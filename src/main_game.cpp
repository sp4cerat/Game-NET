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

	////////////////////////////////////////////////////////////////////////////////
	// 
	// Global Function for Text rendering - used by game_client.h 
	//
	namespace global
	{
		bool		exit = false;
		const int	width = 75, height = 25;
		const char  level[height][width] =
		{
			{ "-Giant Dungeon Tutorial--------         ------------------------------" },//0
			{ "                                                                      " },//1
			{ "                                                                      " },//2
			{ "       x------+--------------------x                                  " },//3
			{ "              |                                                       " },//4
			{ "              |                                    x  x               " },//5
			{ "              |                                    |  |               " },//6
			{ "              x                                    |  |               " },//7
			{ "                          x  x                     x--x               " },//8
			{ "                          |  |                                        " },//9
			{ "                          |  |     x----x                             " },//10
			{ "                          |  |     |                                  " },//11
			{ "                  x-------x  |     x               x                  " },//12
			{ "                  |          |                     |                  " },//13
			{ "                  x---x x----x                     |      x----x      " },//14
			{ "                                                   |                  " },//15
			{ "                                                   |                  " },//16
			{ "                                              x----+-------x          " },//17
			{ "       /|_____                                                        " },//18
			{ "      /   /   |___                                                    " },//19
			{ " ____/   /     /  |_______                                            " },//20
			{ "              /                                                       " },//21
			{ "                                                                      " },//22
			{ "                                                                      " } //23
		};
		char	screen[height][width + 2];
		void	screen_set(int x, int y, char c)
		{
			if (x < 0 || x >= width || y < 0 || y >= height) return;
			screen[y][x] = c;
		}
		void	screen_set(int x, int y, string s)
		{
			loopi(0, s.length()) screen_set(x + i, y, s[i]);
		}
		void	screen_clear()
		{
			loopi(0, width) loopj(0, height) screen_set(i, j, ' ');
			loopj(0, height) screen[j][width] = '\n';
			loopj(0, height) screen[j][width + 1] = 0;
			screen[height - 1][width] = 0;
		};
		void	screen_draw_level()
		{
			//return;
			loopj(0, height) loopi(0, strlen(&(level[j][0])))
				screen_set(i, j, level[j][i]);
		};
		void	screen_draw()
		{
			string s;
			loopi(0, height)	s.append(&(screen[i][0]));
			core_console_draw(s);
		};
		bool	collision(int x, int y) { return level[y][x] == ' ' ? false : true; };
	};
	////////////////////////////////////////////////////////////////////////////////
	#include "game_server.h"
	#include "game_client.h"
};
int main()
{
	net::GameServer::start(	12345	/* server port */,
						10,			/* update delay */
						10,			/* max connected players */
						2000		/* client timeout before disconnect*/);

	core_sleep(100);

	std::cout << "Enter user: ";
	std::string login;
	std::cin >> login;

	net::GameClient::start(login,		/* username */
						"12345",		/* password (not checked) */
						"localhost",	/* server host */	
						12345,			/* server port */
						10				/* update delay */	);	

	while (!net::global::exit) core_sleep(1000);

	net::GameClient::stop();
	net::GameServer::stop();
	
	return 0;
}

