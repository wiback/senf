// $Id$
//
// Copyright (C) 2012
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
    \brief local_config_sample public header */

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// Debug build
// #define SENF_DEBUG 1

///////////////////////////////////////////////////////////////////////////
// Maximum number of callers to show when backtraces are enabled
// #define SENF_DEBUG_BACKTRACE_NUMCALLERS 64

///////////////////////////////////////////////////////////////////////////
// If defined, the PPI tracing function is disabled to increase performance
// #define SENF_PPI_NOTRACE 1

///////////////////////////////////////////////////////////////////////////
// If defined, senf will not use the console itself
// #define SENF_DISABLE_CONSOLE 1

///////////////////////////////////////////////////////////////////////////
// Maximum number of automatically parsed arguments of any console command
// #define SENF_CONSOLE_MAX_COMMAND_ARITY 6

///////////////////////////////////////////////////////////////////////////
// Defines the compile-time logging configuration. See documentation.
// #define SENF_LOG_CONF (( (senf)(log)(Debug), (_), VERBOSE ))

///////////////////////////////////////////////////////////////////////////
// Default log limit for the senf::SenfLog stream used by senf itself
// #define SENF_SENFLOG_LIMIT senf::log::IMPORTANT

///////////////////////////////////////////////////////////////////////////
// Maximum number of additional arguments which may be passed to a
// multi-connector using additional trailing senf::ppi::connect arguments
// #define SENF_MULTI_CONNECTOR_MAX_ARGS 3

///////////////////////////////////////////////////////////////////////////
// The ppi packet dump will align colons at this column
// #define SENF_PACKET_DUMP_COLON_COLUMN 27

///////////////////////////////////////////////////////////////////////////
// Maximum number of preallocated simple annotations
// #define SENF_PACKET_ANNOTATION_SLOTS 8

///////////////////////////////////////////////////////////////////////////
// Maximum size of a preallocated simple annotation
// #define SENF_PACKET_ANNOTATION_SLOTSIZE 16

///////////////////////////////////////////////////////////////////////////
// Maximum number of preallocated packet interpreters
// #define SENF_PACKET_PREALLOC_INTERPRETERS 8

///////////////////////////////////////////////////////////////////////////
// If defined, complex annotations are disabled to increase performance.
// #define SENF_PACKET_NO_COMPLEX_ANNOTATIONS 1

///////////////////////////////////////////////////////////////////////////
// If defined, non preallocated packet interpreters are disabled. A packet
// may not have more than SENF_PACKET_PREALLOC_INTERPRETERS interpreters
// #define SENF_PACKET_NO_HEAP_INTERPRETERS 1

///////////////////////////////////////////////////////////////////////////
// If defined, use an alternative preallocation strategy. This together with
// SENF_PACKET_NO_HEAP_INTERPRETERS generates very fast allocation and
// deallocation at the cost of slightly higher PacketImpl construction.
// #define SENF_PACKET_ALTERNATIVE_PREALLOC

///////////////////////////////////////////////////////////////////////////
// The base-2 logarithm of the initial packet allocation.
// #define SENF_PACKET_VECTOR_SIZE_INDEX 10

////////////////////////////////////////////////////////////////
// The initial headroom of PacketVector
// #define SENF_PACKET_VECTOR_HEADROOM 64

///////////////////////////////////////////////////////////////////////////
// The initial tailroom of PacketVector
// #define SENF_PACKET_VECTOR_TAILROOM 64


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
