// $Id$
//
// Copyright (C) 2008 
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
    \brief AuxParser public header */

#ifndef HH_AuxParser_
#define HH_AuxParser_ 1

#ifndef HH_Packets_
#error "Don't include 'AuxParser.hh' directly, include 'Packets.hh'"
#endif

// Custom includes
#include "PacketParser.hh"
#include "SafeIterator.hh"

//#include "AuxParser.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace detail {

    template <class P>
    struct PrefixAuxParserPolicy
    {
        typedef PrefixAuxParserPolicy WrapperPolicy;
        typedef P ParserType;

        static PacketParserBase::size_type const aux_bytes = ParserType::fixed_bytes;
        
        ParserType aux(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
        PacketParserBase::data_iterator adjust(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
    };

    template <class P, unsigned Dist>
    struct FixedAuxParserPolicy
    {
        typedef FixedAuxParserPolicy WrapperPolicy;
        typedef P ParserType;

        static PacketParserBase::size_type const aux_bytes = 0;
        
        ParserType aux(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
        PacketParserBase::data_iterator adjust(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
    };

    template <class P> struct DynamicWrapperAuxParserPolicy;
    
    template <class P>
    struct DynamicAuxParserPolicy
    {
        typedef DynamicWrapperAuxParserPolicy<P> WrapperPolicy;
        typedef P ParserType;

        static PacketParserBase::size_type const aux_bytes = 0;

        DynamicAuxParserPolicy(ParserType p);
        DynamicAuxParserPolicy(WrapperPolicy const & other);
        
        ParserType aux(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
        PacketParserBase::data_iterator adjust(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;

        ParserType p_;
    };

    template <class P>
    struct DynamicWrapperAuxParserPolicy
    {
        typedef DynamicAuxParserPolicy<P> ParserPolicy;
        typedef P ParserType;

        static PacketParserBase::size_type const aux_bytes = 0;

        DynamicWrapperAuxParserPolicy(ParserPolicy const & other);
        
        ParserType aux(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
        PacketParserBase::data_iterator adjust(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;

        SafePacketParserWrapper<ParserType> p_;
    };
}}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_Packets__decls_) && !defined(HH_AuxParser_i_)
#define HH_AuxParser_i_
//#include "AuxParser.cci"
//#include "AuxParser.ct"
#include "AuxParser.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
