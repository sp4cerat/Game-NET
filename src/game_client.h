
// ##################### CLIENT ##################### //

namespace GameClient
{
	enum GAMESTATE { GAMESTATE_LOGIN = 0, GAMESTATE_LOBBY = 1, GAMESTATE_GAME = 2 };

	// global
	Client client;
	shared_ptr<thread> client_thread;
	bool      quit;

	// player info
	string	username="";
	uint	clientid=0;
	uint	score=0;

	// lobby
	map<uint, string>	lobby_players;
	map<uint, string>	lobby_games;
	vector<string>		lobby_messages;

	// game
	struct GameObject
	{
		enum Type{Player=0,Shot=1};
		GameObject(uint t = 0, string n = "default", vec3 p = vec3(0, 0, 0), vec4 r = vec4(0, 0, 0, 0), uint h=100)
		{
			type = t; pos = p; rot = r; name = n; health = h; 
		};
		uint type;
		uint health;
		uint score;
		string name;
		vec3 pos;
		vec4 rot;
	};
	map<uint, GameObject>	game_objects;

	// game state
	int game_state = GAMESTATE_LOGIN;

	// ------------------------------- //  Game State // ------------------------------- //
	
	void update_login()
	{
		cout << "Client::Waiting to logon " << endl; core_sleep(1000);
	}
	void render_lobby()
	{
		uint line = 0;
		global::screen_clear();
		global::screen_set(0, line++, "Simple Multiplayer Tutorial using RPCs");
		line++;
		global::screen_set(0, line++, "---------  LOBBY  ---------");
		global::screen_set(0, line++, "-Lobby Controls           -");
		global::screen_set(0, line++, "-2..9 : Join game         -");
		global::screen_set(0, line++, "-m    : Message           -");
		global::screen_set(0, line++, "-x    : Exit              -");
		global::screen_set(0, line++, "---------------------------");
		global::screen_set(0, line++, "-Game Controls            -");
		global::screen_set(0, line++, "-wasd : Move    x : Exit  -");
		global::screen_set(0, line++, "-space: Fire    l : Leave -");
		global::screen_set(0, line++, "-r    : Respawn           -");
		global::screen_set(0, line++, "---------------------------");

		for (auto &i : lobby_players)
		{
			global::screen_set(0, line++, "-Player [" + core_to_string(i.first) + "] " + i.second);
		}

		global::screen_set(0, line++, "---------------------------");

		for (auto &i : lobby_games)
		{
			global::screen_set(0, line++, "-Game[" + core_to_string(i.first) + "] " + i.second);
		}

		global::screen_set(0, line++, "---------------------------");

		line = 5;

		global::screen_set(40, line++, "- Chat --------------------");

		for (auto &i : lobby_messages)
		{
			global::screen_set(40, line++, i);
		}

		if (lobby_messages.size() == 10) lobby_messages.clear();

		global::screen_draw();
	}
	void update_lobby()
	{
		render_lobby();

		// process keys

		char c = core_keyb(); 
		
		if (c==0) return;
		if (c == 'x') global::exit=true;
		
		if (c == 'm') // press m for message
		{
			string s("Message from " + username);
			client.call( "lobby_msg", s);
		}
		if (c > '0' && c < '9') // j = join first game that is not lobby
		{
			uint id = c - '0';
			if (lobby_games.find(id) == lobby_games.end())
			{
				cout << "game id does not exist!" << endl;
				return;
			}
			client.call("game_join", id);
		}
	}

	void render_game()
	{
		global::screen_clear();
		global::screen_draw_level();

		// Draw Game Objects

		for (auto &i : game_objects) 
		{ 
			vec3	p = i.second.pos; 
			string	n = i.second.name;
			string  h = core_to_string(i.second.health);

			global::screen_set(p.x, p.y, (i.second.type == GameObject::Player) ? 'O' : '*');
			
			// player name
			global::screen_set(p.x - n.length() / 2, p.y + 1, n);
						
			// health
			if (i.second.type == GameObject::Player)
			{
				global::screen_set(p.x - h.length() / 2, p.y + 2, h);
			}
		}

		// Draw Statusbar
		 
		bool dead = (game_objects.find(clientid) == game_objects.end());
		
		string statusbar;
		statusbar.append(
			string("Health: ") + 
			core_to_string((dead ? 0 : game_objects[clientid].health)) + 
			string("   Score " )+ 
			core_to_string(score) + 
			string((dead ? " [r] to respawn  " : "             ")) 
		);
		loopi(0, statusbar.length()) global::screen_set(i, global::height - 1, statusbar[i]);
		
		// Show Screen

		global::screen_draw();
	}
	void update_game()
	{
		render_game();

		// process keys

		char c = core_keyb(); 
		
		if(c==0)return;	
		if (c == 'x') global::exit = true;

		bool dead = (game_objects.find(clientid) == game_objects.end()); 

		if (dead)
		{
			if (c == 'r') // ready?
			{
				client.call("game_player_ready");
			}
			return;
		}

		GameObject &my = game_objects[clientid];

		bool update_pos = false; 
		if (c == 'd'){ my.rot = vec4(1, 0, 0, 0); update_pos = true; }
		if (c == 'a'){ my.rot = vec4(-1, 0, 0, 0); update_pos = true; }
		if (c == 'w'){ my.rot = vec4(0, -1, 0, 0); update_pos = true; }
		if (c == 's'){ my.rot = vec4(0, 1, 0, 0); update_pos = true; }
		if (update_pos)
		{
			vec3 p = clamp(my.pos + vec3(my.rot.x, my.rot.y, 0), 
				vec3(0, 0, 0), vec3(global::width - 1, global::width - 2, 0));

			if (!global::collision(p.x, p.y))
			{ 
				my.pos = p;
				client.call("game_player_pos", my.pos, vec4(my.rot.x, my.rot.y, 0, 0));
			}
		}
		if (c == ' ')
		{
			client.call("game_player_shoot", my.pos , my.rot);
		}
		if (c == 'l') //  leave to lobby
		{
			for (auto i : lobby_games) if (i.second == "Lobby")
			{
				client.call("game_join", i.first);
				break;
			}
		}
	}
	void update()
	{
		if (game_state == GAMESTATE_LOGIN		) update_login();
		if (game_state == GAMESTATE_LOBBY		) update_lobby();
		if (game_state == GAMESTATE_GAME		) update_game();
	}

	// ------------------------------- //  RPCs  // ------------------------------- //

	void set_game_state(uint id , uint s)
	{
		if (s == GAMESTATE_LOGIN)  cout << "Logging in .." << endl;
		if (s == GAMESTATE_LOBBY)  cout << "Entering Lobby" << endl;
		if (s == GAMESTATE_GAME )  cout << "Entering Game" << endl;

		game_state = s;
		clientid = id;
	}
	void lobby_update(map<uint, string> players,map<uint, string> games)
	{
		lobby_players = players;
		lobby_games = games;
	}
	void lobby_msg( string s)
	{
		lobby_messages.push_back(s);
	}
	void game_obj_health(uint id, uint health)
	{
		game_objects[id].health = health;
		if (health <= 0) { game_objects.erase(id); return; }
	}
	void game_obj_score(uint id, uint s)
	{
   		if(id==clientid) score = s;
	}
	void game_obj_spawn(uint id, uint type, string name, vec3 pos, vec4 rot, float health)
	{
		game_objects[id] = GameObject(type,name,pos,rot,health);
	}
	void game_obj_set_pos(uint id, vec3 pos , vec4 rot)
	{
		game_objects[id].pos = pos;
		game_objects[id].rot = rot;
	}
	// ------------------------------- //  Main Loop // ------------------------------- //
	void main_loop(		string login = "default",
						string password = "12345",
						string hostname = "localhost",
						uint port = 12345,
						uint update_delay_ms = 10)
	{
		// Register RPCs on Server and Client in same order (!!)
		Rpc &rpc = client.get_rpc();
		rpc_register_local( rpc,set_game_state);
		rpc_register_local(	rpc,lobby_update);
		rpc_register_local(	rpc,lobby_msg	);
		rpc_register_local(	rpc,game_obj_health);
		rpc_register_local( rpc,game_obj_score);
		rpc_register_local(	rpc,game_obj_spawn);
		rpc_register_local(	rpc,game_obj_set_pos);

		rpc_register_remote(rpc,login);
		rpc_register_remote(rpc,lobby_msg );
		rpc_register_remote(rpc,game_join);
		rpc_register_remote(rpc,game_player_ready);
		rpc_register_remote(rpc,game_player_shoot);
		rpc_register_remote(rpc,game_player_pos);

		client.connect(hostname, port);

		client.call("login", login, password);

		while (!quit)
		{
			client.process();
			update();
			core_sleep(update_delay_ms);
		}

		client.disconnect();
	}
	void start(		string login="default", 
					string password="12345",
					string hostname="localhost", 
					uint port=12345 , 
					uint update_delay_ms=10)
	{
		quit = 0;
		username = login;
		client_thread = make_shared<thread>(main_loop, login, password, hostname, port, update_delay_ms);
	}
	void stop()
	{
		quit = 1;
		client_thread->join();
	}
};
