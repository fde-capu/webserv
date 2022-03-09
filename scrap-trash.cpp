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

//	std::string serv = file_conf.get<std::string>("server");
//	std::cout << "--" << serv << "--" << std::endl;

	std::vector<std::string> dmsg = file_conf.get<std::vector<std::string> >("double_message");
	std::cout << "dmsg : " << dmsg[0] << std::endl;
	std::cout << "dmsg : " << dmsg[1] << std::endl;

	std::vector<std::string> single_str = file_conf.get<std::vector<std::string> >("bye_message");
	std::cout << "single : " << single_str[0] << std::endl;
