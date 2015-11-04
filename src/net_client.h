class Client
{
	uint _client_id; // server side id of this client
	ENetHost * _client;
	ENetAddress _address;
	ENetEvent _event;
	ENetPeer *_peer;
	bool _connected;
	Rpc _rpc;

public:

	bool connected(){ return _connected; }

	Rpc& get_rpc(){ return _rpc; }

	Client(){ _client = 0; _peer = 0;  _connected = 0; }

	uint get_id(){ return _client_id; }

	bool connect(string host, int port, int bps_down = 0, int bps_up = 0, uint timeout = 5000, bool compress_data=false)
	{
		if (enet_initialize() != 0)
		{
			cout << "Client::An error occurred while initializing ENet.\n";
			return false;
		}
		_client = enet_host_create(NULL /* create a client host */,
			1 /* only allow 1 outgoing connection */,
			4 /* allow up 2 channels to be used, 0 and 1 */,
			bps_down / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
			bps_up / 8 /* 56K modem with 14 Kbps upstream bandwidth */);
		if (_client == NULL)
		{
			cout << "Client::An error occurred while trying to create an ENet client host.\n";
			return false;
		}
		/* Connect to some.server.net:1234. */
		enet_address_set_host(&_address, host.c_str());
		_address.port = port;
		/* Initiate the connection, allocating the two channels 0 and 1. */
		_peer = enet_host_connect(_client, &_address, 4, 0);
		if (_peer == NULL)
		{
			cout << "Client::No available peers for initiating an ENet connection.\n";
			return false;
		}
		if (compress_data)
		{
			enet_host_compress_with_range_coder(_client);
		}

		/* Wait up to 5 seconds for the connection attempt to succeed. */
		if (enet_host_service(_client, &_event, timeout) > 0 &&
			_event.type == ENET_EVENT_TYPE_CONNECT)
		{
			cout << "Client::Connected" << endl;
		}
		else
		{
			/* Either the timeout mseconds are up or a disconnect event was */
			/* received. Reset the peer in the event the 5 seconds   */
			/* had run out without any significant event.            */
			enet_peer_reset(_peer);
			cout << "Client::Connection to " << host << ":" << port << " failed." << endl;
			return false;
		}
		
		_connected = 1;

		enet_peer_timeout(_peer, timeout, 0, timeout);
	}

	void disconnect()
	{
		enet_peer_disconnect_now(_peer, 0);
		enet_host_destroy(_client);
		enet_deinitialize();
		_connected = 0;
	}
	void send(vector<uchar> &data, bool reliable = 1)
	{
		if (!_connected)return;
		if (data.size() == 0) return;

		ENetPacket * packet = enet_packet_create(&data[0], data.size(),
			reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);

		enet_peer_send(_peer, 0 /*channel*/ , packet);
	}
	void flush_send()
	{
		loopi(0, 2)
		{
			vector<uchar> &snd = *(_rpc.get_send(i));
			if (snd.size() > 0)
			{
				send(snd, i);
				snd.clear();
			}
		}
	}
	void process()
	{
		flush_send();
		while (enet_host_service(_client, &_event, 0) > 0)
		{
			//if (_event.type == ENET_EVENT_TYPE_CONNECT){}
			if (_event.type == ENET_EVENT_TYPE_DISCONNECT)
			{
				printf("server disconnected.\n");
				_connected = 0;
			}
			if (_event.type == ENET_EVENT_TYPE_RECEIVE)
			{
				//cout << "packet received" << endl;
				if (_event.packet->dataLength > 0)
					_rpc.process( _event.packet->data, _event.packet->dataLength);
				enet_packet_destroy(_event.packet);
			}
		}
	}
	template <class ...Args> void call(string name, Args... args)
	{
		_rpc.call(1, name, args...);
	};
	template <class ...Args> void call_ex(int reliable /* 0=unrelible 1=reliable */ , string name, Args... args)
	{
		_rpc.call(reliable, name, args...);
	};
};

