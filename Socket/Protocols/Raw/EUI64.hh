// $Id$
//
// Copyright (C) 2009 
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
    \brief EUI64 public header */

#ifndef HH_SENF_Socket_Protocols_Raw_EUI64_
#define HH_SENF_Socket_Protocols_Raw_EUI64_ 1

// Custom includes
#include <iostream>
#include <boost/cstdint.hpp>
#include <boost/array.hpp>
#include "../../../Utils/Tags.hh"
#include "../../../Utils/safe_bool.hh"

//#include "EUI64.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    class MACAddress;

    /** \brief EUI-64 data type
        
        An EUI-64 is a 64 bit (8 octet) id. 
     */
    class EUI64
        : public boost::array<boost::uint8_t,8>,
          public senf::comparable_safe_bool<EUI64>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // default copy constructor
        // default copy assignment
        // default destructor
        // no conversion constructors

        explicit EUI64(boost::uint64_t v=0u);
        explicit EUI64(senf::NoInit_t);

        static EUI64 from_mac(MACAddress const & mac);
        static EUI64 from_string(std::string const & s);
        template <class InputIterator>
        static EUI64 from_data(InputIterator i);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        bool isMACCompatible() const;
        bool boolean_test() const;
        boost::uint64_t uint64() const;
    };

    std::ostream & operator<<(std::ostream & os, EUI64 const & v);
    std::istream & operator>>(std::istream & is, EUI64 & v);

}

///////////////////////////////hh.e////////////////////////////////////////
#include "EUI64.cci"
//#include "EUI64.ct"
#include "EUI64.cti"
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
