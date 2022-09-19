/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TemplateError.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 20:43:45 by fde-capu          #+#    #+#             */
/*   Updated: 2022/09/19 17:32:16 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TemplateError.hpp"

std::string TemplateError::path("");
FileString TemplateError::config;

TemplateError::TemplateError(const char * executable, const char * u_defaults)
{
	path = StringTools::remove_filename_from_path(executable);
	std::cout << "path: " << path << std::endl;

	file_name = u_defaults;
	std::cout << "file_name: " << file_name << std::endl;

	full_file_name = path + "/" + file_name;
	std::cout << "full_file_name: " << full_file_name << std::endl;

	config.load(full_file_name.c_str());
	std::cout << "config: " << config << std::endl;
}

std::string TemplateError::page(size_t error_code)
{
	std::string error_query = StringTools::stoa(error_code);
	std::cout << "error_query: " << error_query << std::endl;

	DataFold data(config.getDataFold());
	std::cout << "data: " << data << std::endl;

	std::string t_file = data.getValStr(error_query);
	std::cout << "t_tile: " << t_file << std::endl;

	std::string full_t_file = path + "/" + t_file;
	std::cout << "full_t_file: " << full_t_file << std::endl;

	FileString result;
	result.load(full_t_file.c_str());

	std::cout << "result: " << result << std::endl;
	return result.getContent();
}

TemplateError::operator std::string()
{
	return "THIS FINAL RESULT";
}

TemplateError::~TemplateError()
{
	return ;
}
