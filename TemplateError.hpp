/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TemplateError.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 20:43:31 by fde-capu          #+#    #+#             */
/*   Updated: 2022/09/21 21:26:22 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEMPLATEERROR_HPP
# define TEMPLATEERROR_HPP

# include <string>
# include "FileString.hpp"

class TemplateError
{
	private:
		std::string error_to_get;
		std::string file_name;
		static std::string path;
		std::string full_file_name;
		static FileString config;
		TemplateError();
	public:
		TemplateError(const char *, const char *);
		static std::string page(size_t, std::string = "");
};

#endif