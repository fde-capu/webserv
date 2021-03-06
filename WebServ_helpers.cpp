/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_helpers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 15:25:13 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/03 14:28:13 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

int ws_server_instance::read_more_general()
{
	if (is_chunked() && is_multipart())
		return 422;
	if (!is_chunked() || !is_multipart())
		read_more_plain();
	if (is_chunked())
		read_more_chunked();
	if (is_multipart())
		read_more_multipart();
	return 0;
}

void ws_server_instance::read_more_plain()
{
	static int V(1);
	size_t next_load;
	CircularBuffer buf(fd);

	if (in_body.length() > max_size)
		return ;
	do
	{
		next_load = max_size - in_body.length();
		in_body = buf.receive_until_eof(next_load);
		set_sizes();
		if (exceeded_limit)
		{
			verbose(V) << "(read_more_plain) Multipart content exceeded limit." \
				<< std::endl;
			break ;
		}
		if (buf.ended())
		{
			verbose(V) << "(read_more_plain) No more data." << std::endl;
			break ;
		}
	}
	while (buf.length() < next_load);

	verbose(V) << "(read_more_plain) Finished with body " << in_body.length() << \
		" and multipart-content " << multipart_content.length() << "." << \
		std::endl;
}

void ws_server_instance::read_more_chunked()
{
	static int V(1);
	verbose(V) << "(read_more_chunked) WILL READ MORE CHUNKS." << std::endl;
}


void ws_server_instance::read_more_multipart()
{
	static int V(1);
	size_t next_load;
	CircularBuffer buf(fd);

	if (in_body.length() > static_cast<size_t>(in_header.content_length))
		return ;
	do
	{
		next_load = in_header.content_length - in_body.length();
		in_body = buf.receive_at_most(next_load);
		mount_multipart();
		if (exceeded_limit)
		{
			verbose(V) << "(read_more_multipart) Multipart content exceeded limit." \
				<< std::endl;
			break ;
		}
		if (buf.ended())
		{
			verbose(V) << "(read_more_multipart) No more data." << std::endl;
			break ;
		}
	}
	while (buf.length() < next_load);

	verbose(V) << "(read_more_multipart) Finished with body " << in_body.length() << \
		" and multipart-content " << multipart_content.length() << "." << \
		std::endl;
}

bool WebServ::is_port_taken(int port) const
{
	for (size_t i = 0; i < taken_ports.size(); i++)
		if (taken_ports[i] == port)
			return true;
	return false;
}

bool WebServ::same_port_another_name(const ws_server_instance* candidate) const
{
	bool out = false;
	for (size_t i = 0; i < instance.size(); i++)
	{
		for (size_t j = 0; j < instance[i].port.size(); j++)
		{
			for (size_t k = 0; k < candidate->port.size(); k++)
			{
				if ((instance[i].port[j] == candidate->port[k]) && \
						(instance[i].config.getValStr("server_name") \
						 != candidate->config.getValStr("server_name")))
					out = true;
			}
		}
	}
	return out;
}

ws_server_instance WebServ::dftosi(DataFold df)
{
	ws_server_instance si;
	int port;
	DataFold df_listen(df.get("listen"));
	while (df_listen.loop())
	{
		port = std::atoi(df_listen.val.c_str());
		si.port.push_back(port); // htons?
		si.config = df;
	}
	return si;
}

struct pollfd WebServ::stdin_to_pollfd()
{ return make_pollin_fd(0); }

struct pollfd WebServ::make_pollin_fd(int newfd) const
{
	struct pollfd ufds;

	ufds = pollfd();
	ufds.fd = newfd;
	ufds.events = POLLIN;
	return ufds;
}

void WebServ::exit_gracefully()
{
	flush_stdin();
	verbose(1) << "(webserv) Exit gracefully. Thanks!" << std::endl;
	verbose(1) << config.getValStr("bye_message") << std::endl;
	lit = false;
	return ;
}

bool WebServ::there_is_an_instance(int fd) const
{
	for (size_t i = 0; i < instance.size(); i++)
		for (size_t j = 0; j < instance[i].listen_sock.size(); j++)
			if (instance[i].listen_sock[j] == fd)
				return true;
	return false;
}

bool WebServ::validate_header_1st_line
	(std::string& test_str, size_t expected_size, bool& is_valid)
{
	std::vector<std::string> carrier;

	carrier = split_trim(test_str, " ");
	is_valid = true;
	return validate_header_entry(carrier, expected_size, is_valid);
}

bool WebServ::validate_header_entry
	(std::vector<std::string>& test, size_t expected_size, bool& is_valid)
{
	is_valid = is_valid && test.size() == expected_size;
	return is_valid;
}

ws_header::ws_header()
{
	port = 0;
	is_valid = false;
	status = 0;
	content_length = 0;
}

bool WebServ::ignore_empty(std::string& line)
{ return line.length() == 0; }

bool WebServ::read_1st_line(std::string& line, ws_header& header, bool& is_valid)
{
	std::vector<std::string> carrier;

	verbose(4) << "(read_1st_line) >" << line << "<" << std::endl;
	if (validate_header_1st_line(line, 3, is_valid))
	{
		carrier = split_trim(line, " ");
		header.method = carrier[0];
		header.directory = carrier[1];
		header.protocol = carrier[2];
	}
	else
		is_valid = false;
	return is_valid;
}

bool WebServ::read_host(std::string& line, ws_header& header, bool& is_valid)
{
	std::vector<std::string> carrier;
	bool prev_valid = is_valid;

	carrier = split_trim(line, ":");
	if (is_equal_insensitive(carrier[0], "host"))
	{
		if (validate_header_entry(carrier, 3, is_valid))
		{
			header.host = carrier[1];
			header.port = atoi(carrier[2].c_str());
		}
		else
		{
			is_valid = prev_valid;
			if (validate_header_entry(carrier, 2, is_valid))
				header.host = carrier[1];
		}
		return is_valid;
	}
	return false;
}

struct ws_header WebServ::get_header(const std::string& full_file)
{
	static int V(1);
	ws_header header;
	std::vector<std::string> line;
	std::vector<std::string> carrier;
	bool is_valid;
	is_valid = false;
	size_t header_break;

	header_break = full_file.find("\r\n\r\n");
	if (header_break == std::string::npos)
		return header;
	std::string h_block = full_file.substr(0, header_break);
	line = split_trim(h_block, "\r\n");
	for (size_t i = 0; i < line.size(); i++)
	{
		verbose(V) << "(webserv) LINE >>" << line[i] << "<<" << std::endl;
		if (ignore_empty(line[i])) continue ;
		if (i == 0 && read_1st_line(line[i], header, is_valid)) continue ;
		if (read_host(line[i], header, is_valid)) continue ;
		carrier = split_trim(line[i], ":");
		if (!validate_header_entry(carrier, 2, is_valid)) continue ;
		if (is_equal_insensitive(carrier[0], "user-agent"))
			header.user_agent = carrier[1];
		if (is_equal_insensitive(carrier[0], "accept"))
			header.accept = carrier[1];
		if (is_equal_insensitive(carrier[0], "content-length"))
			header.content_length = atoi(carrier[1].c_str());
		if (is_equal_insensitive(carrier[0], "content-type"))
			header.content_type = carrier[1];
		if (is_equal_insensitive(carrier[0], "transfer-encoding"))
			header.transfer_encoding = carrier[1];
		if (is_equal_insensitive(carrier[0], "expect"))
			header.expect = carrier[1];
	}
	header.is_valid = is_valid;
	verbose(V) << "(get_header) " << header << std::endl;
	return header;
}

std::string WebServ::get_body(const std::string& full_file)
{
	size_t pos = full_file.find("\r\n\r\n");
	return pos == std::string::npos ? "" : full_file.substr(pos + 4);
}

std::string WebServ::get_raw_data(int fd)
{
	CircularBuffer buffer(fd);
	buffer.receive_until_eof();
	verbose(5) << "(get_raw_data) EOF? " << buffer.ended() << std::endl;
	std::string raw_data(buffer.output);
	verbose(6) << "(get_raw_data) RAW_DATA >>" << raw_data << "<<" << std::endl;
	return raw_data;
}

WebServ::WebServ(WebServ const & src)
{
	*this = src;
	return ;
}

DataFold WebServ::getConfig() const
{ return config; }

DataFold WebServ::getServer() const
{ return server; }

std::vector<ws_server_instance> WebServ::getInstance() const
{ return instance; }

WebServ & WebServ::operator= (WebServ const & rhs)
{
	if (this != &rhs)
	{
		config = rhs.getConfig();
		server = rhs.getServer();
		instance = rhs.getInstance();
	}
	return *this;
}

std::ostream & operator<< (std::ostream & o, WebServ const & self)
{
	o << "::WebServ::" << std::endl;
	(void)self;
	return o;
}

WebServ::~WebServ()
{
	return ;
}

void WebServ::flush_stdin()
{
	std::string line;
	std::getline(std::cin, line);
}

std::ostream & operator<< (std::ostream & o, ws_header const & wsh)
{
	o << "ws_header | method     | " << wsh.method << std::endl;
	o << "ws_header | directory  | " << wsh.directory << std::endl;
	o << "ws_header | protocol   | " << wsh.protocol << std::endl;
	o << "ws_header | host       | " << wsh.host << std::endl;
	o << "ws_header | port       | " << wsh.port << std::endl;
	o << "ws_header | user_agent | " << wsh.user_agent << std::endl;
	o << "ws_header | accept     | " << wsh.accept << std::endl;
	o << "ws_header | is_valid   | " << wsh.is_valid << std::endl;
	o << "ws_header | status     | " << wsh.status << std::endl;
	o << "ws_header | status_msg | " << wsh.status_msg << std::endl;
	o << "ws_header | connection | " << wsh.connection << std::endl;
	o << "ws_header | con-length | " << wsh.content_length << std::endl;
	o << "ws_header | con-type   | " << wsh.content_type << std::endl;
	o << "ws_header | transf-enc | " << wsh.transfer_encoding << std::endl;
	o << "ws_header | expect     | " << wsh.expect << std::endl;
	return o;
}

std::ostream & operator<< (std::ostream & o, ws_server_instance const & wssi)
{
	o << "ws_server_instance | port        | ";
	for (size_t i = 0; i < wssi.port.size(); i++)
	{ o << wssi.port[i] << " "; }
	o << std::endl;
	o << "ws_server_instance | listen_sock | ";
	for (size_t i = 0; i < wssi.listen_sock.size(); i++)
	{ o << wssi.listen_sock[i] << " "; }
	o << std::endl;
	o << "ws_server_instance | config      | " << wssi.config << std::endl;
	o << "ws_server_instance | in_header   :" << std::endl << wssi.in_header \
		<< std::endl;
	if (wssi.in_body.length() < 512)
	{
		o << "ws_server_instance | in_body     :" << std::endl << ">>" << \
			wssi.in_body << "<<" << std::endl;
	}
	else
	{
		o << "ws_server_instance | in_body     :" << std::endl << ">>" << \
			wssi.in_body.substr(0, 500) << "(...)<< len " \
			<< wssi.in_body.length() << "." << std::endl;
	}
	o << "ws_server_instance | multipart_type | " << wssi.multipart_type \
		<< std::endl;
	o << "ws_server_instance | multipart_content_disposition | " << \
		wssi.multipart_content_disposition << std::endl;
	o << "ws_server_instance | multipart_name | " << wssi.multipart_name \
		<< std::endl;
	o << "ws_server_instance | multipart_filename | " \
		<< wssi.multipart_filename << std::endl;
	o << "ws_server_instance | multipart_content_type | " \
		<< wssi.multipart_content_type << std::endl;
	o << "ws_server_instance | boundary >>" << wssi.boundary << "<<" << std::endl;
	o << "ws_server_instance | max_size | " << wssi.max_size << std::endl;
	return o;
}

const DataFold ws_server_instance::operator[] (std::string df_query) const
{ return this->config.get_val(df_query); }

std::string ws_server_instance::val(std::string key) const
{ return this->config.get<DataFold>(key)[key]; }

void ws_reply_instance::set_code(int code_n, const std::string& u_output)
{
	out_header.status = code_n;
	out_header.status_msg = u_output;
	verbose(1) << "(webserv) " << code_n << " " << u_output << std::endl;
}

void ws_reply_instance::set_redirect(const std::string& target)
{ out_header.location = target; }

void WebServ::load_defaults()
{
	if (config.getValStr("server_name") == "")
		config.set("server_name", DEFAULT_SERVER_NAME);
	if (config.getValStr("welcome_message") == "")
		config.set("welcome_message", DEFAULT_WELCOME_MESSAGE);
	if (config.getValStr("bye_message") == "")
		config.set("bye_message", DEFAULT_BYE);
	if (config.getValStr("index") == "")
		config.set("index", DEFAULT_INDEX);
}

std::string ws_reply_instance::encapsulate()
{
	std::string out = "";

	out += out_header.protocol + " ";
	out += itoa(out_header.status) + " " + out_header.status_msg + "\n";
	if (out_header.connection != "")
		out += "Connection: " + out_header.connection + "\n";
	out += "Content-Length: " + itoa(out_body.length()) + "\n";
	if (out_header.location != "")
		out += "Location: " + out_header.location + "\n";
	out += "\n";
	out += out_body;
	package_length = out.length();
	return out;
}

void ws_header::header500()
{
	method = "";
	directory = "";
	protocol = "HTTP/1.1";
	status = 500;
	status_msg = "Internal Server Error";
	connection = "close";
}

ws_reply_instance::ws_reply_instance()
{
	out_header.header500();
}

bool ws_server_instance::is_multipart() const
{ return in_header.content_type.find("multipart") == 0; }

bool ws_server_instance::is_chunked() const
{ return stool.is_equal_insensitive(in_header.transfer_encoding, "chunked"); }

void WebServ::respond_timeout(int fd)
{
	ws_reply_instance respond;
	respond.set_code(408, "Request Timeout");
	respond.out_body = "BODY FOR 408";
	if (send(fd, respond.encapsulate().c_str(),
		respond.package_length, 0) == -1)
		throw std::domain_error("(webserv) Sending response went wrong.");
	close(fd);
	remove_from_poll(fd);
}

void ws_server_instance::mount_multipart()
{
	set_sizes();
	multipart_content = in_body.substr(body_start, body_end - body_start);
	exceeded_limit = multipart_content.length() > max_size;
}

void ws_server_instance::set_sizes()
{
	static int V(1);

	max_size = std::atoi(location_get_single("client_max_body_size", \
				itoa(DEFAULT_MAX_BODY_SIZE)).c_str());
	if (is_multipart())
	{
		multipart_type = word_from(in_header.content_type,
				in_header.content_type.find("/") + 1);
		boundary = in_header.content_type.substr( \
				(in_header.content_type.find("boundary=") + 9));
		payload_start = in_body.find(boundary);
		if (payload_start == std::string::npos)
			payload_start = 0;
		else
			payload_start = in_body.find("\r\n", payload_start \
					+ boundary.length()) + 2;
		payload_end = in_body.find(boundary, payload_start);
		payload_end = payload_end == std::string::npos ? \
					  in_body.length() : payload_end - 4;
		body_start = in_body.find("\r\n\r\n", payload_start);
		body_start = body_start == std::string::npos ? 0 : body_start;
		body_start += body_start ? 4 : 0;
		body_end = payload_end;

		multipart_content_disposition = StringTools::query_for( \
				"Content-Disposition", in_body);
		multipart_name = StringTools::query_for("name", in_body);
		multipart_filename = StringTools::query_for("filename", in_body);
		multipart_content_type = StringTools::query_for("Content-Type", in_body);
		exceeded_limit = multipart_content.length() > max_size;
	}
	else if (is_chunked())
	{
		verbose(V) << "(set_sizes) WILL SET SIZE FOR CHUNKED" << std::endl;
		exceeded_limit = chunked_content.length() > max_size;
	}
	else
	{
		payload_start = 0;
		payload_end = in_body.length();
		body_start = payload_start;
		body_end = payload_end;
		exceeded_limit = in_body.length() > max_size;
	}
}

DataFold ws_server_instance::get_location_config() const
{
	DataFold locations(config.get<DataFold>("location"));
	if (locations.empty()) return locations;
	while (locations.loop())
		if (DataFold(locations.val).getValStr("uri") == in_header.directory)
			return locations.val;
	return locations;
}

DataFold ws_server_instance::location_get(const std::string& key, \
	std::string u_default) const
{
	static int V(2);
	DataFold locations(config.get<DataFold>("location"));
	DataFold loc;
	DataFold out;

	out = std::string(key + ":" + u_default);
	verbose(V) << "(location_get) default: " << out << std::endl;
	if (root_config.getValStr(key) != "")
	{
		out = root_config.get(key);
		verbose(V) << "(location_get) root_config: " << out << std::endl;
	}
	if (config.get(key) != "")
	{
		out = config.get(key);
		verbose(V) << "(location_get) config.get: " << out << std::endl;
	}

	while (locations.loop())
	{
		loc = locations.val;
		if (StringTools::startsWith(in_header.directory, \
			loc.getValStr("uri")))
		{
			while (loc.loop())
				if (loc.key == key)
				{
					out = loc.get(key);
					verbose(V) << "(location_get) location: " << out << std::endl;
				}
		}
	}
	verbose(V) << "(location_get) Return: " << out << std::endl;
	return out;
}

std::string ws_server_instance::location_get_single \
	(const std::string& key, std::string u_default) const
{
	DataFold x(location_get(key, u_default));
	if (x.loop())
		return x.val.c_str();
	return 0;
}

std::string ws_server_instance::location_path(const std::string& default_file) const
{
	std::string html_dir = config.getValStr("root");
	std::string uri2root = location_get_single("root", default_file);
	std::string sys_dir = root_config.getValStr("root");
	std::string get_request = in_header.directory;
	std::string converted = get_request;
	std::string trunk = get_request;
	std::string full_path;
	size_t h;

	verbose(3) << "(location_path) sys_dir: " << sys_dir << std::endl;
	verbose(3) << "(location_path) html_dir: " << html_dir << std::endl;
	verbose(3) << "(location_path) uri2root: " << uri2root << std::endl;
	verbose(3) << "(location_path) default_file: " << default_file << std::endl;
	verbose(3) << "(location_path) get_request: " << get_request << std::endl;

	h = trunk.find("/", 1);
	if (h != std::string::npos)
		trunk = trunk.substr(1, h - 1);

	verbose(3) << "(location_path) trunk: " << trunk << std::endl;

	converted = stool.substitute_all(converted, trunk, uri2root);
	if (html_dir != uri2root)
		converted = html_dir + "/" + converted;
	full_path = sys_dir + "/" + converted;
	stool.remove_rep_char(full_path, '/');

	if (FileString::is_dir(full_path))
	{
		full_path += "/" + default_file;
		stool.remove_rep_char(full_path, '/');
		verbose(2) << "(location_path) Returns (is directory, append file): " << full_path << "." << std::endl;
		return full_path;
	}
	verbose(2) << "(location_path) Returns (file from header): " << full_path << "." << std::endl;
	return full_path;
}
