/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TemplatePage.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/15 23:04:47 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/25 17:45:38 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TemplatePage.hpp"

std::string TemplatePage::path("");
FileString TemplatePage::config;

TemplatePage::TemplatePage(const char * executable, const char * u_defaults)
{
	int V(8);

	path = StringTools::remove_filename_from_path(executable);
	verbose(V) << "(TemplatePage) path: " << path << std::endl;

	file_name = u_defaults;
	verbose(V) << "(TemplatePage) file_name: " << file_name << std::endl;

	full_file_name = path + "/" + file_name;
	verbose(V) << "(TemplatePage) full_file_name: " << full_file_name << std::endl;

	config.load(full_file_name.c_str());
	verbose(V) << "(TemplatePage) config: " << config << std::endl;
}

std::string TemplatePage::for_code(size_t error_code)
{
	int V(1);

	std::string error_query = StringTools::stoa(error_code);
	verbose(V) << "(TemplatePage) error_query: " << error_query << std::endl;

	DataFold data(config.getDataFold());
	verbose(V) << "(TemplatePage) data: " << data << std::endl;

	std::string t_file = data.getValStr(error_query);
	verbose(V) << "(TemplatePage) t_tile: " << t_file << std::endl;

	std::string full_t_file = path + "/" + t_file;
	verbose(V) << "(TemplatePage) full_t_file: " << full_t_file << std::endl;

	stool.remove_dup_char(full_t_file, '/');
	return full_t_file;
}
