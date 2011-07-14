// $Id$
//
// Copyright (C) 2008
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
    \brief AuxParser public header */

#ifndef HH_SENF_Packets_AuxParser_
#define HH_SENF_Packets_AuxParser_ 1

#ifndef HH_SENF_Packets_Packets_
#error "Don't include 'AuxParser.hh' directly, include 'Packets.hh'"
#endif

// Custom includes
#include "PacketParser.hh"
#include "SafeIterator.hh"

//#include "AuxParser.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

        Transform must satisfy the interface
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

    struct PacketSizeAuxParserPolicy
    {
        typedef PacketSizeAuxParserPolicy WrapperPolicy;
        typedef PacketSizeAuxParserPolicy ParserPolicy;

        static PacketParserBase::size_type const aux_bytes = 0;

        PacketParserBase::size_type aux(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
        void aux(unsigned v, PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
        PacketParserBase::data_iterator adjust(PacketParserBase::data_iterator i, PacketParserBase::state_type s) const;
    };
}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_AuxParser_i_)
#define HH_SENF_Packets_AuxParser_i_
#include "AuxParser.cci"
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
