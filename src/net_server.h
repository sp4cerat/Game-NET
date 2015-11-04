class Server
{
public:

	void send_to(int id, Message m, int reliable = 1 /* 0=unrelible 1=reliable */){ _mailbox.send_to(id, 1, m); };
	template <class ...Args> Message msg(string name, Args... args){ return _rpc.msg(name, args ...); }
	template <class ...Args> void call(uint id, string name, Args... args){ send_to(id, _rpc.msg(name, args ...)); }
	template <class ...Args> void call_ex(uint reliable /* 0=unrelible 1=reliable */, uint id, string name, Args... args){ send_to(id, _rpc.msg(name, args ...), reliable); }
	
	Server(
		uint port = 12345,
		uint update_delay = 10,
		uint max_connections = 32,
		uint client_timeout = 2000,
		bool compress_data = false,
		void c(uint clientid)=0,
		void d(uint clientid) = 0,
		void u(Server &s) = 0
		)
	{
		_port = port;
		_update_delay = update_delay;
		_onConnect = c;
		_onDisconnect = d;
		_onUpdate = u;
		_quit = 0;
		_max_connections = max_connections;
		_client_timeout = client_timeout;
		_connected_clients = 0; 
		_compress_data = compress_data;
	}

	uint get_num_clients(){ return _connected_clients; };
	uint get_id(ENetPeer* host){ return ((uint)host->data); };
	void set_id(ENetPeer* host, uint id){ host->data = (void*)(id); };

	uint get_new_id(){ static uint count = 0; count++; return count; };

	void main_loop()
	{
		if (enet_initialize() != 0)
		{
			core_stop("An error occurred while initializing ENet.");
		}

		ENetAddress address;

		address.host = ENET_HOST_ANY;
		address.port = _port;
		_server = enet_host_create(&address /* the address to bind the _server host to */,
			_max_connections	/* allow up to 32 clients and/or outgoing connections */,
			4					/* allow up to 2 channels to be used, 0 and 1 */,
			0					/* assume any amount of incoming bandwidth */,
			0					/* assume any amount of outgoing bandwidth */);
		if (_server == NULL)
		{
			printf("Server not created - maybe up already?\n");
			return;
		}

		if (_compress_data)
		{
			enet_host_compress_with_range_coder(_server);
		}

		printf("Server::started\n");

		uint _server_time = core_time();

		while (!_quit)
		{
			while (1)
			{
				uint t = core_time();
				if (t - _server_time >= _update_delay)
				{
					_server_time = t;
					break;
				}
				core_sleep(1);
			}

			if (_onUpdate) _onUpdate(*this);
			_mailbox.process();
			
			ENetEvent event;
			if (enet_host_service(_server, &event, _update_delay) <= 0)continue;

			if(event.type == ENET_EVENT_TYPE_CONNECT)
			{
				_connected_clients++;
				uint id = get_new_id();
				set_id(event.peer, id);
				_mailbox[id].peer = event.peer;
				if (_onConnect) _onConnect(get_id(event.peer));
				enet_peer_timeout(event.peer, _client_timeout, 0, _client_timeout);
				enet_packet_destroy(event.packet);
			}
			if (event.type == ENET_EVENT_TYPE_RECEIVE)
			{
				//cout << "rec " << event.packet->dataLength << endl;
				if (event.packet->dataLength > 0)
					_rpc.process(event.packet->data, event.packet->dataLength, get_id(event.peer));
				enet_packet_destroy(event.packet);
			}
			if (event.type == ENET_EVENT_TYPE_DISCONNECT)
			{
				_connected_clients--;
				if (_onDisconnect) _onDisconnect(get_id(event.peer));
				_mailbox.erase(get_id(event.peer));
			}
			//enet_host_flush(_server);
		}
		enet_host_destroy(_server);
		
	};
	void start(){ _quit = 0; _server_thread = make_shared<thread>(&Server::main_loop, this); };
	void stop() { _quit = 1; _server_thread->join(); };

	Rpc& get_rpc(){ return _rpc; }

	private:

	Rpc _rpc;
	ENetHost *_server;
	bool _quit;
	uint _connected_clients;
	uint _update_delay;
	uint _port;

	uint _max_connections;
	uint _client_timeout;

	void(*_onConnect)(uint clientid);
	void(*_onDisconnect)(uint clientid);
	void(*_onUpdate)(Server &s);

	struct Mailbox
	{
		struct Box
		{
			ENetPeer* peer;
			vector<Message> out_rel; // reliable
			vector<Message> out_unr; // unreliable
		};
		map<uint, Box> _box;

		void erase(uint i){ _box.erase(i); }
		Box& operator[](uint i){ return _box[i]; }
		void send_to(int id, int reliable, Message m)
		{
			//cout << " send_to " << m.size() << " bytes" << endl;
			if (reliable)
				_box[id].out_rel.push_back(m);
			else
				_box[id].out_unr.push_back(m);
		};
		void send(ENetPeer *peer, vector<uchar> data, int channel, bool reliable = 1)
		{
			if (data.size() == 0) return;
			//cout << " sending " << data.size() << " bytes" << endl;
			ENetPacket * packet = enet_packet_create(&data[0], data.size(),
				reliable ? ENET_PACKET_FLAG_RELIABLE :
				ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
			enet_peer_send(peer, channel, packet);
		}
		void process()
		{
			for (auto &i : _box)
			{
				int id = i.first;
				Box& b = i.second;

				Message m;
				loopj(0, b.out_rel.size())
				{
					//cout << "b.out_rel.size())"<< endl;
					loopk(0, b.out_rel[j].size())
						m.push_back(b.out_rel[j][k]);
				}
				send(b.peer, m, 0, 1); // reliable

				m.clear();
				loopj(0, b.out_unr.size())
					loopk(0, b.out_unr[j].size())
					m.push_back(b.out_unr[j][k]);

				send(b.peer, m, 0, 0); // unrealiable

				b.out_rel.clear();
				b.out_unr.clear();
			}
		}
	};
	Mailbox _mailbox;

	shared_ptr<thread> _server_thread;
	bool _compress_data;
};
