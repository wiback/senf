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

/** \file

    \todo Implement possibly non-blocking connect and SO_ERROR in the
    protocol interface
 */

#ifndef HH_TCPSocketHandle_
#define HH_TCPSocketHandle_ 1

// Custom includes
#include "INetProtocol.hh"
#include "TCPProtocol.hh"
#include "BSDSocketProtocol.hh"
#include "FramingPolicy.hh"
#include "CommunicationPolicy.hh"
#include "ReadWritePolicy.hh"
#include "BufferingPolicy.hh"
#include "ProtocolClientSocketHandle.hh"
#include "ProtocolServerSocketHandle.hh"

//#include "TCPSocketHandle.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {


    typedef MakeSocketPolicy<
        INet4AddressingPolicy,
        StreamFramingPolicy,
        ConnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy,
        SocketBufferingPolicy
        >::policy TCPv4Socket_Policy;

    class TCPv4SocketProtocol
        : public ConcreteSocketProtocol<TCPv4Socket_Policy>,
          public IPv4Protocol, 
          public TCPProtocol,
          public BSDSocketProtocol,
          public AddressableBSDSocketProtocol
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // internal interface

        void init_client() const;
        void init_client(INet4Address const & address) const;
        void init_server() const;
        void init_server(INet4Address const & address, unsigned backlog=1) const;

        std::auto_ptr<SocketProtocol> clone() const;
    };

    typedef ProtocolClientSocketHandle<TCPv4SocketProtocol> TCPv4ClientSocketHandle;
    typedef ProtocolServerSocketHandle<TCPv4SocketProtocol> TCPv4ServerSocketHandle;

    typedef MakeSocketPolicy<
        TCPv4Socket_Policy,
        INet6AddressingPolicy
        >::policy TCPv6Socket_Policy;

    class TCPv6SocketProtocol
        : public ConcreteSocketProtocol<TCPv6Socket_Policy>, 
          public IPv6Protocol,
          public TCPProtocol,
          public BSDSocketProtocol,
          public AddressableBSDSocketProtocol
    {
        /** \todo Implement */
    };

    typedef ProtocolClientSocketHandle<TCPv6SocketProtocol> TCPv6ClientSocketHandle;
    typedef ProtocolServerSocketHandle<TCPv6SocketProtocol> TCPv6ServerSocketHandle;

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "TCPSocketHandle.cci"
//#include "TCPSocketHandle.ct"
//#include "TCPSocketHandle.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
