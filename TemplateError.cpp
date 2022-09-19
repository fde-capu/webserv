/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TemplateError.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 20:43:45 by fde-capu          #+#    #+#             */
/*   Updated: 2022/09/19 22:07:21 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TemplateError.hpp"

std::string TemplateError::path("");
FileString TemplateError::config;

TemplateError::TemplateError(const char * executable, const char * u_defaults)
{
	path = StringTools::remove_filename_from_path(executable);
	verbose(8) << "path: " << path << std::endl;

	file_name = u_defaults;
	verbose(8) << "file_name: " << file_name << std::endl;

	full_file_name = path + "/" + file_name;
	verbose(8) << "full_file_name: " << full_file_name << std::endl;

	config.load(full_file_name.c_str());
	verbose(8) << "config: " << config << std::endl;
}

std::string TemplateError::page(size_t error_code)
{
	std::string error_query = StringTools::stoa(error_code);
	verbose(8) << "error_query: " << error_query << std::endl;

	DataFold data(config.getDataFold());
	verbose(8) << "data: " << data << std::endl;

	std::string t_file = data.getValStr(error_query);
	verbose(8) << "t_tile: " << t_file << std::endl;

	std::string full_t_file = path + "/" + t_file;
	verbose(8) << "full_t_file: " << full_t_file << std::endl;

	FileString result;
	result.load(full_t_file.c_str());

	verbose(8) << "result: " << result << std::endl;
	return result.getContent();
}
