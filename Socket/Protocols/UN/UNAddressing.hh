// $Id$
//
// Copyright (C) 2007 
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
#include "boost/filesystem/path.hpp"
#include "../../../Socket/SocketPolicy.hh"
#include "../../../Socket/ClientSocketHandle.hh"
#include "../../../Socket/CommunicationPolicy.hh"
#include "../../../Socket/Protocols/GenericAddressingPolicy.hh"
#include "UNAddress.hh"

//#include "UNAddressing.mpp"
///////////////////////////////hh.p////////////////////////////////////////
namespace senf {
    class UNSocketAddress
        : public ComparableSafeBool<UNSocketAddress>
    {
    public:

        //UNSocketAddress();
        explicit UNSocketAddress(boost::filesystem::path p);
                                        ///< Construct an address constant
        static UNSocketAddress from_string(std::string const s);
        static UNSocketAddress from_path(boost::filesystem::path const p);
        static std::string path();
        static sockaddr_un sockaddr(); 
        struct sockaddr * sockaddr_p();
        struct sockaddr const * sockaddr_p() const;
        unsigned sockaddr_len() const;
    private:
        static struct sockaddr_un sockAddr;
    };
    std::ostream & operator<<(std::ostream & os, UNSocketAddress const & addr);


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
