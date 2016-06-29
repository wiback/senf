// $Id:$
//
// Copyright (C) 2009
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
//       Christian Niephaus <cni@berlios.de>

/** \file
    \brief HardwareEthernetInterface public header */

#ifndef HH_SENF_Ext_NetEmu_Ethernet_HardwareEthernetInterface_
#define HH_SENF_Ext_NetEmu_Ethernet_HardwareEthernetInterface_ 1

// Custom includes
#include <senf/PPI/QueueingSocketSink.hh>
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

        std::string device() const;

        void qAlgorithm(senf::ppi::QueueingAlgorithm::ptr qAlgorithm);
        senf::ppi::QueueingAlgorithm & qAlgorithm() const;

        std::uint16_t pvid() const;
        bool     pvid(std::uint16_t p); 
        
        unsigned sndBuf();
        void     sndBuf(unsigned sndbuf);
        unsigned rcvBuf();
        void     rcvBuf(unsigned rcvbuf);

        unsigned qlen() const;
        void     qlen(unsigned qlen);

        unsigned rxQueueDropped() const;

#ifdef SENF_DEBUG
        unsigned burstMax();
        unsigned sharedPackets();
#endif

        unsigned maxBurst() const;
        void maxBurst(unsigned maxBurst);

    protected:
        EthernetController & ethController();

        virtual void v_enable();
        virtual void v_disable();

        virtual unsigned v_mtu() const;
        virtual void v_mtu(unsigned v);

    private:
        void init_sockets();
        void close_sockets();
        
        // interface
        virtual bool v_enabled() const;
        virtual void v_id(MACAddress const & id);
        virtual bool v_promisc() const;
        virtual void v_promisc(bool v);
        virtual void v_mcAdd(MACAddress const & address);
        virtual void v_mcDrop(MACAddress const & address);
        virtual void v_flushRxQueues();
        virtual void v_flushTxQueues();

        virtual MACAddress v_id() const;

        void dumpMmapStats(std::ostream & os);

        std::string dev_;
        EthernetController ctrl_;

        unsigned rcvBufSize_;
        unsigned sndBufSize_;
        unsigned qlen_;
        std::uint16_t  pvid_;

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
