/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TemplatePage.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/15 23:04:47 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/27 14:14:00 by fde-capu         ###   ########.fr       */
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
	int V(4);

	std::string query = StringTools::stoa(error_code);
	verbose(V) << "(TemplatePage) query: " << query << std::endl;

	DataFold data(config.getDataFold());
	verbose(V + 1) << "(TemplatePage) data: " << data << std::endl;

	std::string t_file = data.getValStr(query);
	verbose(V + 1) << "(TemplatePage) t_tile: " << t_file << std::endl;

	std::string full_t_file = path + "/" + t_file;
	verbose(V + 1) << "(TemplatePage) full_t_file: " << full_t_file << std::endl;

	stool.remove_dup_char(full_t_file, '/');
	return full_t_file;
}
