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
    \brief TunnelInterface public header */

#ifndef HH_SENF_Ext_NetEmu_P2PTunnel_TunnelInterface_
#define HH_SENF_Ext_NetEmu_P2PTunnel_TunnelInterface_ 1

// Custom includes
#include "senf/Ext/NetEmu/InterfaceAPIBase.hh"
#include "senf/Ext/NetEmu/InterfaceId.hh"
#include "TunnelController.hh"

#include "TunnelInterface.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    class TunnelInterfaceBase
        : public BidirectionalWiredInterface,
          public HardwareInterface
    {
    public:
        virtual ~TunnelInterfaceBase();

        INet6SocketAddress const & address() const;

        ClockService::clock_type timeout() const;
        void timeout(ClockService::clock_type t);

        boost::shared_ptr<InterfaceIdFactoryBase> ifaceIdFactory();
        void ifaceIdFactory(boost::shared_ptr<InterfaceIdFactoryBase> factory);

        void qAlgorithm(senf::ppi::QueueingAlgorithm::ptr qAlgorithm);
        senf::ppi::QueueingAlgorithm & qAlgorithm() const;
        
    protected:
        template <class Output, class Input>
        TunnelInterfaceBase(Output & output, Input & input, detail::TunnelControllerBase & ctrlBase, INet6SocketAddress const & address);

        void isUp(bool up);

        detail::TunnelControllerBase & tunnelCtrlBase();
        detail::TunnelControllerBase & tunnelCtrlBase() const;

    private:
        // interface API
        virtual bool v_enabled() const;
        virtual void v_id(MACAddress const & id);
        virtual MACAddress v_id() const;
        virtual void v_mcAdd(MACAddress const & address);
        virtual void v_mcDrop(MACAddress const & address);
        virtual boost::uint8_t v_linkTypeId() const;

        detail::TunnelControllerBase & ctrlBase_;
        INet6SocketAddress socketAddress_;
        boost::shared_ptr<InterfaceIdFactoryBase> ifaceIdFactory_;
        MACAddress id_;
        bool isUp_;
    };


    /** \brief Tunnel Server Interface

        \see WiredInterface
        \ingroup senf_emu_interface_implementations
     */
    class TunnelServerInterface
        : private detail::TunnelServerInterfaceNet,
          public TunnelInterfaceBase
    {
    public:
        typedef TypedInterfaceDecorator<TunnelServerInterface, Interface::Decorator> Decorator;

        TunnelServerInterface(INet6SocketAddress const & address);
        TunnelServerInterface(INet4SocketAddress const & address);

        unsigned capacity(MACAddress const & client, tunnel::CapacityDirection direction) const;

        unsigned sndBuf();
        void     sndBuf(unsigned sndbuf);
        unsigned rcvBuf();
        void     rcvBuf(unsigned rcvbuf);

        unsigned fragmentationThreshold(MACAddress const & client) const;
        void fragmentationThreshold(MACAddress const & client, unsigned mtu);

        unsigned maxBurst() const;
        void maxBurst(unsigned v);

    private:
        // interface API
        virtual void v_enable();
        virtual void v_disable();
        virtual unsigned v_mtu() const;
        virtual void v_mtu(unsigned v);
        virtual bool v_promisc() const;
        virtual void v_promisc(bool v);

    private:
        unsigned sndBufSize_;
        unsigned rcvBufSize_;
    };


    class TunnelClientInterface
        : private detail::TunnelClientInterfaceNet,
          public TunnelInterfaceBase
    {
    public:
        typedef TypedInterfaceDecorator<TunnelClientInterface, Interface::Decorator> Decorator;

        TunnelClientInterface(INet6SocketAddress const & address);
        TunnelClientInterface(INet4SocketAddress const & address);

        void serverAddress(INet6SocketAddress const & address);
        void serverAddress(INet4SocketAddress const & address);
        INet6SocketAddress const & serverAddress() const;

        bool established() const;
        
        unsigned capacity(tunnel::CapacityDirection direction) const;
        void capacity(tunnel::CapacityDirection direction, unsigned cap);

        unsigned sndBuf();
        void     sndBuf(unsigned sndbuf);
        unsigned rcvBuf();
        void     rcvBuf(unsigned rcvbuf);

        unsigned fragmentationThreshold() const;
        void fragmentationThreshold(unsigned mtu);
        
        unsigned maxBurst() const;
        void maxBurst(unsigned v);

    private:
        // interface API
        virtual void v_enable();
        virtual void v_disable();
        virtual unsigned v_mtu() const;
        virtual void v_mtu(unsigned v);
        virtual bool v_promisc() const;
        virtual void v_promisc(bool v);

        tunnel::Capacity capacity_;

        unsigned sndBufSize_;
        unsigned rcvBufSize_;

        friend class detail::TunnelClientController;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "TunnelInterface.cci"
//#include "TunnelInterface.ct"
//#include "TunnelInterface.cti"
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
