// $Id:PacketSocketHandle.hh 218 2007-03-20 14:39:32Z tho $
//
// Copyright (C) 2011
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
    \brief MmapedPacketSocketProtocol and MmapedPacketSocketHandle public header
 */

#ifndef HH_SENF_Socket_Protocols_Raw_MmapedPacketSocketHandle_
#define HH_SENF_Socket_Protocols_Raw_MmapedPacketSocketHandle_ 1

// Custom includes
#include <senf/Socket/SocketPolicy.hh>
#include <senf/Socket/SocketProtocol.hh>
#include <senf/Socket/ProtocolClientSocketHandle.hh>
#include <senf/Socket/FramingPolicy.hh>
#include <senf/Socket/CommunicationPolicy.hh>
#include <senf/Socket/ReadWritePolicy.hh>
#include "LLAddressing.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    //\{

    typedef MakeSocketPolicy<
        LLAddressingPolicy,
        DatagramFramingPolicy,
        UnconnectedCommunicationPolicy,
        NotReadablePolicy,
        NotWriteablePolicy
        >::policy MmapedPacket_Policy;        ///< Policy of MmapedPacketSocketProtocol


    /** \brief
     */
    class MmapedPacketSocketProtocol
        : public ConcreteSocketProtocol<MmapedPacket_Policy, MmapedPacketSocketProtocol>
    {
    public:
        enum SocketType { RawSocket, DatagramSocket };
                                        ///< Socket types

        ///\name Constructors
        //\{
        void init_client(SocketType type = RawSocket, int protocol = -1);
        //\}

        ///\name Protocol Interface
        //\{

        //\}

        ///\name Abstract Interface Implementation
        //\{
        unsigned available() const;
        bool eof() const;
        //\}

    private:
        char * map_;
//        struct iovec * ring_;
        struct tpacket_req req_;
        typedef std::vector<struct iovec> iovecRing;
        iovecRing ring_;
    };


    typedef ProtocolClientSocketHandle<MmapedPacketSocketProtocol> MmapedPacketSocketHandle;
                                        ///< SocketHandle of the MmapedPacketSocketProtocol
                                        /**< \related MmapedPacketSocketProtocol */

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "MmapedPacketSocketHandle.cci"
//#include "MmapedPacketSocketHandle.ct"
//#include "MmapedPacketSocketHandle.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
