/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileString.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 09:30:53 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/03 23:12:47 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileString.hpp"

FileString::operator DataFold() const
{ return getDataFold(); }

FileString::FileString()
: _read_ok(false), _processed_ok(false),
  key(""), val(""), type(0)
{}

void FileString::load()
{ load(getFileName()); }

void FileString::load(const std::string & str)
{
	_content = str;
	parse();
}

void FileString::load(const char * u_fn)
{
	static int V(1);
	std::fstream file_read;
	std::string line;

	file_read.open(u_fn, std::ios::in);
	if (!file_read)
	{
		_read_ok = false;
		file_read.close();
		verbose(V) << "(FileString) Failed to load or empty string for file " << u_fn << "." << std::endl;
		return ;
	}
	_read_ok = true;
	while (std::getline(file_read, line))
		_content += line + "\n";
	verbose(V) << "(FileString::load) _content pre-parse: " << _content \
		<< std::endl;
	parse();
}

FileString::FileString(const char * u_file_name)
: _content(""), _processed(""),
  _read_ok(false), _processed_ok(false),
  key(""), val(""), type(0)
{
	_file_name = const_cast<char *>(u_file_name);
	load();
}

std::string const FileString::operator[](std::string key) const
{ return fs_data[key]; }

void FileString::parse()
{
	static int V(1);

	_processed = _content;
	remove_comments(_processed);
	substitute_super(_processed, "\n", ";");
	substitute_super(_processed, "\r", ";");
	substitute_super(_processed, "\t", " ");
//	substitute_super(_processed, "; {", " {");
//	substitute_super(_processed, "{; ", "{ ");
//	substitute_super(_processed, "; ;", "; ");
//	substitute_super(_processed, ";;", ";");
	soft_trim(_processed);
	hard_trim(_processed);
	_processed_ok = true;

	verbose(V) << "(FileString::parse) _processed: " << _processed \
		<< std::endl;

	this->fs_data = DataFold(_processed);

	verbose(V) << "(FileString::parse) this->fs_data: " << this->fs_data \
		<< std::endl;
}

void FileString::parse(std::string str)
{
	_content = str;
	parse();
}

FileString::FileString(FileString & src)
: key(""), val(""), type(0)
{
	*this = src;
	return ;
}

FileString& FileString::operator= (FileString & rhs)
{
	if (this != &rhs)
	{
		this->_file_name = rhs.getFileName();
		this->_content = rhs.getContent();
		this->_read_ok = rhs.success();
		this->_processed_ok = rhs.isProcessed();
		this->_processed = rhs.getProcessed();
	}
	return *this;
}

FileString& FileString::operator= (const std::string& rhs)
{ 
	this->_file_name = const_cast<char*>(std::string("").c_str());
	this->_content = rhs;
	this->_read_ok = true;
	parse(rhs); // Puts _processed and _processed_ok.
	return *this;
}

bool FileString::isProcessed() const
{ return _processed_ok; }

char * FileString::getFileName() const
{ return _file_name; }

std::ostream & operator<< (std::ostream & o, FileString & self)
{
 	o << self.getContent() << std::endl;
	return o;
}

FileString::~FileString()
{ return ; }

DataFold FileString::get_val(std::string key) const
{ return fs_data.get_val(key); }

DataFold FileString::get_val(std::string key, std::string sub) const
{ return fs_data.get_val(key, sub); }

DataFold FileString::getDataFold() const
{ return fs_data; }

std::string FileString::getProcessed() const
{ return _processed_ok ? _processed : 0; }

std::string FileString::content() const
{ return getContent(); }

std::string FileString::getContent() const
{ return _content; }

bool FileString::fail() const
{ return !_read_ok; }

bool FileString::success() const
{ return _read_ok; }

bool FileString::loop()
{
	if (fs_data.loop())
	{
		key = fs_data.key;
		val = fs_data.val;
		type = fs_data.type;
		return true;
	}
	return false;
}

int FileString::key_count(std::string key)
{ return fs_data.key_count(key); }

void FileString::write(std::string filename, std::string& data)
{
	std::ofstream out(filename.c_str());
	out << data;
	out.close();
}

bool FileString::is_dir(const std::string& pathname)
{
	struct stat s;
	return stat(pathname.c_str(), &s) == 0 && s.st_mode & S_IFDIR;
}

bool FileString::is_file(const std::string& filename)
{
	struct stat s;
	return stat(filename.c_str(), &s) == 0 && !(s.st_mode & S_IFDIR);
}

bool FileString::exists()
{ return exists(_file_name); }

bool FileString::exists(const std::string& pathorfile)
{
	struct stat s;
	return stat(pathorfile.c_str(), &s) == 0;
}
