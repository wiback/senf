// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief config public header */

#ifndef HH_config_
#define HH_config_ 1

// Custom includes
#include <boost/cstdint.hpp>
#include <limits.h>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace config {

    typedef boost::int_fast64_t time_type;

}}

# include "autoconf.hh"
# include "local_config.hh"
#
# ifndef SENF_ABSOLUTE_INCLUDE_PATH
#     define SENF_ABSOLUTE_INCLUDE_PATH(senf_relative_include_file_path) \
          <senf/senf_relative_include_file_path>
# endif
#
# ifndef SENF_copy_n
#     ifdef HAVE_GNUCXX_COPYN
#         include <ext/algorithm>
          namespace senf { namespace config { using __gnu_cxx::copy_n; }}
#     endif
#     ifdef HAVE_STD_COPYN
#         include <algorithm>
          namespace senf { namespace config { using std::copy_n; }}
#     endif
#  endif
#
# ifndef SENF_MPL_RV_ALIGNMENT
#     define SENF_MPL_RV_ALIGNMENT 16
# endif
#
# if    !defined(SENF_BUFFER_USE_LOCALS) \
     && !defined(SENF_BUFFER_USE_ALLOCA) \
     && !defined(SENF_BUFFER_USE_NEW)
#     define SENF_BUFFER_USE_NEW 1
# endif
#
# ifndef SENF_SENFLOG_LIMIT
#     ifndef SENF_DEBUG
#         define SENF_SENFLOG_LIMIT senf::log::IMPORTANT
#     else
#         define SENF_SENFLOG_LIMIT senf::log::NOTICE
#     endif
# endif
#
# ifndef SENF_MULTI_CONNECTOR_MAX_ARGS
#     define SENF_MULTI_CONNECTOR_MAX_ARGS 3
# endif
#
# ifndef SENF_DEBUG_BACKTRACE_NUMCALLERS
#     define SENF_DEBUG_BACKTRACE_NUMCALLERS 64
# endif
#
# ifndef SENF_CONSOLE_MAX_COMMAND_ARITY
#     define SENF_CONSOLE_MAX_COMMAND_ARITY 6
# endif
#
# ifndef PHOENIX_LIMIT
#     define PHOENIX_LIMIT SENF_CONSOLE_MAX_COMMAND_ARITY
# endif
#
# ifndef PHOENIX_LIMIT
#     define PHOENIX_LIMIT 6
# endif
#
# ifndef SENF_PACKET_DUMP_COLON_COLUMN
#     define SENF_PACKET_DUMP_COLON_COLUMN 27
# endif
#
# ifndef SENF_PACKET_ANNOTATION_SLOTS
#     define SENF_PACKET_ANNOTATION_SLOTS 8
# endif
#
# ifndef SENF_PACKET_ANNOTATION_SLOTSIZE
#     define SENF_PACKET_ANNOTATION_SLOTSIZE 16
# endif
#
# ifndef SENF_PACKET_PREALLOC_INTERPRETERS
#     define SENF_PACKET_PREALLOC_INTERPRETERS 8
# endif
#
# ifndef SENF_PACKET_VECTOR_SIZE_INDEX
#     define SENF_PACKET_VECTOR_SIZE_INDEX 11
# endif
#
# ifndef SENF_PACKET_VECTOR_HEADROOM
#     define SENF_PACKET_VECTOR_HEADROOM 128
# endif
#
# ifndef SENF_PACKET_VECTOR_TAILROOM
#     define SENF_PACKET_VECTOR_TAILROOM 128
# endif
#
# ifdef SENF_DEBUG
#     ifdef HAVE_EXECINFO_H
#          define SENF_BACKTRACE
#     endif
# endif
#
//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u all_tests"
// End:
