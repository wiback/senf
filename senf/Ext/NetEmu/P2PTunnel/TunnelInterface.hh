//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
        virtual bool v_enabled() const override;
        virtual void v_id(MACAddress const & id) override;
        virtual MACAddress v_id() const override;
        virtual void v_mcAdd(MACAddress const & address) override;
        virtual void v_mcDrop(MACAddress const & address) override;
        virtual boost::uint8_t v_linkTypeId() const override;
        std::string const & v_device() const override;

        detail::TunnelControllerBase & ctrlBase_;
        INet6SocketAddress socketAddress_;
        boost::shared_ptr<InterfaceIdFactoryBase> ifaceIdFactory_;
        MACAddress id_;
        bool isUp_;
        std::string dev_;
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
        virtual void v_enable() override;
        virtual void v_disable() override;
        virtual unsigned v_mtu() const override;
        virtual void v_mtu(unsigned v) override;
        virtual bool v_promisc() const override;
        virtual void v_promisc(bool v) override;
        virtual bool v_annotationMode() const override;
        virtual void v_annotationMode(bool a) override;

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
        virtual void v_enable() override;
        virtual void v_disable() override;
        virtual unsigned v_mtu() const override;
        virtual void v_mtu(unsigned v) override;
        virtual bool v_promisc() const override;
        virtual void v_promisc(bool v) override;
        virtual bool v_annotationMode() const override;
        virtual void v_annotationMode(bool a) override;

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
