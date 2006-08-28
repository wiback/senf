// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

#ifndef HH_INetProtocol_
#define HH_INetProtocol_ 1

// Custom includes
#include "SocketPolicy.hh"
#include "INetAddress.hh"
#include "ClientSocketHandle.hh"
#include "CommunicationPolicy.hh"

//#include "INetProtocol.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    struct INet4AddressingPolicy : public AddressingPolicyBase
    {
        typedef INet4Address Address;

        template <class Policy>
        static void peer(ClientSocketHandle<Policy> handle, Address & addr,
                         typename IfCommunicationPolicyIs<Policy,ConnectedCommunicationPolicy>::type * = 0);
        static void local(FileHandle handle, Address & addr);

        template <class Policy>
        static void connect(ClientSocketHandle<Policy> handle, Address const & addr,
                            typename IfCommunicationPolicyIs<Policy,ConnectedCommunicationPolicy>::type * = 0);
        static void bind(FileHandle handle, Address const & addr);

    private:
        static void do_peer(FileHandle handle, Address & addr);
        static void do_connect(FileHandle handle, Address const & addr);
    };

    struct INet6AddressingPolicy : public AddressingPolicyBase
    {
        typedef INet6Address Address;

        // TODO: Implement
    };

    class IPv4Protocol 
    {};
    
    class IPv6Protocol
    {};

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "INetProtocol.cci"
//#include "INetProtocol.ct"
#include "INetProtocol.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
