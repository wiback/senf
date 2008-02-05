# // $Id$
# //
# // Copyright (C) 2007
# // Fraunhofer Institute for Open Communication Systems (FOKUS)
# // Competence Center NETwork research (NET), St. Augustin, GERMANY
# //     Stefan Bund <g0dil@berlios.de>
# //
# // This program is free software; you can redistribute it and/or modify
# // it under the terms of the GNU General Public License as published by
# // the Free Software Foundation; either version 2 of the License, or
# // (at your option) any later version.
# //
# // This program is distributed in the hope that it will be useful,
# // but WITHOUT ANY WARRANTY; without even the implied warranty of
# // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# // GNU General Public License for more details.
# //
# // You should have received a copy of the GNU General Public License
# // along with this program; if not, write to the
# // Free Software Foundation, Inc.,
# // 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 
# /** \file
#     \brief config public header */
# 
# ifndef HH_config_
# define HH_config_ 1
# 
# // Custom includes
# 
# ///////////////////////////////hh.p////////////////////////////////////////
# 
# include "local_config.hh"
#
# ifndef SENF_ABSOLUTE_INCLUDE_PATH
#     define SENF_ABSOLUTE_INCLUDE_PATH(senf_relative_include_file_path) <senf/senf_relative_include_file_path>
# endif
#
# ifndef SENF_copy_n
#     include <algorithm>
#     if defined(__GNUC__) && ! defined(_STLP_ALGORITHM) && (__GNUC__>=4 || (__GNUC__==3 && __GNUC_MINOR__>=4))
#         include <ext/algorithm>
#         define SENF_copy_n __gnu_cxx::copy_n
#     else
#         define SENF_copy_n std::copy_n
#     endif
# endif
#
# ifndef SENF_MPL_RV_ALIGNMENT
#     define SENF_MPL_RV_ALIGNMENT 16
# endif
#
# if !defined(SENF_BUFFER_USE_LOCALS) && !defined(SENF_BUFFER_USE_ALLOCA) && !defined(SENF_BUFFER_USE_NEW)
#
#     if defined(__GNUC__)
#         define SENF_BUFFER_USE_LOCALS 1
#
#     // Add other compilers here ...
#
#     // dynamic arrays are part of C99. Which is NOT part of C++ 
#     // but lets try nonetheless ...
#     elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#         define SENF_BUFFER_USE_LOCALS 1
#     endif
#
#     if !defined(SENF_BUFFER_USE_LOCALS) && !defined(SENF_BUFFER_USE_ALLOCA)
#         define SENF_BUFFER_USE_NEW 1
#     endif
#
# endif
#
# ifndef SENF_SENFLOG_LIMIT
#     ifdef SENF_NO_DEBUG
#         define SENF_SENFLOG_LIMIT senf::log::IMPORTANT
#     else
#         define SENF_SENFLOG_LIMIT senf::log::VERBOSE
#     endif
# endif
# 
# ///////////////////////////////hh.e////////////////////////////////////////
# endif
 

// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u all_tests"
// End:
 
