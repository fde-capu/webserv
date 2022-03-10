//	std::cout << file_conf << std::endl;
//	std::cout << file_conf["server_name"] << std::endl;
//	std::cout << ">" << file_conf["server"] << "<" << std::endl;
//
//	std::string servn = file_conf["server_name"];
//	std::string serv = file_conf["server"];
//	std::cout << "--" << servn << "--" << std::endl;
//	std::cout << "--" << serv << "--" << std::endl;

//	int test_int = file_conf["client_max_body_size"];
//	int test_int = file_conf.getInt("test_port");
	int test_int = file_conf.get<int>("test_port");
	std::cout << "int: " << test_int << std::endl;

	std::vector<int> test_single_vec = file_conf.get<std::vector<int> >("test_port");
	std::cout << "single vec: " << test_single_vec[0] << std::endl;

//	int fail_int = file_conf.get<int>("bye_message");
//	std::cout << "fail: " << fail_int << std::endl;

//	std::string string_int = file_conf.get<std::string>("test_port");
//	std::cout << "string int: " << string_int << std::endl;

	int arr_int = file_conf.get<std::vector<int> >("test_int_array")[1];
	std::cout << "int array: " << arr_int << std::endl;

//	std::string servn = file_conf.get<std::string>("server_name");
//	std::string serv = file_conf["server"];
//	std::cout << "--" << servn << "--" << std::endl;

	vstr aa = file_conf.get<vstr>("test_port");
	std::cout << "aa " << aa[0] << std::endl;
	std::cout << "aa " << aa[1] << std::endl;
	std::cout << "aa " << aa[2] << std::endl;
	std::cout << "aa " << aa[3] << std::endl;

//	std::string serv = file_conf.get<std::string>("server");
//	std::cout << "--" << serv << "--" << std::endl;

	std::vector<std::string> dmsg = file_conf.get<std::vector<std::string> >("double_message");
	std::cout << "dmsg : " << dmsg[0] << std::endl;
	std::cout << "dmsg : " << dmsg[1] << std::endl;

	std::vector<std::string> single_str = file_conf.get<std::vector<std::string> >("bye_message");
	std::cout << "single : " << single_str[0] << std::endl;

//	int sub_int = file_conf.get<int>("server");
//	std::cout << "sub_int: " << sub_int << std::endl;
	// Ok error server is array.

//	std::vector<int> sub_vint = file_conf.get<std::vector<int> >("server");
//	std::cout << "sub_vint: " << sub_vint[0] << std::endl;
	// Ok error server is not a number.

//	std::string sub_str = file_conf.get<std::string>("server");
//	std::cout << "sub_str: " << sub_str << std::endl;
	// Ok error server is array when server is array.
	// Ok error when server is object.

//	std::vector<std::string> sub_vecs = file_conf.get<std::vector<std::string> >("server");
//	std::cout << "sub_vecs: " << sub_vecs[0] << std::endl;
//	std::cout << "sub_vecs: " << sub_vecs[1] << std::endl;
	// ok error server is object.

//	int server_listen = file_conf.get<int>("server", "listen");
//	std::cout << "server_listen: " << server_listen << std::endl;

//	int port = file_conf.get<int>("test_port");
//	std::cout << "port: " << port << std::endl;
	// ok error port is not number (3491 3492)

//	int test_int = file_conf.get<int>("test_port");
//	std::cout << "int: " << test_int << std::endl;
	// ok error if test_port is array 3490 3491

//	vstr test_single_vec = file_conf.get<vstr>("test_port");
//	std::cout << "single vec str: " << test_single_vec[0] << std::endl;
//	
//	vint single_int = file_conf.get<vint>("test_port");
//	std::cout << "single vec int: " << single_int[0] << std::endl;
//	std::cout << "single vec int: " << single_int[1] << std::endl;
//	std::cout << "single vec int: " << single_int[2] << std::endl;
//	std::cout << "single vec int: " << single_int[3] << std::endl;
//	
//	vint inline_vec = file_conf.get<vint>("test_port");
//	std::cout << "inline vec: " << inline_vec[0] << std::endl;
//	std::cout << "inline vec: " << inline_vec[1] << std::endl;
//	std::cout << "inline vec: " << inline_vec[2] << std::endl;
//	std::cout << "inline vec: " << inline_vec[9] << std::endl; // std::vector accepts!
//	std::cout << "size(): " << inline_vec.size() << std::endl;

//	if (!read_conf(conf, DEFAULT_CONFIG_FILE) || !read_conf2(conf, argv[1]) ||
//		!essential_configuration(conf))
//		return !die(BYE);

	std::cout << "as array: " << conf["server_name"] << std::endl;
//	std::cout << conf << std::endl;
//	std::string name = conf.get<std::string>("server_name");
//	std::cout << "name: " << name << std::endl;
