// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief GenericTLV public header */

#ifndef HH_SENF_Packets_GenericTLV_
#define HH_SENF_Packets_GenericTLV_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include <senf/Utils/type_traits.hh>

//#include "GenericTLV.hh.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
 
    template <class Base>
    class GenericTLVParserBase : public Base
    {
    public:
        GenericTLVParserBase(senf::PacketParserBase::data_iterator i, senf::PacketParserBase::state_type s) 
            : Base(i,s) {}
    
        senf::PacketParserBase::size_type bytes();
        void init() const;

        template <class Parser>
        Parser init();
        
        template <class Parser>
        Parser as();
        
        template <class Parser>
        bool is();

        senf::PacketInterpreterBase::range value() const;
        
        template<class ForwardReadableRange>
        void value(ForwardReadableRange const & val,
                typename boost::disable_if<senf::is_pair<ForwardReadableRange> >::type * = 0);

        template<class First, class Second>
        void value(std::pair<First, Second> const & val,
                typename boost::disable_if<boost::is_convertible<First, typename Base::type_t::value_type> >::type * = 0);

        template <class Type, class ForwardReadableRange>
        void value(std::pair<Type, ForwardReadableRange> const & val,
                typename boost::enable_if<boost::is_convertible<Type, typename Base::type_t::value_type> >::type * = 0);        

    private:
        template<class ForwardReadableRange>
        void value_(ForwardReadableRange const &range);
    };
}


///////////////////////////////hh.e////////////////////////////////////////
//#include "GenericTLV.cci"
#include "GenericTLV.ct"
#include "GenericTLV.cti"
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
