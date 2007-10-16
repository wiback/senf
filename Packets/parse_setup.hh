// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief parse_setup public header */

#ifdef SENF_PARSER_SETUP_INCLUDED
#  undef SENF_PARSE_FIELD
#  undef SENF_PARSE_FIELD_RO
#  undef SENF_PARSE_FIELD_AFTER
#  undef SENF_PARSE_FIELD_AFTER_RO
#  undef SENF_PARSE_CUSTOM_FIELD
#  undef SENF_PARSE_CUSTOM_FIELD_AFTER
#  undef SENF_PARSE_BITFIELD
#  undef SENF_PARSE_BITFIELD_RO
#  undef SENF_PARSER_INHERIT
#  undef SENF_PARSER_FINALIZE
#  undef SENF_PARSER_FINALIZE_INHERITED
#  undef SENF_PARSER_SETUP_INCLUDED
#endif

#define SENF_PARSE_FIELD              SENF_PARSER_FIELD
#define SENF_PARSE_FIELD_RO           SENF_PARSER_FIELD_RO
#define SENF_PARSE_FIELD_AFTER        SENF_PARSER_FIELD_AFTER
#define SENF_PARSE_FIELD_AFTER_RO     SENF_PARSER_FIELD_AFTER_RO
#define SENF_PARSE_CUSTOM_FIELD       SENF_PARSER_CUSTOM_FIELD
#define SENF_PARSE_CUSTOM_FIELD_AFTER SENF_PARSER_CUSTOM_FIELD_AFTER
#define SENF_PARSE_BITFIELD           SENF_PARSER_BITFIELD
#define SENF_PARSE_BITFIELD_RO        SENF_PARSER_BITFIELD_RO

#define SENF_PARSER_INHERIT           SENF_PARSER_INHERIT_I
#define SENF_PARSER_FINALIZE          SENF_PARSER_FINALIZE_DYNAMIC
#define SENF_PARSER_FINALIZE_INHERITED SENF_PARSER_FINALIZE_INHERITED_DYNAMIC

#define SENF_PARSER_SETUP_INCLUDED 1

SENF_PARSER_INITIALIZE()


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
