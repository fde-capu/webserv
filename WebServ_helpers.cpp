/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_helpers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 15:25:13 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/26 22:08:53 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
extern DataFold g_config;

int ws_reply_instance::bad_gateway(std::string u_filename)
{
	set_code(502, "Bad Gateway");
	template_page(502, u_filename);
	return 502;
}

int ws_reply_instance::list_autoindex(std::string dir, ws_server_instance& si)
{
	DIR *dp;
	struct dirent *dirp;
	std::string list;
	std::string href;
	std::string fname;

	dp = opendir(dir.c_str());
	while ((dirp = readdir(dp)) != NULL)
	{
		fname = dirp->d_name;
		if (fname == "." || fname == ".." || !StringTools::startsWith(fname, "."))
		{
			href = si.in_header.directory + "/" + fname;
			remove_dup_char(href, '/');
			list += "<a href='" + href + "'>" + fname + "</a><br>\n";
		}
	}
	closedir(dp);
	set_code(200, "OK");
	out_body = "<html><head><title>" + \
		g_config.getValStr("server_name") + \
		"</title><body><h1>" + g_config.getValStr("server_name") + \
		"</h1>" + list + "</body></html>";
	return 200;
}

struct pollfd WebServ::stdin_to_pollfd()
{
	struct pollfd ufds;

	ufds = pollfd();
	ufds.fd = 0;
	ufds.events = POLLIN;
	return ufds;
}

struct pollfd WebServ::make_in_out_fd(int newfd)
{
	struct pollfd ufds;

	ufds = pollfd();
	ufds.fd = newfd;
	ufds.events = POLLIN | POLLOUT;
	return ufds;
}


void WebServ::exit_gracefully()
{
	flush_stdin();
	verbose(3) << "(webserv) Exit gracefully." << std::endl;
	verbose(2) << config.getValStr("bye_message") << std::endl;
	lit = false;
	return ;
}

bool WebServ::is_a_webserv(int fd) const
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
{
	int V(5);
	verbose(V) << "(webserv) Destructor." << std::endl;
	free(static_cast<void*>(buffer));
}

void WebServ::flush_stdin()
{
	std::string line;
	std::getline(std::cin, line);
}

const DataFold ws_server_instance::operator[] (std::string df_query) const
{ return this->config.get_val(df_query); }

std::string ws_server_instance::val(std::string key) const
{ return this->config.get<DataFold>(key)[key]; }

bool ws_reply_instance::save_canceled() const
{
	return false \
		|| out_header.status == 200 \
		|| out_header.status == 301 \
		|| out_header.status == 400 \
		|| out_header.status == 403 \
		|| out_header.status == 404 \
		|| out_header.status == 424 \
		|| out_header.status == 405 \
		|| out_header.status == 413 \
		|| out_header.status == 421 \
		|| out_header.status == 422 \
		|| out_header.status == 424 \
		|| out_header.status == 501 \
		|| out_header.status == 507 \
		;
}

void ws_reply_instance::set_code(int code_n, const std::string& u_output)
{
	out_header.status = code_n;
	out_header.status_msg = u_output;
	verbose(3) << "(webserv) " << code_n << " " << u_output << std::endl;
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

void ws_reply_instance::encapsulate()
{
	int V(3);
	std::string out = "";

	verbose(V) << "(encapsulate) out_header:" << std::endl << out_header << std::endl;
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
	if (out_header.cookie != "")
		out += "Set-Cookie: " + out_header.cookie + "\r\n";
	out += "\r\n";
	verbose(V) << "(encapsulate) pre header:" << std::endl << out << std::endl;
	verbose(V) << "(encapsulate) pre body:" << LONG(out_body) << std::endl;
	out_body = out + out_body;
	package_length = out_body.length();
	verbose(V) << "(encapsulate) package_length " << package_length << std::endl;
	verbose(V) << "(encapsulate) post body:" << std::endl << LONG(out_body) << std::endl;
}

bool ws_header::is_get()
{ return is_equal_insensitive(method, "GET"); }

bool ws_header::is_post()
{ return is_equal_insensitive(method, "POST"); }

void ws_header::header500()
{
	method = "";
	directory = "";
	protocol = "HTTP/1.1";
	status = 500;
	status_msg = "Internal Server Error";
	connection = "close";
}

ws_reply_instance& ws_reply_instance::operator= (ws_reply_instance const & rhs)
{
	int V(5);
	verbose(V) << "(ws_reply_instance) operator=" << std::endl;
	if (this != &rhs)
	{
		buffer = rhs.buffer;
		out_header = rhs.out_header;
		out_body = rhs.out_body;
		package_length = rhs.package_length;
		file_name = rhs.file_name;
		chronometer = rhs.chronometer;
		first_time = rhs.first_time;
		full_path = rhs.full_path;
		file_page = rhs.file_page;
		file_save = rhs.file_save;
		pipe_pc[0] = rhs.pipe_pc[0];
		pipe_pc[1] = rhs.pipe_pc[1];
		pipe_cp[0] = rhs.pipe_cp[0];
		pipe_cp[1] = rhs.pipe_cp[1];
		child_pid = rhs.child_pid;
		dumping_to_cgi = rhs.dumping_to_cgi;
		getting_from_cgi = rhs.getting_from_cgi;
		to_work_load = rhs.to_work_load;
		poll_list = rhs.poll_list;
	}
	return *this;
}

void ws_reply_instance::init()
{
	int V(5);
	verbose(V) << "(ws_reply_instance) Initialization." << std::endl;
	buffer = 0;
	out_header.header500();
	out_body = "";
	package_length = 0;
	file_name = "";
	chronometer.btn_reset();
	first_time = true;
	full_path = "";
	pipe_pc[0] = 0;
	pipe_pc[1] = 0;
	pipe_cp[0] = 0;
	pipe_cp[1] = 0;
	child_pid = 0;
	dumping_to_cgi = false;
	getting_from_cgi = false;
	to_work_load = false;
}

ws_reply_instance::ws_reply_instance()
{
	int V(5);
	verbose(V) << "(ws_reply_instance) Empty constructor." << std::endl;
	init();
}

ws_reply_instance::~ws_reply_instance()
{
	int V(5);
	verbose(V) << "(ws_reply_instance) Destructor." << std::endl;
	if (buffer)
		free(static_cast<void*>(buffer));
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
	static int V(3);
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
		reached_limit = exceeded_limit || in_body.length() == static_cast<size_t>(in_header.content_length);

		verbose(V) << "(set_sizes:multipart) exceeded_limit: " << exceeded_limit << std::endl;
		verbose(V) << "(set_sizes:multipart) reached_limit: " << reached_limit << std::endl;
	}
	else if (is_chunked())
	{
		exceeded_limit = max_size && chunked_content.length() > max_size;
		reached_limit = max_size && chunked_content.length() >= max_size;
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
	static int V(5);
	DataFold locations(config.get<DataFold>("location"));
	DataFold loc;
	DataFold out;

	out = std::string(key + ":" + u_default);
	verbose(V) << "(server_location_config) default: " << out << std::endl;
	if (root_config.getValStr(key) != "")
	{
		out = root_config.getValStr(key);
		verbose(V) << "(server_location_config) root_config.get(" << key \
			<< "): " << out << std::endl;
	}
	if (config.get(key) != "")
	{
		out = config.get(key);
		verbose(V) << "(server_location_config) config.get: " << out \
			<< std::endl;
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
					verbose(V) << "(server_location_config) location: " << \
						out << std::endl;
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
		full_path = sys_dir + "/" + html_dir + "/";
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

void ws_reply_instance::cgi_setenv(ws_server_instance& si, std::string path_info)
{
	int V(2);

	setenv("REQUEST_METHOD", si.in_header.method.c_str(), 1);
	setenv("SERVER_PROTOCOL", si.in_header.protocol.c_str(), 1);
	setenv("PATH_INFO", path_info.c_str(), 1);
	setenv("SESSION_ID", si.in_header.cookie.c_str(), 1);

	std::vector<std::string> cookie(split_trim(si.in_header.cookie, ";"));
	std::string key;
	std::string val;
	for (size_t i = 0; i < cookie.size(); i++)
	{
		if (cookie[i].empty())
			continue ;
		key = StringTools::get_before_first(cookie[i], "=");
		val = StringTools::get_after_first(cookie[i], "=");
		key = "WS_" + to_upper(key);
		verbose(V) << "(cgi_setenv) " << key << "=" << val << std::endl;
		setenv(key.c_str(), val.c_str(), 1);
	}
}

void ws_reply_instance::header_from_body()
{
	size_t dbreak[3];
	size_t h_body(std::string::npos);
	size_t dblen;

	out_header.status = atoi(StringTools::query_for("Status", out_body).c_str());
	out_header.status_msg = StringTools::query_for(itoa(out_header.status), out_body);
	out_header.content_type = StringTools::query_for("Content-Type", out_body);
	out_header.charset = StringTools::query_for("charset", out_body);
	out_header.cookie = StringTools::get_after_until_line(out_body, "Set-Cookie:");
	trim(out_header.cookie);
	dbreak[0] = out_body.find("\r\n\r\n");
	dbreak[1] = out_body.find("\n\n");
	dbreak[2] = out_body.find("\r\r");
	for (size_t i = 0; i < 3; i++)
		h_body = h_body > dbreak[i] ? dbreak[i] : h_body;
	dblen = h_body == dbreak[0] ? 4 : 2;
	if (h_body != std::string::npos)
		out_body.erase(0, h_body + dblen);
}

bool ws_server_instance::is_cgi() const
{ return cgi_flag; }

void ws_reply_instance::init_buffer()
{
	int V(5);

	if (first_time)
	{
		verbose(V) << "(ws_reply_instance) Initialized buffer." << std::endl;
		first_time = false;
		buffer = static_cast<char*>(malloc(ASYNC_CHUNK_SIZE));
		chronometer.btn_reset();
	}
}

void ws_reply_instance::template_page(size_t error_code, std::string u_filename)
{
	int V(3);

	file_name = u_filename != "" ? u_filename : TemplatePage::for_code(error_code);
	verbose(V) << "(template_page) file_name " << file_name << std::endl;
	if (!FileString::is_file(file_name) || !FileString::exists(file_name))
	{
		out_body = "Response code: " + itoa(error_code) + "\n";
		return ;
	}
	file_page = open(file_name.c_str(), O_CLOEXEC | O_NONBLOCK | O_RDONLY);
	if (file_page == -1)
		throw std::domain_error("(template_page) Cannot open file to load page.");
	if (fcntl(file_page, F_SETFL, O_NONBLOCK) == -1)
		throw std::domain_error("(template_page) Could not set non-blocking file.");
	poll_list.push_back(WebServ::make_in_out_fd(file_page));
	to_work_load = true;
}

std::string ws_reply_instance::custom_error(const size_t code, ws_server_instance& si) const
{
	size_t V(3);
	std::string out;
	DataFold loop;
	DataFold err;
	DataFold loc;
	std::string code_str(itoa(code));
	bool match(false);

	verbose(V) << "(custom_error) for " << code << std::endl;

	loop = si.config;
	while (loop.loop())
	{
		if (loop.key == "error_page")
		{
			verbose(V) << "(custom_error) config: " << loop.val << std::endl;
			err = ":" + loop.val;
			while (err.loop())
			{
				verbose(V) << "(custom_error) err: " << err.val << std::endl;
				if (err.val == code_str)
				{
					verbose(V) << "(custom_error) MATCH" << std::endl;
					match = true;
				}
				if (match)
					out = err.val;
			}
		}
		match = false;
	}
	loop = si.config.get<DataFold>("location");
	while (loop.loop())
	{
		loc = loop.val;
		while (loc.loop())
		{
			if (!StringTools::startsWith(si.in_header.directory, loc.getValStr("uri")))
				continue ;
			if (loc.key == "error_page")
			{
				verbose(V) << "(custom_error) locations: " << loc.val << std::endl;
				err = ":" + loc.val;
				while (err.loop())
				{
					verbose(V) << "(custom_error) err: " << err.val << std::endl;
					if (err.val == code_str)
					{
						verbose(V) << "(custom_error) MATCH" << std::endl;
						match = true;
					}
				}
				if (match)
					out = err.val;
			}
		}
		match = false;
	}
	if (out == "")
		return "";
	out = si.root_config.getValStr("root") + "/" + si.location_get_single("root") + "/" + out;
	if (!FileString::exists(out))
		return "";
	verbose(V) << "(custom_error) Set file_name: " << out << std::endl;
	return out;
}
