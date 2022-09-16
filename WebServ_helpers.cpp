/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_helpers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 15:25:13 by fde-capu          #+#    #+#             */
/*   Updated: 2022/09/16 04:50:08 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

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

bool WebServ::ignore_empty(std::string& line)
{ return line.length() == 0; }

std::string WebServ::get_body(const std::string& full_file)
{
	size_t pos = full_file.find("\r\n\r\n");
	return pos == std::string::npos ? "" : full_file.substr(pos + 4);
}

DataFold WebServ::getConfig() const
{ return config; }

DataFold WebServ::getServer() const
{ return server; }

std::vector<ws_server_instance> WebServ::getInstance() const
{ return instance; }

WebServ::~WebServ()
{ return ; }

void WebServ::flush_stdin()
{
	std::string line;
	std::getline(std::cin, line);
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
	out += itoa(out_header.status) + " " + out_header.status_msg + "\r\n";
	if (out_header.connection != "")
		out += "Connection: " + out_header.connection + "\r\n";
	out += "Content-Length: " + itoa(out_body.length()) + "\r\n";
	if (out_header.location != "")
		out += "Location: " + out_header.location + "\r\n";
	if (out_header.content_type != "")
	{
		out += "Content-Type: " + out_header.content_type;
		if (out_header.charset != "")
			out += "; charset=" + out_header.charset;
		out += "\r\n";
	}
	else
		out += "Content-Type: text/html; charset=utf-8\r\n";
	out += "\r\n";
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

void ws_server_instance::set_props()
{
	max_size = std::atoi(location_get_single("client_max_body_size", \
				itoa(DEFAULT_MAX_BODY_SIZE)).c_str());
	if (is_multipart())
	{
		multipart_type = word_from(in_header.content_type,
				in_header.content_type.find("/") + 1);
		boundary = in_header.content_type.substr( \
				(in_header.content_type.find("boundary=") + 9));
	}
}

void ws_server_instance::set_sizes()
{
	static int V(4);
	size_t payload_start, payload_end;

	if (is_multipart())
	{
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

		verbose(V) << "(set_sizes:multipart) max_size:" << max_size << \
			" multipart len:" \
			<< multipart_content.length() << \
			" body len:" << in_body.length() << \
			" declared len:" \
			<< static_cast<size_t>(in_header.content_length) << std::endl;

		exceeded_limit = max_size && body_end - body_start > max_size;
		exceeded_limit = exceeded_limit || \
			in_body.length() > static_cast<size_t>(in_header.content_length);
		reached_limit = in_body.length() == static_cast<size_t>(in_header.content_length);
	}
	else if (is_chunked())
	{
		exceeded_limit = max_size && chunked_content.length() > max_size;
		reached_limit = max_size && chunked_content.length() == max_size;
	}
	else
	{
		payload_start = 0;
		payload_end = in_body.length();
		body_start = payload_start;
		body_end = payload_end;
		exceeded_limit = max_size && in_body.length() > max_size;
		exceeded_limit = exceeded_limit || \
			in_body.length() > static_cast<size_t>(in_header.content_length);
		reached_limit = in_body.length() == static_cast<size_t>(in_header.content_length);
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

DataFold ws_server_instance::server_location_config(const std::string& key, \
	std::string u_default) const
{
	static int V(3);
	DataFold locations(config.get<DataFold>("location"));
	DataFold loc;
	DataFold out;

	out = std::string(key + ":" + u_default);
	verbose(V) << "(server_location_config) default: " << out << std::endl;
	if (root_config.getValStr(key) != "")
	{
		out = root_config.getValStr(key);
		verbose(V) << "(server_location_config) root_config.get(" << key << "): " << out << std::endl;
	}
	if (config.get(key) != "")
	{
		out = config.get(key);
		verbose(V) << "(server_location_config) config.get: " << out << std::endl;
	}

	while (locations.loop())
	{
		loc = locations.val;
		if (StringTools::startsWith(in_header.directory, loc.getValStr("uri")))
		{
			while (loc.loop())
				if (loc.key == key)
				{
					out = loc.get(key);
					verbose(V) << "(server_location_config) location: " << out << std::endl;
				}
		}
	}
	verbose(V) << "(server_location_config) Return: " << out << std::endl;
	return out;
}

std::string ws_server_instance::location_get_single \
	(const std::string& key, std::string u_default) const
{
	DataFold x(server_location_config(key, u_default));
	if (x.loop())
		return x.val.c_str();
	return 0;
}

std::string ws_server_instance::location_path(const std::string default_file) const
{
	static int V(3);
	std::string html_dir = config.getValStr("root");
	std::string uri2root = location_get_single("root", default_file);
	std::string sys_dir = root_config.getValStr("root");
	std::string get_request = in_header.directory;
	std::string trunk = get_request;
	std::string full_path;
	size_t h;

	verbose(V) << "(location_path) sys_dir: " << sys_dir << std::endl;
	verbose(V) << "(location_path) html_dir: " << html_dir << std::endl;
	verbose(V) << "(location_path) uri2root: " << uri2root << std::endl;
	verbose(V) << "(location_path) default_file: " << default_file << std::endl;
	verbose(V) << "(location_path) get_request: " << get_request << std::endl;

	h = trunk.find("/", 2);
	trunk = h && h != std::string::npos ? trunk.substr(h + 1) : "";

	verbose(V) << "(location_path) trunk: " << trunk << std::endl;

	if (uri2root == html_dir)
		full_path = sys_dir + "/" + html_dir + "/" + trunk;
	else
		full_path = sys_dir + "/" + html_dir + "/" + uri2root + "/" + trunk;

	if (uri2root == html_dir)
		full_path += "/" + get_request;

	verbose(V) << "(location_path) full location: " << full_path << std::endl;

	if (FileString::is_dir(full_path))
	{
		full_path += "/" + default_file;
		stool.remove_rep_char(full_path, '/');
		verbose(V) << "(location_path) Returns (is directory, append file): " \
			<< full_path << "." << std::endl;
		return full_path;
	}
	stool.remove_rep_char(full_path, '/');
	verbose(V) << "(location_path) Returns: " << \
		full_path << "." << std::endl;
	return full_path;
}
