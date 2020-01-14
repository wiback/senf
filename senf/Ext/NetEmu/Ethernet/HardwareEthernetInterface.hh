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
    \brief HardwareEthernetInterface public header */

#ifndef HH_SENF_Ext_NetEmu_Ethernet_HardwareEthernetInterface_
#define HH_SENF_Ext_NetEmu_Ethernet_HardwareEthernetInterface_ 1

// Custom includes
#include <senf/PPI/QueueingSocketSink.hh>
#include <senf/Ext/NetEmu/VLanId.hh>
#include "EthernetController.hh"
#include "EthernetInterface.hh"

#include "HardwareEthernetInterface.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    /** \brief Hardware Ethernet Interface

        This class adapts a real, hardware based Ethernet interface into the \ref
        senf_emu_interfaces.

        \see WiredInterface
        \ingroup senf_emu_interface_implementations
     */
    class HardwareEthernetInterface
        : private detail::HardwareEthernetInterfaceNet,
          public EthernetInterface,
          public HardwareInterface
    {
    public:
        typedef TypedInterfaceDecorator<
            HardwareEthernetInterface, Interface::Decorator> Decorator;

        HardwareEthernetInterface(std::string const & name);

        virtual ~HardwareEthernetInterface();
        
        std::string const & v_device() const override;

        void setREDFilterCallbackRx(senf::ppi::REDFilterCallback const & cb);
        void setREDFilterCallbackTx(senf::ppi::REDFilterCallback const & cb);

        VLanId const & pvid() const;
        bool pvid(VLanId const & p, bool accessMode); 
        std::uint32_t vlanMismatchRx();
        std::uint32_t vlanMismatchTx();

        unsigned sndBuf();
        void     sndBuf(unsigned sndbuf);
        unsigned rcvBuf();
        void     rcvBuf(unsigned rcvbuf);

        unsigned qlen() const;
        void     qlen(unsigned qlen);

        std::pair<unsigned,unsigned> rxDropped();
        std::tuple<unsigned,unsigned,unsigned> txDropped();

        void dumpMmapStats(std::ostream & os);
        bool isDead() const;

#ifdef SENF_DEBUG
        unsigned burstMax();
#endif

        unsigned maxBurst() const;
        void maxBurst(unsigned maxBurst);

        EthernetController & ethController();

    protected:

        virtual void v_enable() override;
        virtual void v_disable() override;

        virtual unsigned v_mtu() const override;
        virtual void v_mtu(unsigned v) override;

    private:
        void init_sockets();
        void close_sockets();
        
        // interface
        virtual bool v_enabled() const override;
        virtual void v_id(MACAddress const & id) override;
        virtual bool v_annotationMode() const override;
        virtual void v_annotationMode(bool a) override;
        virtual bool v_promisc() const override;
        virtual void v_promisc(bool v) override;
        virtual void v_mcAdd(MACAddress const & address) override;
        virtual void v_mcDrop(MACAddress const & address) override;
        virtual void v_flushRxQueues() override;
        virtual void v_flushTxQueues() override;

        virtual MACAddress v_id() const override;

        std::string dev_;
        EthernetController ctrl_;

        unsigned rcvBufSize_;
        unsigned sndBufSize_;
        unsigned qlen_;
        VLanId  pvid_;
        bool accessMode_;
        senf::MACAddress initialId_;

        friend struct detail::HardwareEthernetInterfaceNet;
        friend class EthernetAnnotator;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "HardwareEthernetInterface.cci"
//#include "HardwareEthernetInterface.ct"
//#include "HardwareEthernetInterface.cti"
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
