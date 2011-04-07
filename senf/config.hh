// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

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
# ifndef SENF_DEBUG_BACKTRACE_NUMCALLERS
#     define SENF_DEBUG_BACKTRACE_NUMCALLERS 64
# endif
#
# ifndef SENF_CONSOLE_MAX_COMMAND_ARITY
#     define SENF_CONSOLE_MAX_COMMAND_ARITY 6
# endif
#
# ifndef PHOENIX_LIMIT
#     define PHOENIX_LIMIT 6
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
# ifdef SENF_DEBUG
#     ifdef HAVE_EXECINFO_H
#          define SENF_BACKTRACE
#     endif
# endif
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
