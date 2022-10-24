/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TemplatePage.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/15 23:04:55 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/24 22:31:25 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEMPLATEERROR_HPP
# define TEMPLATEERROR_HPP

# include <string>
# include "FileString.hpp"

class TemplatePage
{
	private:
		std::string error_to_get;
		std::string file_name;
		static std::string path;
		std::string full_file_name;
		static FileString config;
		TemplatePage();
	public:
		TemplatePage(const char *, const char *);
		static std::string page(size_t, std::string = "");
		static std::string for_code(size_t);
};

#endif
