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
//       Stefan Bund <g0dil@berlios.de>

/** \file
    \brief EmulatedInterface public header */

#ifndef HH_SENF_Ext_NetEmu_EmulatedInterface_
#define HH_SENF_Ext_NetEmu_EmulatedInterface_ 1

// Custom includes
#include <senf/Socket/Protocols/INet/UDPSocketHandle.hh>
#include <senf/PPI/SocketSink.hh>
#include <senf/PPI/SocketSource.hh>
#include "Modules/PacketDelayer.hh"
#include "Modules/PacketDropper.hh"
#include "InterfaceAPI.hh"
#include "config.hh"

//#include "EmulatedInterface.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    /** \defgroup senf_emu_emulation Emulated interfaces

        An emulated interface is an implementation of the Interface API which does not send or
        receive packets using real interface hardware but exchanges packets with other emulated
        interfaces in the same collision/broadcast domain.

        Emulated interfaces send and receive packets using a UDP Network socket on a collision
        domain specific multicast group.

        \see senf::emu::InterfaceAPIBase for information on how to implement new emulated interfaces

        \todo Should we add a source interface field to the PacketHeader so that self-loop
            suppression (and later things like drop calculations) do not depend on a correct
            EthernetPacket header?

        \ingroup senf_emu_interfaces
     */

    // Change this to UDPv6ClientSocketHandle to use IPv6
    typedef senf::UDPv4ClientSocketHandle UDPClientHandle;

}}

#include "EmulatedInterface.ih"

namespace senf {
namespace emu {

    class EmulatedReceiver;
    class EmulatedTransmitter;

    //-////////////////////////////////////////////////////////////////////////
    // Interface

    /** \brief Emulated interface base-class

        This is the base-class of all emulated interfaces. Don't derive directly from this class,
        derive from EmulatedWirelessInterface or EmulatedWiredInterface.

        \ingroup senf_emu_emulation
     */
    class EmulatedInterface
        : public detail::InterfaceAccess<Interface>,
          public detail::CommfaceAccess<EmulatedReceiver, EmulatedTransmitter>
    {
    public:
        using detail::CommfaceAccess<EmulatedReceiver, EmulatedTransmitter>::receiver;
        using detail::CommfaceAccess<EmulatedReceiver, EmulatedTransmitter>::transmitter;

        virtual ~EmulatedInterface();

        UDPClientHandle emulationSocket() const; ///< Get the emulation socket handle
                                        /**< This is the socket on which all emulated traffic
                                             is sent or received. */

        UDPClientHandle::Address emulationAddress() const;
                                        ///< Get current emulated traffic multicast address

        void emulationInterface(std::string const & interface);
                                        ///< Set interface on which to send/receive emulated traffic
        std::string const & emulationInterface() const;
                                        ///< Get interface on which emulated traffic is sent/received

    protected:
        EmulatedInterface();
        void init();

        void emulationAddress(UDPClientHandle::Address const & address);
                                        ///< Change the emulated traffic multicast address
                                        /**< This member allows to change the multicast address
                                             on which emulated network traffic is sent. Depending on
                                             the type of emulated interface, it should not be called
                                             directly but may be controlled by the interface
                                             implementation. */
        void enableEmulatedInterface(); ///< Enable the interface
                                        /**< Must be called be the interface implementations \c
                                             v_enable() implementation. */
        void disableEmulatedInterface(); ///< Disable the interface
                                        /**< Must be called by the interface implementations \c
                                             v_disable() implementation. */
        bool emulatedInterfaceEnabled() const; ///< Get current interface state
                                        /**< This should be used to implement the interface
                                             implementations \c v_enabled() member. */

#ifndef DOXYGEN
    private:
#else
    protected:
#endif
        virtual void v_initEmulationInterface() = 0; ///< Called to initialize the emulated interface
        virtual void v_deinitEmulationInterface() = 0; ///< Called to shut down the emulated interface

    private:
        void openSocket();
        void closeSocket();

        UDPClientHandle::Address address_;
        UDPClientHandle socket_;
        bool enabled_;
        std::string interface_;
    };


    /** \brief Emulated interface base-class for receive capable interfaces
        \ingroup senf_emu_emulation
     */
    class EmulatedReceiver
        : public detail::InterfaceAccess<EmulatedInterface>,
          public detail::InterfaceAccess<Interface>
    {
        ppi::module::ActiveSocketSource<ppi::DgramReader<PacketHeader,SENF_EMU_MAXMTU> > source_;
        detail::EmulatedInterfaceReceiveFilter filter_;
        detail::DroppedPacketChecker dropChecker_;
        detail::DelayTimeChecker delayChecker_;
        emu::PacketDropper dropper_;
        detail::AdvancePacket advance_;

    public:
        void startStatistics(ClockService::clock_type pollInterval);
                                        ///< Start statistics generation or change interval

        void maxDelay(ClockService::clock_type maxDelay);
                                        ///< Set maximum allowed packet delay
        ClockService::clock_type maxDelay() const;
                                        ///< Get current maxDelay() value

        unsigned delayedPackets() const; ///< Number of packets with delay > maxDelay()
        ClockService::clock_type maxPacketDelay() const;
                                        ///< Maximum packet delay encountered
        unsigned droppedPackets() const; ///< Number of dropped packets

        double lossRate() const;
        void lossRate(double lossRate);

    protected:
        ppi::connector::ActiveOutputJack<> receiverJack;

        EmulatedReceiver();
        void init();

        void initEmulatedReceiver();    ///< Initialize receiver
                                        /**< Must be called by the interface implementations \c
                                             v_initEmulationInterface() member. */
        void deinitEmulatedReceiver();  ///< Shut down receiver
                                        /**< Must be called by the interface implementations \c
                                             v_deinitEmulationInterface() member. */
        bool promiscEmulatedReceiver() const; ///< Get receiver promiscuous state
                                        /**< Must be called by the interface implementations \c
                                             v_promisc() member. */
        void promiscEmulatedReceiver(bool v); ///< Set receiver promiscuous state
                                        /**< Must be called by the interface implementations \c
                                             v_promisc(bool) member. */
        void mcAddEmulatedReceiver(MACAddress const & address);///< Add multicast MAC address
                                        /**< Must be called by the interface implementations \c
                                             v_mcAdd(senf::MACAddress) member. */
        void mcDropEmulatedReceiver(MACAddress const & address);///< Drops multicast MAC address
                                        /**< Must be called by the interface implementations \c
                                             v_mcDrop(senf::MACAddress) member. */

    private:
        typedef detail::InterfaceAccess<EmulatedInterface> emui;
        typedef detail::InterfaceAccess<Interface> basei;

        bool promisc_;
        std::set<MACAddress> mcSet_;

        friend class detail::EmulatedInterfaceReceiveFilter;
    };


    /** \brief Emulated interface base-class for transmit capable interfaces
        \ingroup senf_emu_emulation
     */
    class EmulatedTransmitter
        : public detail::InterfaceAccess<EmulatedInterface>,
          public detail::InterfaceAccess<Interface>
    {
    public:
        void delay(ClockService::clock_type delay); ///<set the delay of the sender
        ClockService::clock_type delay() const; ///<get the sender delay

    protected:
        ppi::connector::PassiveInputJack<> transmitterJack;

        EmulatedTransmitter();
        void init();

        void initEmulatedTransmitter(); ///< Initialize transmitter
                                        /**< Must be called by the interface implementations \c
                                             v_initEmulationInterface() member. */
        void deinitEmulatedTransmitter(); ///< Shut down transmitter
                                        /**< Must be called by the interface implementations \c
                                             v_deinitEmulationInterface() member. */

        unsigned mtuEmulatedTransmitter() const; ///< Get emulated MTU
                                        /**< Must be called by the interface implementations \c
                                             v_mtu() member. */
        void mtuEmulatedTransmitter(unsigned v); ///< Set emulated MTU
                                        /**< Must be called by the interface implementations
                                             <tt>v_mtu(unsigned)</tt> member. */

        virtual Packet::size_type v_emulatedPayloadSize(Packet packet); ///< Called to get size of payload
                                        /**< This member is called to find out the size of the
                                             payload of an emulated packet without any emulation
                                             specific headers. The default implementation will search
                                             for the first EthernetHeader found and use the size of the
                                             next packet thus found. */

    private:
        typedef detail::InterfaceAccess<EmulatedInterface> emui;
        typedef detail::InterfaceAccess<Interface> basei;

        detail::EmulatedInterfaceTransmitFilter filter_;
        PacketDelayer delayer_;
        ppi::module::PassiveSocketSink<ppi::TargetDgramWriter<UDPClientHandle> > sink_;

        friend class detail::EmulatedInterfaceTransmitFilter;
    };


    class EmulatedWiredReceiver;
    class EmulatedWiredTransmitter;

    /** \brief Emulated interface base-class for wired interfaces
        \ingroup senf_emu_emulation
     */
    class EmulatedWiredInterface
        : public EmulatedInterface,
          public detail::InterfaceAccess<WiredInterface>,
          public detail::CommfaceAccess<EmulatedWiredReceiver, EmulatedWiredTransmitter>
    {
    public:
        using detail::InterfaceAccess<WiredInterface>::interface;
        using detail::CommfaceAccess<EmulatedWiredReceiver, EmulatedWiredTransmitter>::receiver;
        using detail::CommfaceAccess<EmulatedWiredReceiver, EmulatedWiredTransmitter>::transmitter;

        using EmulatedInterface::emulationAddress;

    protected:
        void init();
    };


    /** \brief Emulated interface base-class for receive capable wired interfaces
        \ingroup senf_emu_emulation
     */
    class EmulatedWiredReceiver
        : public EmulatedReceiver,
          public detail::InterfaceAccess<WiredInterface>
    {
    protected:
        void init();

    private:
        using detail::InterfaceAccess<EmulatedInterface>::interface;
    };

    /** \brief Emulated interface base-class for transmit capable wired interfaces
        \ingroup senf_emu_emulation
     */
    class EmulatedWiredTransmitter
        : public EmulatedTransmitter,
          public detail::InterfaceAccess<WiredInterface>
    {
    protected:
        void init();

    private:
        using detail::InterfaceAccess<EmulatedInterface>::interface;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "EmulatedInterface.cci"
//#include "EmulatedInterface.ct"
//#include "EmulatedInterface.cti"
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
