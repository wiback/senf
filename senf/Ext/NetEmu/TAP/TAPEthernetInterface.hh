//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief TAPEthernetInterface public header */

#ifndef HH_SENF_Ext_NetEmu_TAP_TAPEthernetInterface_
#define HH_SENF_Ext_NetEmu_TAP_TAPEthernetInterface_ 1

// Custom includes
#include <senf/PPI/QueueingSocketSink.hh>
#include <senf/Ext/NetEmu/VLanId.hh>
#include "senf/Ext/NetEmu/InterfaceAPIBase.hh"
#include "senf/Ext/NetEmu/Ethernet/EthernetController.hh"
#include "senf/Ext/NetEmu/Ethernet/EthernetInterface.hh"

#include "TAPEthernetInterface.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    /** \brief Hardware Ethernet Interface

        This class adapts a real, hardware based Ethernet interface into the \ref
        senf_emu_interfaces.

        \see WiredInterface
        \ingroup senf_emu_interface_implementations
     */
    class TAPEthernetInterface
        : private detail::TAPEthernetInterfaceNet,
          public EthernetInterface,
          public HardwareInterface
    {
    public:
        typedef TypedInterfaceDecorator<
            TAPEthernetInterface, Interface::Decorator> Decorator;

        TAPEthernetInterface(std::string const & name);
        ~TAPEthernetInterface();

        std::string const & v_device() const override;

        unsigned maxBurst() const;
        void maxBurst(unsigned maxBurst);

        VLanId const & pvid() const;
        bool pvid(VLanId const & p, bool accessMode); 
        std::uint32_t vlanMismatchRx();
        std::uint32_t vlanMismatchTx();

    protected:
        EthernetController & ethController();

        virtual void v_enable() override;
        virtual void v_disable() override;

        virtual unsigned v_mtu() const override;
        virtual void v_mtu(unsigned v) override;

    private:
        VLanId pvid_;
        bool accessMode_;

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

        virtual MACAddress v_id() const override;
        
        void dumpMmapStats(std::ostream & os);

        friend struct detail::TAPEthernetInterfaceNet;
        friend class TAPAnnotator;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "TAPEthernetInterface.cci"
//#include "TAPEthernetInterface.ct"
//#include "TAPEthernetInterface.cti"
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
