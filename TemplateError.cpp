/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TemplateError.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 20:43:45 by fde-capu          #+#    #+#             */
/*   Updated: 2022/09/28 23:42:53 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TemplateError.hpp"

std::string TemplateError::path("");
FileString TemplateError::config;

TemplateError::TemplateError(const char * executable, const char * u_defaults)
{
	int V(8);

	path = StringTools::remove_filename_from_path(executable);
	verbose(V) << "(TemplateError) path: " << path << std::endl;

	file_name = u_defaults;
	verbose(V) << "(TemplateError) file_name: " << file_name << std::endl;

	full_file_name = path + "/" + file_name;
	verbose(V) << "(TemplateError) full_file_name: " << full_file_name << std::endl;

	config.load(full_file_name.c_str());
	verbose(V) << "(TemplateError) config: " << config << std::endl;
}

std::string TemplateError::page(size_t error_code, std::string u_content)
{
	int V(8);

	if (u_content != "")
		return u_content;

	std::string error_query = StringTools::stoa(error_code);
	verbose(V) << "(TemplateError) error_query: " << error_query << std::endl;

	DataFold data(config.getDataFold());
	verbose(V) << "(TemplateError) data: " << data << std::endl;

	std::string t_file = data.getValStr(error_query);
	verbose(V) << "(TemplateError) t_tile: " << t_file << std::endl;

	std::string full_t_file = path + "/" + t_file;
	verbose(V) << "(TemplateError) full_t_file: " << full_t_file << std::endl;

	FileString result;
	result.load(full_t_file.c_str());
	verbose(V) << "(TemplateError) fail: " << result.fail() << std::endl;

	if (result.fail())
		return "Error " + itoa(error_code) + "\n";

	verbose(V) << "(TemplateError) result: " << result << std::endl;
	return result.getContent();
}
