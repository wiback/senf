// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     David Wagner <dw6@berlios.de>
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
    \brief UNAddressing public header */

#ifndef HH_UNAddressing_
#define HH_UNAddressing_ 1

// Custom includes
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <boost/cstdint.hpp>
#include <boost/operators.hpp>
#include "../../../Socket/SocketPolicy.hh"
#include "../../../Socket/ClientSocketHandle.hh"
#include "../../../Socket/CommunicationPolicy.hh"
#include "../../../Socket/Protocols/GenericAddressingPolicy.hh"
#include "../../../Utils/safe_bool.hh"

//#include "UNAddressing.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Unix domain socket address

        UNSocketAddress wraps the standard sockaddr_un datatype. It provides simple accessor methods
        to access the path. 
        
        \implementation This implementation is based on sockaddr_un.

        \ingroup addr_group

        \fixme Why both std::string constructor and from_string member ?
     */
    class UNSocketAddress
        : public comparable_safe_bool<UNSocketAddress>
    {
    public:
        UNSocketAddress(); 
        explicit UNSocketAddress(std::string p);
                                        ///< Construct an address constant from given path
        static UNSocketAddress from_string(std::string const s); 
                                        ///< Create UNSocketAddress from string

        bool operator==(UNSocketAddress const & other) const;
                                        ///< Compare UNSocketAddress for equality

        std::string path() const ;      ///< Return path as string

        bool boolean_test() const;      ///< \c true, if address is not empty
        
        void clear();                   ///< Clear address

        struct sockaddr * sockaddr_p() ;
        struct sockaddr const * sockaddr_p() const;
        unsigned sockaddr_len() const;

    private:
        struct sockaddr_un addr_;
    };

    /** \brief Write path to os

        \related UNSocketAddress
     */
    std::ostream & operator<<(std::ostream & os, UNSocketAddress const & addr);

    /// \addtogroup policy_impl_group
    /// @{

    /** \brief Addressing policy supporting unix domain addressing

        \par Address Type:
            UNAddress

        This addressing policy implements addressing using unix domain
        addresses.

        The various members are directly imported from
        GenericAddressingPolicy which see for a detailed
        documentation.
     */
    struct UNAddressingPolicy
        : public AddressingPolicyBase,
          private GenericAddressingPolicy<UNSocketAddress>
    {
        typedef UNSocketAddress Address;

        using GenericAddressingPolicy<UNSocketAddress>::peer;
        using GenericAddressingPolicy<UNSocketAddress>::local;
        using GenericAddressingPolicy<UNSocketAddress>::connect;
        using GenericAddressingPolicy<UNSocketAddress>::bind;
    };

    ///@}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "UNAddressing.cci"
//#include "UNAddressing.ct"
//#include "UNAddressing.cti"
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
