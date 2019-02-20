// $Id:$
//
// Copyright (C) 2012
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
//       Thorsten Horstmann <Thorsten.Horstmann@fokus.fraunhofer.de>

/** \file
    \brief TunnelController internal header */

#ifndef HH_SENF_Ext_NetEmu_P2PTunnel_TunnelController_
#define HH_SENF_Ext_NetEmu_P2PTunnel_TunnelController_ 1

// Custom includes
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <senf/Packets/Packets_fwd.hh>
#include <senf/Scheduler/TimerEvent.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Socket/Protocols/INet/UDPSocketHandle.hh>
#include <senf/PPI/QueueingAlgorithm.hh>
#include <senf/Utils/FlowStatistics.hh>
#include <senf/Ext/NetEmu/Ethernet/EthernetFragmenter.hh>
#include <senf/Ext/NetEmu/Ethernet/EthernetReassembler.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace senf {
namespace emu {

    // forward declarations:
    class TunnelInterfaceBase;
    class TunnelServerInterface;
    class TunnelClientInterface;

    SENF_PACKET_FWD_DECL( TunnelCtrlPacket );
    SENF_PACKET_FWD_DECL( TunnelHeaderPacket );

    namespace tunnel {
        enum CapacityDirection { FromClientToServer, FromServerToClient };
        typedef std::map<CapacityDirection, unsigned> Capacity;
    }

namespace detail {

    struct TunnelIOStatistics
    {
        senf::ClockService::clock_type tstamp;
        
        unsigned rxPackets;
        unsigned rxData;
        unsigned rxControl;
        unsigned rxIgnored;

        unsigned txPackets;
        unsigned txSent;
        unsigned txError;
        unsigned txOverrun;
        unsigned txDSQDropped;

        TunnelIOStatistics();
        void reset();            
        senf::ClockService::clock_type duration() const;
        TunnelIOStatistics stats();
        void dump(std::ostream & os) const;
        std::string dump() const;
    };
    
    class TunnelControllerBase
    {
    public:
        // The tunnel's MAC header must be accounted for, as it counts as payload in this case, while the Ethernet Fragmenter would exclude it
        static const unsigned TunnelOverhead = 46u; // IPv4 (20) + UDP (8) + TunnelHeader (4) + Tunnel MAC (14)

        typedef UDPv6ClientSocketHandle Handle;
        typedef EthernetPacket PacketType;

        PacketType readPacket(Handle & handle);
        bool writePacket(Handle & handle, PacketType packet);

        ClockService::clock_type timeout() const;
        void timeout(ClockService::clock_type t);

        ppi::QueueingAlgorithm & qAlgorithm() const;
        void qAlgorithm(ppi::QueueingAlgorithm::ptr qAlgorithm);

        void dumpInfo(std::ostream & os);

        unsigned fragmentationCount();

    protected:
        TunnelControllerBase(TunnelInterfaceBase & interface);
        virtual ~TunnelControllerBase() {};

        bool sendPkt(Handle & handle, MACAddress const & dstMAC, senf::EthernetPacket pkt);
        void do_sendPkt(Handle & handle, senf::EthernetPacket & pkt, std::pair<senf::INet6SocketAddress,unsigned> const & txInfo);
        void do_sendPkt(Handle & handle, senf::EthernetPacket & pkt);
        void flushQueue(Handle & handle);

        static bool isTunnelCtrlPacket(EthernetPacket const & eth);
        void sendCtrlPacket(Handle & handle, MACAddress const & dstMAC, boost::uint8_t code);
        void sendCtrlPacket(Handle & handle, MACAddress const & dstMAC, TunnelCtrlPacket ctrlPacket);

        virtual void v_prependTHdr(EthernetPacket & eth) = 0;
        virtual std::pair<INet6SocketAddress,unsigned> v_getTxInfo(Packet const & eth) const = 0;

    private:
        virtual void v_handleCtrlPacket(EthernetPacket const & ctrlPacket, INet6SocketAddress const & srcAddr, Handle & handle) = 0;
        virtual signed v_processSequenceNumber(TunnelHeaderPacket const & thdr, INet6SocketAddress const & srcAddr) = 0;
        virtual bool v_writePacket(Handle & handle, PacketType & packet) = 0;
        virtual void v_dumpInfo(std::ostream & os) const = 0;
        virtual void v_timeoutChanged() {};

        ClockService::clock_type timeout_;
        TunnelInterfaceBase & interface_;
        boost::scoped_ptr<ppi::QueueingAlgorithm> qAlgo_;
        EthernetFragmenter fragmenter_;
        EthernetReassembler reassembler_;
        TunnelIOStatistics stats_;
    protected:
        senf::Detail::DifferenceSigned seqNoDiff_;
    };


    class TunnelServerController
        : public TunnelControllerBase
    {
    public:
        typedef TunnelServerInterface Interface;

        TunnelServerController(TunnelServerInterface & interface);
        virtual ~TunnelServerController() {}
        void terminateAllClients(Handle handle);
        unsigned capacity(MACAddress const & clientAddr, tunnel::CapacityDirection direction) const;

        unsigned fragmentationThreshold(MACAddress const & clientAddr) const;
        void fragmentationThreshold(MACAddress const & clientAddr, unsigned mtu);

    protected:
        void v_prependTHdr(EthernetPacket & eth) override;
        std::pair<INet6SocketAddress,unsigned> v_getTxInfo(Packet const & eth) const override;

    private:
        void v_handleCtrlPacket(EthernetPacket const & ctrlPacket, INet6SocketAddress const & srcAddr, Handle & handle) override;
        signed v_processSequenceNumber(TunnelHeaderPacket const & thdr, INet6SocketAddress const & srcAddr) override;
        bool v_writePacket(Handle & handle, PacketType & packet) override;
        void v_dumpInfo(std::ostream & os) const override;
        void sendSetupRequest();
        void processTimeout();
        void resetTimer();

        struct TunnelClient
        {
            MACAddress macAddr;
            INet6SocketAddress inetAddr;
            ClockService::clock_type lastSeen;
            tunnel::Capacity capacity;
            unsigned txSeqNo;
            unsigned rxSeqNo;
            unsigned reSyncs;
            unsigned fragmentationThreshold;

            struct incrTxSeqNo {
                void operator()(TunnelClient & client){
                    client.txSeqNo = (client.txSeqNo + 1) % 0x20000;
                }
            };
            struct incReSyncs {
                void operator()(TunnelClient & client){
                    client.reSyncs++;
                }
            };
            struct updateRxSeqNo {
                unsigned seqno;
                updateRxSeqNo(unsigned _seqno)
                    : seqno(_seqno) {} ;
                void operator()(TunnelClient & client){
                    client.rxSeqNo = seqno;
                }
            };
            struct updateFragmentationThreshold {
                unsigned fragmentationThreshold;
                updateFragmentationThreshold(unsigned _fragmentationThreshold)
                    : fragmentationThreshold(_fragmentationThreshold) {} ;
                void operator()(TunnelClient & client){
                    client.fragmentationThreshold = fragmentationThreshold;
                }
            };
            struct updateLastSeen {
                void operator()(TunnelClient & client) {
                    client.lastSeen = scheduler::now();
                }
            };
            struct updateInetAddr {
                INet6SocketAddress inetAddr;
                updateInetAddr(INet6SocketAddress const & _inetAddr)
                    : inetAddr(_inetAddr) {}
                void operator()(TunnelClient & client) {
                    client.inetAddr = inetAddr;
                }
            };
            struct updateCapacity {
                tunnel::CapacityDirection direction;
                unsigned capacity;
                updateCapacity(tunnel::CapacityDirection _direction, unsigned _capacity)
                    : direction(_direction), capacity(_capacity) {}
                void operator()(TunnelClient & client) {
                    client.capacity[direction] = capacity;
                }
            };

            TunnelClient(MACAddress const & _macAddr, INet6SocketAddress const & _inetAddr)
                : macAddr(_macAddr), inetAddr(_inetAddr), lastSeen(scheduler::now()), txSeqNo(0), rxSeqNo(0xffffffff), reSyncs(0),
                  fragmentationThreshold(1280u) {}
        };

        struct ByLastSeen {};
        struct ByINetAddr {};
        struct ByMACAddr {};
        struct ClientIndexes
            : public boost::multi_index::indexed_by<
                  boost::multi_index::ordered_non_unique<
                      boost::multi_index::tag<ByLastSeen>,
                      BOOST_MULTI_INDEX_MEMBER(TunnelClient, ClockService::clock_type, lastSeen)>,
                  boost::multi_index::ordered_unique<
                      boost::multi_index::tag<ByMACAddr>,
                      BOOST_MULTI_INDEX_MEMBER(TunnelClient, MACAddress, macAddr)>,
                  boost::multi_index::ordered_unique<
                      boost::multi_index::tag<ByINetAddr>,
                      BOOST_MULTI_INDEX_MEMBER(TunnelClient, INet6SocketAddress, inetAddr)> >
        {};

        typedef boost::multi_index_container<TunnelClient, ClientIndexes> Clients;
        typedef Clients::index<ByLastSeen>::type Clients_by_lastSeen;
        typedef Clients::index<ByMACAddr>::type Clients_by_macAddr;
        typedef Clients::index<ByINetAddr>::type Clients_by_inetAddr;

        Clients clients_;
        Clients_by_lastSeen & clients_by_lastSeen_;
        Clients_by_macAddr & clients_by_macAddr_;
        Clients_by_inetAddr & clients_by_inetAddr_;

        scheduler::TimerEvent timer_;
    };


    class TunnelClientController
        : public TunnelControllerBase
    {
    public:
        typedef TunnelClientInterface Interface;

        TunnelClientController(TunnelClientInterface & interface);
        virtual ~TunnelClientController() {}

        bool established() const;
        void reset();

        void serverAddress(INet6SocketAddress const & address);
        INet6SocketAddress const & serverAddress() const;

        unsigned fragmentationThreshold() const;
        void fragmentationThreshold(unsigned mtu);

    protected:
        void v_prependTHdr(EthernetPacket & eth) override;
        std::pair<INet6SocketAddress,unsigned> v_getTxInfo(Packet const & eth) const override;

    private:
        void v_handleCtrlPacket(EthernetPacket const & ctrlPacket, INet6SocketAddress const & srcAddr, Handle & handle) override;
        signed v_processSequenceNumber(TunnelHeaderPacket const & thdr, INet6SocketAddress const & srcAddr) override;
        bool v_writePacket(Handle & handle, PacketType & packet) override;
        void v_dumpInfo(std::ostream & os) const override;
        void v_timeoutChanged() override;
        void sendSetupRequest();
        void processTimeout();

        TunnelClientInterface & interface_;
        bool established_;
        scheduler::TimerEvent timer_;
        ClockService::clock_type echoInterval_;
        ClockService::clock_type serverLastSeen_;
        senf::MACAddress serverMAC_;
        INet6SocketAddress serverINet_;
        unsigned txSeqNo_;
        unsigned rxSeqNo_;
        unsigned fragmentationThreshold_;
        unsigned setupRequests_;
        unsigned reSyncs_;
        unsigned reordered_;
        unsigned duplicate_;
    };

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
