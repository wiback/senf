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

#ifndef HH_PacketSocketHandle_
#define HH_PacketSocketHandle_ 1

// Custom includes
#include "SocketPolicy.hh"
#include "SocketProtocol.hh"
#include "ProtocolClientSocketHandle.hh"
#include "LLAddressing.hh"
#include "FramingPolicy.hh"
#include "CommunicationPolicy.hh"
#include "ReadWritePolicy.hh"
#include "BufferingPolicy.hh"
#include "BSDSocketProtocol.hh"

//#include "PacketSocketHandle.mpp"
#include "PacketSocketHandle.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    
    typedef MakeSocketPolicy<
        LLAddressingPolicy,
        DatagramFramingPolicy,
        UnconnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy,
        SocketBufferingPolicy
        >::policy Packet_Policy;

    class PacketProtocol 
        : public ConcreteSocketProtocol<Packet_Policy>,
          public BSDSocketProtocol
    {
    public:
        enum SocketType { RawSocket, DatagramSocket };

        void init_client(SocketType type = RawSocket, int protocol = -1) const;

        std::auto_ptr<SocketProtocol> clone() const;

        enum PromiscMode { Promiscuous, AllMulticast, None };

        void promisc(std::string interface, PromiscMode mode) const;
        // See LLSocketAddress for a discussion/rationale for
        // ForwardRange here
        template <class ForwardRange>
        void mcAdd(std::string interface, ForwardRange const & address) const;
        template <class ForwardRange>
        void mcDrop(std::string interface, ForwardRange const & address) const;

        unsigned available() const;
        bool eof() const;

    private:
        template<class ForwardRange>
        void do_mc(std::string interface, ForwardRange const & address, bool add) const;
        void do_mc_i(std::string interface, detail::LLAddressCopier const & copier, bool add) const;
    };

    typedef ProtocolClientSocketHandle<PacketProtocol> PacketSocketHandle;

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "PacketSocketHandle.cci"
#include "PacketSocketHandle.ct"
#include "PacketSocketHandle.cti"
//#include "PacketSocketHandle.mpp"
#endif


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
