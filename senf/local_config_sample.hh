// $Id$
//
// Copyright (C) 2012
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
// #define SENF_PACKET_VECTOR_SIZE_INDEX 11

////////////////////////////////////////////////////////////////
// The initial headroom of PacketVector
// #define SENF_PACKET_VECTOR_HEADROOM 128

///////////////////////////////////////////////////////////////////////////
// The initial tailroom of PacketVector
// #define SENF_PACKET_VECTOR_TAILROOM 128


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
