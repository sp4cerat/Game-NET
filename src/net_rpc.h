// --- process single datatype ---

//#define RPC_DEBUG

#define RPC_NUM_TYPES 18

const int AnySize[RPC_NUM_TYPES] = {
	1, 1,
	sizeof(char), sizeof(uchar), sizeof(short),
	sizeof(ushort), sizeof(int), sizeof(uint),
	sizeof(double), sizeof(float),
	sizeof(vec2), sizeof(vec3), sizeof(vec4), sizeof(quat),
	sizeof(mat2), sizeof(mat3), sizeof(mat4), 
	1
};
const string AnyStr[RPC_NUM_TYPES] = {
	"String", "Vector",
	"Char", "UChar", "Short",
	"UShort", "Int", "UInt",
	"Double", "Float",
	"Vec2", "Vec3", "Vec4", "Quat",
	"Mat2", "Mat3", "Mat4", "Map"
};

typedef vector < uchar > Message;

struct Any {
	enum Type {
		String = 0, Vector = 1,
		Char = 2, UChar = 3, Short = 4,
		UShort = 5, Int = 6, UInt = 7,
		Double = 8, Float = 9,
		Vec2 = 10, Vec3 = 11, Vec4 = 12, Quat = 13,
		Mat2 = 14, Mat3 = 15, Mat4 = 16, Map = 17
		// >=32 : num = type-32
	};
	union All
	{
		double d; float f; char c;
		short s; ushort us;
		int i; uint ui;
		uint  u64[2];
		uchar uc[16];
	};
	inline void check_double_byte_order(All& a)
	{
		if (htons(1) == 1) return;

		uint   b = a.u64[0];
		a.u64[0] = a.u64[1];
		a.u64[1] = b;

		return;
	}

	Any(void){ _type = 0; };
	Any(double e)
	{
		_num = e; _type = Double;
		if (e - (double)((uchar)e) == 0) { _type = UChar; return; }
		if (e - (double)((char)e) == 0)  { _type = Char; return; }
		if (e - (double)((short)e) == 0) { _type = Short; return; }
		if (e - (double)((ushort)e) == 0){ _type = UShort; return; }
		if (e - (double)((int)e) == 0)   { _type = Int; return; }
		if (e - (double)((uint)e) == 0)  { _type = UInt; return; }
		if (e - (double)((float)e) == 0) { _type = Float; return; }
	}
	//Any(ENetPeer* p){ peer = p; }
	Any(string e) { _str = e; _type = String; }
	Any(vec2 v){ _f[0] = v.x; _f[1] = v.y;  _type = Vec2; };
	Any(vec3 v){ _f[0] = v.x; _f[1] = v.y; _f[2] = v.z;  _type = Vec3; };
	Any(vec4 v){ _f[0] = v.x; _f[1] = v.y; _f[2] = v.z; _f[3] = v.w;  _type = Vec4; };
	Any(quat v){ _f[0] = v.x; _f[1] = v.y; _f[2] = v.z; _f[3] = v.w;  _type = Quat; };
	Any(mat2 m){ _type = Mat2; loopi(0, 2)loopj(0, 2)_f[j * 2 + i] = m[j][i]; };
	Any(mat3 m){ _type = Mat3; loopi(0, 3)loopj(0, 3)_f[j * 3 + i] = m[j][i]; };
	Any(mat4 m){ _type = Mat4; loopi(0, 4)loopj(0, 4)_f[j * 4 + i] = m[j][i]; };
	template<class T, class U>
	Any(map<T, U> m)
	{
		_vec.clear(); _vec2.clear();
		for (auto it = m.begin(); it != m.end(); it++) {
			Any first(it->first);
			Any second(it->second);
			_vec.push_back(first);
			_vec2.push_back(second);
		}
		_type = Map;
	};
	template<class T>
	Any(vector<T> v)
	{
		_vec.clear();
		loopi(0, v.size()) _vec.push_back(v[i]);
		_type = Vector;
	};
	int get_type() const { return _type; }
	string get_type_string(){ return AnyStr[_type]; }
	string get_type_string(int type){ return AnyStr[type]; }
	string get_data_as_string()
	{
		std::ostringstream s;
		if (_type == Char || _type == UChar
			|| _type == Short || _type == UShort
			|| _type == Int || _type == UInt)	s << (int)_num;
		if (_type == Float) s << (float)_num;
		if (_type == Double) s << (double)_num;
		if (_type == Vec2) loopi(0, 2) { s << _f[i] << " "; };
		if (_type == Vec3) loopi(0, 3) { s << _f[i] << " "; };
		if (_type == Vec4) loopi(0, 4) { s << _f[i] << " "; };
		if (_type == Quat) loopi(0, 4) { s << _f[i] << " "; };
		if (_type == Mat2) loopj(0, 2) loopi(0, 2) { s << _f[j * 2 + i] << " "; };
		if (_type == Mat3) loopj(0, 3) loopi(0, 3) { s << _f[j * 2 + i] << " "; };
		if (_type == Mat4) loopj(0, 4) loopi(0, 4) { s << _f[j * 2 + i] << " "; };
		if (_type == String) s << _str;
		if (_type == Vector) { s << "[ "; for(auto i:_vec ) { s << i.get_data_as_string() << " "; }; s << "]"; }
		if (_type == Map) {
			s << "[ "; loopi(0, (int)_vec.size()) {
				s << _vec[i].get_data_as_string() << " = " << _vec2[i].get_data_as_string() << " ";
			}; s << "]";
		}
		return s.str();
	}
	void net_push(vector<uchar> &n)
	{
		//cout << get_type_string() << endl;
		All a; int size = 0;
		n.push_back((int)_type);
		if (_type == UChar)
		{
			if ((uchar)_num < (256 - 32)){ n[n.size() - 1] = ((uchar)_num) + 32; return; };
			a.uc[0] = (uchar)_num; size = sizeof(uchar);
		}
		if (_type == Char)  { a.c = (char)_num; size = sizeof(char); }
		if (_type == Short) { a.s = (short)_num; a.us = htons(a.us);  size = sizeof(short); }
		if (_type == UShort){ a.us = (ushort)_num; a.us = htons(a.us);  size = sizeof(ushort); }
		if (_type == Int)   { a.i = (int)_num; a.ui = htonl(a.ui);  size = sizeof(int); }
		if (_type == UInt)  { a.ui = (uint)_num; a.ui = htonl(a.ui);  size = sizeof(uint); }
		if (_type == Float) { a.f = (float)_num; a.ui = htonl(a.ui);  size = sizeof(float); }
		if (_type == Double){ a.d = (double)_num; loopi(0, 2) a.u64[i] = htonl(a.u64[i]); check_double_byte_order(a); size = sizeof(double); }

		if (size > 0) { loopi(0, size) n.push_back(a.uc[i]); return; }

		if (_type >= Vec2) if (_type <= Mat4)
		{
			uint ui[16]; All a;
			loopi(0, (int)(AnySize[_type] / sizeof(float))) { a.f = _f[i]; ui[i] = htonl(a.ui); }
			loopi(0, (int)(AnySize[_type]))  n.push_back(((uchar*)ui)[i]);
		}

		if (_type == String) { loopi(0, (int)_str.size()) n.push_back(_str[i]); n.push_back(0); }
		if (_type == Vector) { Any len(_vec.size()); len.net_push(n); for(auto i:_vec) i.net_push(n); }
		if (_type == Map) { Any len(_vec.size()); len.net_push(n)   ; for(auto i:_vec) i.net_push(n); for (auto i : _vec2) i.net_push(n); }
	}
	int net_pop(vector<uchar> &n, int index = 0)
	{
		_type = UChar;

		if (index < 0) return -1;
		if (index >= (int)n.size()) return -1;// core_stop("message end reached but parameter wanted");

		All a; int size = 0;
		_type = n[index]; index++;
		if (_type >= RPC_NUM_TYPES){ _num = _type - 32; _type = UChar; return index; }
		
		if (index >= (int)n.size()) return -1;
		
		if (_type == String)
		{
			uint maxlen = n.size() - index;
			if (strnlen((char*)&n[index], maxlen) == maxlen )
				return -1;

			_str = ((char*)&n[index]);
			return index + _str.size() + 1;
		}
		if (_type == Vector)
		{
			Any len; index = len.net_pop(n, index);
			_vec.clear();// resize(int(len._num));
			loopi(0, int(len._num))
			{ 
				Any a; 
				if (index >= (int)n.size()) return -1;
				index = a.net_pop(n, index); 
				if (index <0) return -1;  
				_vec.push_back(a); 
			}
			return index;
		}
		if (_type == Map)
		{
			Any len,a; index = len.net_pop(n, index);
			_vec.clear();// resize(int(len._num));
			_vec2.clear();//.resize(int(len._num));
			loopi(0, int(len._num))
			{
				if (index >= (int)n.size()) return -1;
				index = a.net_pop(n, index);
				if (index <0) return -1;
				_vec.push_back(a);
			}
			loopi(0, int(len._num))
			{
				if (index >= (int)n.size()) return -1;
				index = a.net_pop(n, index);
				if (index <0) return -1;
				_vec2.push_back(a);
			}
			//loopi(0, _vec.size()) { index = _vec[i].net_pop(n, index); }
			//loopi(0, _vec2.size()){ index = _vec2[i].net_pop(n, index); }
			return index;
		}
		if (_type >= Char)if (_type <= Float)
		{
			int size = AnySize[_type];
			if (index + size >(int)n.size()) return -1;
			copy(n.begin() + index, n.begin() + index + size, a.uc);
			//loopi(0, size) a.uc[i] = n[index + i];
			if (_type == UChar) _num = a.uc[0];
			if (_type == Char)  _num = a.c;
			if (_type == Short) { a.us = ntohs(a.us); _num = a.s; }
			if (_type == UShort){ a.us = ntohs(a.us); _num = a.us; }
			if (_type == Int)   { a.ui = ntohl(a.ui); _num = a.i; }
			if (_type == UInt)  { a.ui = ntohl(a.ui); _num = a.ui; }
			if (_type == Float) { a.ui = ntohl(a.ui); _num = a.f; }
			if (_type == Double){ loopi(0, 2) a.u64[i] = ntohl(a.u64[i]); check_double_byte_order(a); _num = a.d; }
			return index + size;
		}
		if (_type >= Vec2) if (_type <= Mat4)
		{
			uint ui[16]; All a;
			if (index + (int)AnySize[_type] > (int)n.size()) return -1;
			loopi(0, AnySize[_type])  ((uchar*)ui)[i] = n[index + i];
			loopi(0, AnySize[_type] / (int)sizeof(float)) { a.ui = ntohl(ui[i]); _f[i] = a.f; }
			return index + AnySize[_type];
		}
		return -1;
	}
	bool is_number(){ if (_type >= Char)if (_type <= Float) return true; return false; };
	bool is_number(int t){ if (t >= Char)if (t <= Float) return true; return false; };
	bool is_type(uint type)
	{
		if (type > 17 || _type > 17) core_stop("type %d  _type %d ", type, _type);
		bool ok = 1;
		if (is_number() != is_number(type)) ok = 0;
		if (!is_number() && type != _type)  ok = 0;
#ifdef RPC_DEBUG
		if (!ok) printf("Rpc::Any::Type Mismatch! \nExpected [ %s ], Received [ %s ]\n", get_type_string(type).c_str() , get_type_string().c_str());
#endif
		return ok;
	};
	bool getT(uchar &a){ a = (uchar)_num;	return is_type(Char); };
	bool getT(char &a){ a = (char)_num;		return is_type(UChar); };
	bool getT(short &a){ a = (short)_num;	return is_type(Short); };
	bool getT(ushort &a){ a = (ushort)_num; return is_type(UShort); };
	bool getT(float &a){ a = (float)_num;	return is_type(Char); };
	bool getT(double &a){ a = (double)_num; return is_type(Char); };
	bool getT(int &a){ a = (int)_num;		return is_type(Char); };
	bool getT(uint &a){ a = (uint)_num;		return is_type(Char); };
	bool getT(vec2 &a){ a = vec2(_f[0], _f[1]);					return is_type(Vec2); };
	bool getT(vec3 &a){ a = vec3(_f[0], _f[1], _f[2]);			return is_type(Vec3); };
	bool getT(vec4 &a){ a = vec4(_f[0], _f[1], _f[3], _f[4]);	return is_type(Vec4); };
	bool getT(quat &a){ a = quat(_f[0], _f[1], _f[3], _f[4]);	return is_type(Quat); };
	bool getT(mat2 &m){ loopi(0, 2)loopj(0, 2) m[j][i] = _f[j * 2 + i]; return is_type(Mat2); };
	bool getT(mat3 &m){ loopi(0, 3)loopj(0, 3) m[j][i] = _f[j * 3 + i]; return is_type(Mat3); };
	bool getT(mat4 &m){ loopi(0, 4)loopj(0, 4) m[j][i] = _f[j * 4 + i]; return is_type(Mat4); };
	bool getT(string &a){ a = _str; return is_type(String); };
	template<class T>
	bool getT(vector<T> &v)
	{
		if (!is_type(Vector)) return 0;

		v.clear();
		loopi(0, (int)_vec.size())
		{
			T t;
			_vec[i].getT(t);
			v.push_back(t);
		}
		return 1;
	};
	template<class T, class U>
	bool getT(map<T, U> &m)
	{
		if (!is_type(Map)) return 0;

		m.clear();
		
		loopi(0, (int)_vec.size())
		{
			T t; U u;
			_vec[i].getT(t);
			_vec2[i].getT(u);
			m[t] = u;
		}
		return 1;
	};
	int get_type(){ return _type; }
private:
	uchar _type;
	string _str;
	double _num;
	float _f[16];
	std::vector<Any> _vec, _vec2; // _vec2 is used for map
};

class Rpc
{
public:
	
	Rpc(){ loopi(0, 2){ _send_data[i].clear(); }; _remote_functions_count = 0; }
	// ------------- UTILITY---------------
	template<int...> struct index_tuple{};

	template<int I, typename IndexTuple, typename... Types>
	struct make_indexes_impl;

	template<int I, int... Indexes, typename T, typename ... Types>
	struct make_indexes_impl < I, index_tuple<Indexes...>, T, Types... >
	{
		typedef typename make_indexes_impl<I + 1, index_tuple<Indexes..., I>, Types...>::type type;
	};

	template<int I, int... Indexes>
	struct make_indexes_impl < I, index_tuple<Indexes...> >
	{
		typedef index_tuple<Indexes...> type;
	};

	template<typename ... Types>
	struct make_indexes : make_indexes_impl < 0, index_tuple<>, Types... > {};

	template <typename T> T static read_from(int index, std::vector<Any> &list)
	{
		T t; list[index].getT(t); return t;
	}

	template <typename T> bool static verify_arg(int index, std::vector<Any> &list)
	{
		T t; bool istype = list[index].getT(t);
		if (!istype)
		{
			#ifdef RPC_DEBUG
			cout << "Error for parameter " << index << endl;
			#endif
		}
		return istype;
	}

	// ---- RPC Data to Params ----
	
	vector< std::function<void(std::vector<uchar>&, int&, string& , uint)> > _functionarray;

	template<typename... Args, class F, int... Is>
	void RegisterRPC_index(std::string name, F f, index_tuple< Is... >)
	{
		const int n = sizeof...(Args);
		//cout << "[" << n << " args]\n";

		_local_functions.push_back(name);

		_functionarray.push_back([f](vector<uchar> &data, int &index, string &name, uint hostid)
		{
			const int n = sizeof...(Args);	//cout << n << " args\n";

			std::vector<Any> list; 
			int count = n; 
			bool server = (hostid != 0xffffffff);

			// server rpcs have client id as first parameter
			if (server){ list.push_back(hostid); count--; } 

			if (index >= (int)data.size()) { index = -1; return; }

			Any any_args; int num_args;
			index = any_args.net_pop(data, index);

			if (index == -1) return;

			any_args.getT(num_args);

			if (count != num_args)
			{
#ifdef RPC_DEBUG
				cout << "RPC " << name << " arg count mismatch " << count << " != " << num_args << endl;
#endif
				index = -1;
				return ;
				//string s, t;
				//loopi(0, list.size())s.append(" " + list[i].get_type_string());
				//printf("RPC: [%s] Expected %d arg(s), received %d [%s ] %s", name.c_str(), n, num_args, s.c_str(), t.c_str());
				//return;
			}

			for (int i = 0; i < num_args; i++)
			{
				Any a;
				if (index >= (int)data.size()) { index = -1; return; }
				index = a.net_pop(data, index);
				if (index < 0) return;
				list.push_back(a);
			}


			std::vector<bool> is_type = { verify_arg<Args>(Is, list)... };

			for (auto i : is_type) if (!i) return; // dont call if type mismatch

			f(read_from<Args>(Is, list)...);
		});
	}

	template<typename... Args, class F>
	void RegisterRPC_no_ret(std::string name, F f)
	{
		RegisterRPC_index<Args...>(name, f, typename make_indexes<Args...>::type());
	}

	// --- RPC Params to Data ---


	template <class ...Args>
	void CallRPC_imp(int flags,const Args&... args)
	{
		int ch	= flags & 1; // 0 = unreliable , 1 = reliable
		
		//cout << "channel:" << ch << endl;

		std::vector<Any> vec = { args... };

		for (auto i : vec) i.net_push(_send_data[ch]);
	};

	template <class ...Args> void call(int flags,string name, Args... args)
	{
		const int n = sizeof...(Args);
		//cout << "calling " << name << " " << n << " args" << endl;
		if (_remote_functions.find(name) != _remote_functions.end())
			CallRPC_imp(flags, Any(_remote_functions[name]), Any(n), Any(args)...);
		else
			core_stop("RPC function %s not found\n", name.c_str());
	};

	template <class ...Args>
	vector<uchar> CallRPC_msg(const Args&... args)
	{
		vector<uchar> m;
		std::vector<Any> vec = { args... };
		loopi(0, (int)vec.size())
		{
			vec[i].net_push(m);
		};
		return m;
	};
	template <class ...Args> vector<uchar> msg(string name, Args... args)
	{
		const int n = sizeof...(Args);
		vector<uchar> m;

		//cout << "calling " << name << " " << n << " args" << endl;
		if (_remote_functions.find(name) != _remote_functions.end())
			m=CallRPC_msg( Any(_remote_functions[name]), Any(n), Any(args)...);
		else
			core_stop("RPC function %s not found", name.c_str());
		return m;
	};

	void process(uchar* data, int size, uint hostid=0xffffffff)
	{
		vector<uchar> rcv;
		rcv.resize(size);
		memcpy(&rcv[0],data,size);
		Any any; int index = 0; uint fi = 0;
		//cout << "void process(uint hostid,uchar* data, int size)" << size << endl;

		while (index < size)
		{
			index = any.net_pop(rcv, index);
			if (index == -1)
			{
				cout << "malformed packet" << endl;
				return;
			}
			any.getT(fi);//function index

			if (fi < _functionarray.size() && fi >= 0)
			{
				//cout << _local_functions[fi] << endl;
				_functionarray[fi](rcv, index, _local_functions[fi], hostid);
				if (index < 0 )
				{
#ifdef RPC_DEBUG
					cout << "malformed packet" << endl;
#endif
					return;
				}
			}
			else
			{
#ifdef RPC_DEBUG
				printf("Server RPC function %d not found\n", fi);
#endif
				return;
			}
		}
	}
	// traits helper templates
	template<typename T>
	struct function_traits;
	template<typename R, typename ...Args>
	struct function_traits < std::function<R(Args...)> >
	{
		static const size_t nargs = sizeof...(Args);
		typedef R result_type;
		template <size_t i>
		struct arg
		{
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
		};
		void static set_rpc(Rpc *rpc,string name, std::function<R(Args...)> f)
		{
			rpc->RegisterRPC_no_ret<Args...>(name, f);
		}	
	};
	void set_remote_functions(vector<string> &list)
	{
		loopi(0, (int)list.size()) _remote_functions[list[i]] = i;
	}
	void get_local_functions(vector<string> &list)
	{
		list = _local_functions;
	}
	void add_remote_function(string s) { _remote_functions[s] = _remote_functions_count; _remote_functions_count++; };
	vector<uchar>* get_send(int i) { return &_send_data[i]; }

	private:
	// channel 0 : non return unreliable
	// channel 1 : non return reliable
	vector<uchar> _send_data[2];
	vector<string> _local_functions;
	map<string,int> _remote_functions;
	int _remote_functions_count;
};//

// --------------------- defines ------------------

#define rpc_register_local(grpc,a)\
{\
	::net::Rpc::function_traits<std::function<decltype(a)>>::set_rpc(&grpc,#a, a);\
}

#define rpc_register_remote(grpc,a)\
{\
	grpc.add_remote_function(  #a );\
}

// ------------------------------------------------
