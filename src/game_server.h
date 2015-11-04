
// ##################### SERVER ##################### //

namespace GameServer
{
	enum GAMESTATE { GAMESTATE_LOGIN=0, GAMESTATE_LOBBY=1, GAMESTATE_GAME=2 };

	// ------------------------ Data ------------------------ //

	Server server;

	uint id_lobby;

	struct GameObject
	{
		enum Type{ Player = 0, Shot = 1 };
		GameObject(uint t = Player, vec3 p = vec3(0, 0, 0), vec4 r = vec4(0, 0, 0, 0),float h=100,uint c=0)
		{
			type = t; pos = p; rot = r; health = h; creator = c; 
		};
		float distance(GameObject &other)
		{
			return length(other.pos - pos);
		}
		uint creator; // referring object - player for a shot e.g.
		uint type;
		uint health;
		vec3 pos;
		vec4 rot;
	};
	struct Player
	{
		Player(uint game_id = 0, uint s=0){ game = game_id; game_state = GAMESTATE_LOBBY; score = s; };
		int game_state;
		uint game;
		uint score;
	};
	struct Game
	{
		Game( string l = "default"){ level = l; }
		string level;
		string type;
		map<uint, GameObject> game_objects;
	};
	
	map<uint, Player>	players;
	map<uint, Game>		games;
	map<uint, string>	names;

	// ------------------------ Utility ------------------------ //

	Message lobby_update_msg()
	{
		map<uint, string> game_list;
		map<uint, string> players_in_lobby;

		for (auto &i : games)	game_list[i.first] = names[i.first];
		for (auto &i : players)	if(i.second.game == id_lobby ) players_in_lobby[i.first] = names[i.first];
		
		return server.msg("lobby_update", players_in_lobby, game_list);
	}
	void lobby_join(uint clientid)
	{
		uint game = players[clientid].game;

		// remove from game
		Message m = server.msg("game_obj_health", clientid, 0);
		for (auto &p : players) if (p.second.game == game)
		{
			server.send_to(p.first, m);
		}
		games[game].game_objects.erase(clientid);
		
		// enter lobby
		players[clientid].game_state = GAMESTATE_LOBBY;
		players[clientid].game = id_lobby;
		server.call(clientid, "set_game_state", clientid, GAMESTATE_LOBBY);
		server.send_to(clientid, lobby_update_msg());
	}
	vec3 game_player_spawn_pos()
	{
		static uint t = 12341155; uint x = 0, y = 0;
		
		do{
			t = t * 2423 + core_time() * 2157;
			x = t % 65;
			y = t % 22;
		} while ( global::collision(x,y) );

		return vec3(x, y, 0);
	}
	void game_player_spawn(uint clientid)
	{
		uint id = players[clientid].game;

		// spawn player
		Game &g = games[id];
		g.game_objects[clientid] = GameObject(GameObject::Player, game_player_spawn_pos(), vec4(0, 1, 0, 0), 100);

		// broadcast spawn
		Message msg = server.msg("game_obj_spawn", clientid, GameObject::Player, names[clientid],
			g.game_objects[clientid].pos,
			g.game_objects[clientid].rot,
			g.game_objects[clientid].health);

		for (auto &i : players) if (i.second.game == id && clientid != i.first)
		{
			server.send_to(i.first, msg);
		}
		// other positions -> clientid
		for (auto &i : g.game_objects)
		{
			server.call(clientid,  "game_obj_spawn", i.first,
				i.second.type,
				names[i.first],
				i.second.pos,
				i.second.rot,
				i.second.health);
		}
	}
	void update_game()
	{
		// update shots
		map<uint, uint> id_remove;

		for (auto &g : games) if (g.first != id_lobby)
			for (auto &i : g.second.game_objects)
				if (i.second.type == GameObject::Shot)
				{
					vec3 &pos = i.second.pos;
					pos += vec3(i.second.rot.x, i.second.rot.y, 0)*0.25f;

					Message m = server.msg("game_obj_set_pos", i.first, i.second.pos, i.second.rot);

					//remove shots if collided or outside screen 
					if (pos.x >= global::width || pos.y >= global::height || pos.x < 0 || pos.y < 0 || global::collision(pos.x, pos.y))
					{
						// remove obj by setting health to 0
						m = server.msg("game_obj_health", i.first, 0);
						id_remove[g.first] = i.first;
					}

					// hit other player ?
					for (auto &j : g.second.game_objects)
						if (j.second.type == GameObject::Player)
						{
							if (j.second.distance(i.second) < 1)
							{
								// notify others of health
								j.second.health -= 1;
								Message m1 = server.msg("game_obj_health", j.first, j.second.health);

								// benefit shoot creator if less than 100 health
								uint &creator_health = g.second.game_objects[i.second.creator].health;
								if (creator_health<100)creator_health++;
								Message m2 = server.msg("game_obj_health", i.second.creator, creator_health);

								// get score in addition ?
								uint &creator_score = players[i.second.creator].score;
								Message m3;
								if (j.second.health <= 0)
								{
									creator_score++;
									m3 = server.msg("game_obj_score", i.second.creator, creator_score);
								}
								// broadcast message
								for (auto &p : players) if (p.second.game == g.first)
								{
									server.send_to(p.first, m1);
									server.send_to(p.first, m2);
									server.send_to(p.first, m3);
								}
								if (j.second.health <= 0) id_remove[g.first] = j.first;
							}
						}
					// notify others
					for (auto &p : players) if (p.second.game == g.first)
					{
						server.send_to(p.first, m, 0 /*unrealible*/);
					}
				}
		for (auto i : id_remove)
		{
			games[i.first].game_objects.erase(i.second);
		}
	}
	void update_lobby()
	{
		Message msg_lobby = lobby_update_msg();

		static uint counter = 0; counter = counter + 1;

		if (counter % 100 != 0) return;

		for (auto &i : players)
			if (i.second.game_state == GAMESTATE_LOBBY)
			{
				server.send_to(i.first, msg_lobby);
			}
	}
	// ------------------------ Callbacks ------------------------ //

	void connect(uint clientid)
	{
		cout << "Server::Client " << clientid << " connected" << endl;//server.get_num_clients() <<
	}
	void disconnect(uint clientid)
	{
		cout << "Server::Client " << clientid << " disconnected" << endl;
		players.erase(clientid);
		names.erase(clientid);
	}
	void clientupdate(Server &s)
	{
		update_game();
		update_lobby();
	}

	// ------------------------ RPCs ------------------------ //

	void game_player_shoot(uint clientid, vec3 v, vec4 r)
	{
		// create new obj id
		uint id = server.get_new_id(); uint game_id = players[clientid].game;
		// add shot to scene
		games[game_id].game_objects[id] = GameObject(GameObject::Shot, v, r, 100,clientid);
		// create network message
		Message msg = server.msg("game_obj_spawn", id, GameObject::Shot ,"", v, r, 100);
		// broadcast to all players
		for (auto &i : players) if (i.second.game == game_id) server.send_to(i.first, msg);
	}
	void game_player_pos(uint clientid, vec3 v, vec4 r)
	{
		// update self
		GameObject &g = games[players[clientid].game].game_objects[clientid];
		g.pos = v;
		g.rot = r;

		// notify others
		Message m = server.msg("game_obj_set_pos", clientid,v, r);

		for (auto &i : players) if (i.first != clientid)
		{
			int id = i.first; //  Player &p = i.second;

			server.send_to(id, m, 0 /*unrealiable*/);
		}
	}
	void login(uint clientid, string name, string password)
	{
		// check user / password
		names[clientid] = name;
		players[clientid] = Player(id_lobby);
		lobby_join(clientid);
	}
	void lobby_msg(uint clientid, string s)
	{
		for (auto &i : players)
		if ( players[clientid].game == i.second.game )
		{
			server.call(i.first, "lobby_msg", s);
		}
	}
	void game_join(uint clientid, uint id)
	{
		if (names.find(id) == names.end())
		{ 
			cout << "Game " << id << " not found " << endl;
			return; 
		} 
		if (id == id_lobby){ lobby_join(clientid); return; }

		games[players[clientid].game].game_objects.erase(clientid);

		players[clientid].game_state = GAMESTATE_GAME;
		players[clientid].game = id;
		server.call(clientid, "set_game_state", clientid, GAMESTATE_GAME);

		game_player_spawn(clientid);
	}
	void game_player_ready(uint clientid)
	{
		game_player_spawn(clientid);
	}
	// ------------------------ Control Functions ------------------------ //

	void start(	uint port				=12345, 
				uint update_delay_ms	=10 , 
				uint max_connections	=10, 
				uint client_timeout_ms	=2000)
	{
		server = Server(
			port, 				/*port*/
			update_delay_ms,	/*update delay in ms*/
			max_connections,	/*max number of players*/
			client_timeout_ms,	/*timeout before disconnecting player*/
			false,				/*compress data ; must be same as for client*/
			connect,			/*connect callback*/
			disconnect,			/*disconnect callback*/
			clientupdate		/*update callback*/);

		// Register RPCs on Server and Client in same order (!!)
		Rpc &rpc = server.get_rpc();

		rpc_register_remote(rpc, set_game_state);
		rpc_register_remote(rpc, lobby_update);
		rpc_register_remote(rpc, lobby_msg);
		rpc_register_remote(rpc, game_obj_health);
		rpc_register_remote(rpc, game_obj_score);
		rpc_register_remote(rpc, game_obj_spawn);
		rpc_register_remote(rpc, game_obj_set_pos);

		rpc_register_local(rpc, login);
		rpc_register_local(rpc, lobby_msg);
		rpc_register_local(rpc, game_join);
		rpc_register_local(rpc, game_player_ready);
		rpc_register_local(rpc, game_player_shoot);
		rpc_register_local(rpc, game_player_pos);

		     id_lobby = server.get_new_id();
		uint id_game1 = server.get_new_id();
		uint id_game2 = server.get_new_id();

		names[id_lobby] = "Lobby";
		names[id_game1] = "Giant Dungeon";
		names[id_game2] = "Mage Arena";
		games[id_lobby] = Game("lobby");
		games[id_game1] = Game("map1.map");
		games[id_game2] = Game("map2.map");

		server.start();
	}
	void stop()
	{
		server.stop();
	}
};
