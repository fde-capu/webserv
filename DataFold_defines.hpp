/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DataFold_defines.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 12:02:11 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/16 15:34:19 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DATAFOLD_DEFINES_HPP
# define DATAFOLD_DEFINES_HPP

# define DF_TYPE_NUMBER 2
# define DF_TYPE_STRING	4
# define DF_TYPE_ARRAY	8
# define DF_TYPE_SUB	16
# define DF_QUOTE_SET "\"\'`"
# define DF_KEY_QUOTE "\'"
# define DF_NUM_QUOTE ""
# define DF_VAL_QUOTE "\'"
# define DF_ARRAY_INIT "[ "
# define DF_COMMA " , "
# define DF_ARRAY_END " ]"
# define DF_KVDIV " : "
# define DF_OBJ_INIT "{ "
# define DF_OBJ_END " }"
# define DF_ENDVAL_SET ";"

# define SERVER_TAG "(DataFold controled error)"
# define DFK SERVER_TAG " '" + key + "'" +
# define DF_ERR_NO_KEY DFK " not found."
# define DF_ERR_IS_ARRAY DFK " is array."
# define DF_ERR_NOT_NUMBER DFK " is not a number."
# define DF_ERR_IS_OBJECT DFK " is object."
# define DF_ERR_NO_FILE SERVER_TAG " No file to read."

#endif
