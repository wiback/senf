// $Id$
//
// Copyright (C) 2009
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief MIHTypes public header */

#ifndef HH_SENF_Packets_80221Bundle_MIHTypes_
#define HH_SENF_Packets_80221Bundle_MIHTypes_ 1

// Custom includes
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
#include <senf/Socket/Protocols/INet/INet4Address.hh>
#include <senf/Socket/Protocols/INet/INet6Address.hh>
#include <boost/function_output_iterator.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/variant.hpp>

//#include "MIHTypes.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    class MIHFId
        : public boost::variant< boost::blank, senf::MACAddress, senf::INet4Address,
                senf::INet6Address, std::string, senf::EUI64 >,
          public boost::less_than_comparable<MIHFId>,
          public boost::equality_comparable<MIHFId>
    {
    public:
        static MIHFId const Multicast; ///< The multicast (empty) MIHF Id
        enum Type { MulticastType, MACAddress, INet4Address, INet6Address, String, EUI64 };

        MIHFId();                                   ///< Create multicast id.
        MIHFId(senf::MACAddress const & addr);      ///< Construct id with given MACAddress
        MIHFId(senf::INet4Address const & addr);    ///< Construct id with given INet4Address
        MIHFId(senf::INet6Address const & addr);    ///< Construct id with given INet6Address
        MIHFId(std::string const & addr);           ///< Construct id with given String
        MIHFId(senf::EUI64 const & addr);           ///< Construct id with given EUI64

        Type type() const;
        bool operator==(MIHFId const & other) const;
        bool operator<(MIHFId const & other) const;

    private:
        struct GetTypeVisitor : public boost::static_visitor<Type> {
            Type operator()(boost::blank const &) const { return MulticastType; }
            Type operator()(senf::MACAddress const &) const { return MACAddress; }
            Type operator()(senf::INet4Address const &) const { return INet4Address; }
            Type operator()(senf::INet6Address const &) const { return INet6Address; }
            Type operator()(std::string const & ) const { return String; }
            Type operator()(senf::EUI64 const &) const { return EUI64; }
        };
        struct EqualsVisitor : public boost::static_visitor<bool> {
            template <typename T, typename U>
            bool operator()(T const &, U const &) const {
                return false;
            }
            template <typename T>
            bool operator()( const T & lhs, const T & rhs ) const {
                return lhs == rhs;
            }
        };
        struct LessThanVisitor : public boost::static_visitor<bool> {
            template <typename T, typename U>
            bool operator()(T const &, U const &) const {
                return false;
            }
            template <typename T>
            bool operator()( const T & lhs, const T & rhs ) const {
                return lhs < rhs;
            }
        };
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "MIHTypes.cci"
//#include "MIHTypes.ct"
//#include "MIHTypes.cti"
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
