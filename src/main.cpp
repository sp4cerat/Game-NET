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
// 
// Global Function for Text rendering - used by game_client.h 
//
const int	global_width = 75, global_height = 25;
const char  global_level[global_height][global_width] =
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
char	global_screen[global_height][global_width+2];
void	global_screen_set(int x, int y, char c)
{
	if (x < 0 || x >= global_width || y < 0 || y >= global_height) return;
	global_screen[y][x] = c;
}
void	global_screen_set(int x, int y, string s)
{
	loopi(0, s.length()) global_screen_set( x+i,  y, s[i]);
}
void	global_screen_clear()
{ 
	loopi(0, global_width) loopj(0, global_height) global_screen_set(i, j, ' '); 
	loopj(0, global_height) global_screen[j][global_width]='\n';
	loopj(0, global_height) global_screen[j][global_width+1] = 0;
	global_screen[global_height-1][global_width] = 0;
};
void	global_screen_draw_level()
{
	//return;
	loopj(0, global_height) loopi(0, strlen( &(global_level[j][0]) ))
		global_screen_set(i, j, global_level[j][i]);
};
void	global_screen_draw()
{
	string s;
	loopi(0, global_height)	s.append(&(global_screen[i][0]));
	core_console_draw(s);
};
bool	global_collision(int x, int y) { return global_level[y][x] == ' ' ? false : true; };
////////////////////////////////////////////////////////////////////////////////
#include "game_server.h"
#include "game_client.h"

int main()
{
	GameServer::start(	12345	/* server port */,
						10,		/* update delay */
						10,		/* max connected players */
						2000	/* client timeout before disconnect*/);
	//system("pause");
	//GameServer::stop();

	core_sleep(100);

	cout << "Enter user: ";
	string login;
	cin >> login;

	GameClient::start(	login,			/* username */
						"123345",		/* password (not checked) */
						"localhost",	/* server host */	
						12345,			/* server port */
						10				/* update delay */	);	
	//system("pause");
	//GameClient::stop();

	while (1)core_sleep(1000);
	
	return 0;
}

