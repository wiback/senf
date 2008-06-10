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

    /** \brief Internal: Prefix aux-parser policy

        Place auxiliary field directly before a container/collection.
     */
    template <class P>
    struct PrefixAuxParserPolicy
    {
        typedef PrefixAuxParserPolicy WrapperPolicy;
        typedef PrefixAuxParserPolicy ParserPolicy;

        static PacketParserBase::size_type const aux_bytes = P::fixed_bytes;
        
        typename P::value_type aux(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
        void aux(typename P::value_type const & v, PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
        PacketParserBase::data_iterator adjust(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
    };

    /** \brief Internal: Fixed distance aux-parser policy

        Place auxiliary field a fixed distance before the container/collection
     */
    template <class P, unsigned Dist>
    struct FixedAuxParserPolicy
    {
        typedef FixedAuxParserPolicy WrapperPolicy;
        typedef FixedAuxParserPolicy ParserPolicy;

        static PacketParserBase::size_type const aux_bytes = 0;
        
        typename P::value_type aux(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
        void aux(typename P::value_type const & v, PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
        PacketParserBase::data_iterator adjust(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
    };

    template <class P> struct DynamicWrapperAuxParserPolicy;
    
    /** \brief Internal: Dynamic aux-parser policy

        Place auxiliary field at a variable distance before the container/collection
     */
    template <class P>
    struct DynamicAuxParserPolicy
    {
        typedef DynamicWrapperAuxParserPolicy<P> WrapperPolicy;
        typedef DynamicAuxParserPolicy<P> ParserPolicy;

        static PacketParserBase::size_type const aux_bytes = 0;

        DynamicAuxParserPolicy(P p);
        DynamicAuxParserPolicy(WrapperPolicy const & other);
        
        typename P::value_type aux(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
        void aux(typename P::value_type const & v, PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
        PacketParserBase::data_iterator adjust(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;

        mutable P p_;
    };

    /** \brief Internal: Dynamic aux-parser policy (container wrapper)

        Place auxiliary field at a variable distance before the container/collection. This is the
        wrapper policy used by DynamicAuxParserPolicy
     */
    template <class P>
    struct DynamicWrapperAuxParserPolicy
    {
        typedef DynamicWrapperAuxParserPolicy<P> WrapperPolicy;
        typedef DynamicAuxParserPolicy<P> ParserPolicy;

        static PacketParserBase::size_type const aux_bytes = 0;

        DynamicWrapperAuxParserPolicy(ParserPolicy const & other);
        
        typename P::value_type aux(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
        void aux(typename P::value_type const & v, PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
        PacketParserBase::data_iterator adjust(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;

        mutable SafePacketParserWrapper<P> p_;
    };

    /** \brief Internal: Apply transformation to arbitrary aux-parser policy

        Transform must statisfy the interface
        \code
        struct Transform
        {
            typedef unspecified value_type;
            static value_type get(unspecified v);
            static unspecified set(value_type v);
        };
        \endcode
     */
    template <class Policy, class Transform>
    struct TransformAuxParserPolicy
        : public Policy
    {
        typedef TransformAuxParserPolicy<typename Policy::WrapperPolicy, Transform> WrapperPolicy;
        typedef TransformAuxParserPolicy<typename Policy::ParserPolicy, Transform> ParserPolicy;

        static PacketParserBase::size_type const aux_bytes = Policy::aux_bytes;
        
        TransformAuxParserPolicy();
        template <class Arg> TransformAuxParserPolicy(Arg const & arg);

        typename Transform::value_type aux(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
        void aux(typename Transform::value_type const & v, PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
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
