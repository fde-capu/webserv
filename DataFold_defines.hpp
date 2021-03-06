/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DataFold_defines.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 12:02:11 by fde-capu          #+#    #+#             */
/*   Updated: 2022/07/13 19:52:15 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DATAFOLD_DEFINES_HPP
# define DATAFOLD_DEFINES_HPP

# define DF_TYPE_NUMBER		2
# define DF_TYPE_STRING		4
# define DF_TYPE_ARRAY		8
# define DF_TYPE_SUB		16
# define DF_TYPE_ONLY_VAL	32

# define DF_NUM_QUOTE ""
# define DF_VAL_QUOTE "\'"
# define DF_ARRAY_INIT "[ "
# define DF_COMMA " , "
# define DF_DOTCOMMA " ; "
# define DF_ARRAY_END " ]"
# define DF_KVDIV " : "
# define DF_OBJ_INIT "{ "
# define DF_OBJ_END " }"

# define SERVER_TAG "(DataFold controled error)"
# define DFK SERVER_TAG " '" + key + "'" +
# define DF_ERR_NO_KEY DFK " not found."
# define DF_ERR_IS_ARRAY DFK " is array."
# define DF_ERR_NOT_NUMBER DFK " is not a number."
# define DF_ERR_IS_OBJECT DFK " is object."
# define DF_ERR_NO_FILE SERVER_TAG " No file to read."
# define DF_ERR_NOT_SINGLE_KEY " Not single key."
# define DF_ERR_CANT_CONVERT_TO_INT " Cannot convert to int."
# define DF_ERR_NOT_STRING DFK " is not a string."
# define DF_ERR_NOT_ARRAY DFK " is not array."

#endif
