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
    \brief InterfaceAPI public header */

#ifndef HH_SENF_Ext_NetEmu_InterfaceAPI_
#define HH_SENF_Ext_NetEmu_InterfaceAPI_ 1

// Custom includes
#include <boost/noncopyable.hpp>
#include <senf/PPI/Jack.hh>
#include <senf/Utils/Statistics.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include "InterfaceDecorator.hh"
#include "InterfaceId.hh"

#include "InterfaceAPI.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    /** \defgroup senf_emu_interfaces NetEmu interface API

        The NetEmu library includes a flexible group of classes to model network interfaces,
        emulated and hardware (non-emulated).

        \dot
        // nomunge
        digraph interfaceapi {
          rankdir=RL; ranksep=.6; nodesep=.1;
          node [shape=box,fontname=Helvetica,fontsize=8,height=.22,width=1.7];

          subgraph cluster_receivers {
            pencolor=gray80;
            WirelessReceiver -> Receiver;
            WiredReceiver -> Receiver;
            EmulatedWirelessReceiver -> EmulatedReceiver;
            EmulatedWiredReceiver -> EmulatedReceiver;
          }

          subgraph cluster_interfaces {
            pencolor=gray80;
            WirelessInterface -> Interface;
            WiredInterface -> Interface;
            EmulatedWirelessInterface -> EmulatedInterface;
            EmulatedWiredInterface -> EmulatedInterface;
            HardwareInterface;
          }

          subgraph cluster_transmitters {
            pencolor=gray80;
            WirelessTransmitter -> Transmitter;
            WiredTransmitter -> Transmitter;
            EmulatedWirelessTransmitter -> EmulatedTransmitter;
            EmulatedWiredTransmitter -> EmulatedTransmitter;
          }

          {
            edge [headport="e", tailport="w", color="blue", minlen=2];

            EmulatedWLANInterface [fontcolor="blue"];
            EmulatedWLANInterface -> WirelessInterface;
            EmulatedWLANInterface -> WirelessReceiver;
            EmulatedWLANInterface -> WirelessTransmitter;
            EmulatedWLANInterface -> EmulatedWirelessInterface;
            EmulatedWLANInterface -> EmulatedWirelessReceiver;
            EmulatedWLANInterface -> EmulatedWirelessTransmitter;
          }

          {
            edge [headport="e", tailport="w", color="brown", minlen=2];

            HardwareWLANInterface [fontcolor="brown"];
            HardwareWLANInterface -> WirelessInterface;
            HardwareWLANInterface -> WirelessReceiver;
            HardwareWLANInterface -> WirelessTransmitter;
            HardwareWLANInterface -> HardwareInterface;
          }
        }
        \enddot

        The Interfaces are grouped around a series of base classes providing a generic interface
        API.
        \li Every interface provides the senf::emu::Interface API
        \li If the interface can receive packets, it will provide the senf::emu::Receiver API
        \li If the interface can send packets, it will provide the senf::emu::Transmitter API

        The interfaces will not directly derive from these classes. Instead they will derive from
        either WirelessInterface, WirelessTransmitter and WirelessReceiver or WiredInterface,
        WiredReceiver and WiredTransmitter.

        For emulated interfaces, there is an additional set of interfaces: EmulatedInterface,
        EmulatedReceiver and EmulatedTransmitter. Again, implementations don't directly derive from
        these classes but from either EmulatedWirelessInterface, EmulatedWirelessReceiver and/or
        EmulatedWirelessTransmitter or from EmulatedWiredInterface, EmulatedWiredReceiver and/or
        EmulatedWiredTransmitter.

        \see senf::emu::InterfaceAPIBase for more details on how to implement a new interface type.

        \section senf_emu_interfaces_decoration Interface decoration

        Applications utilizing the NetEmu interface classes will often need additional application
        specific modules to be added to each interface. To simplify this task (especially when using
        a larger number of dynamically configured interfaces), the concept of interface decoration
        is introduced.

        A decorated interface is an arbitrary interface which has been extended with additional
        modules in it's input and/or output chain. These modules are presented to NetEmu in the form
        of two PPI groups (or single modules): One group or module for the input, one for the
        output.

        The interface decoration is applied in two steps: First the type of decoration is defined:
        \code
        struct MyInputGroup
        {
            ppi::connector::PassiveInputJack<EthernetPacket> input;
            ppi::connector::ActiveOutputJack<EthernetPacket> output;

            // ...
        };

        struct MyOutputGroup
        {
            ppi::connector::PassiveInputJack<EthernetPacket> input;
            ppi::connector::ActiveOutputJack<EthernetPacket> output;

            // ...
        };

        typedef senf::emu::ApplyInterfaceDecorator<MyInputGroup, MyOutputGroup> MyDecoration;
        \endcode

        This decoration can then be applied to any type of interface
        \code
        typedef MyDecoration::apply<senf::emu::EmulatedWLANInterface>::type MyWLANInterface;
        \endcode

        When applying a decoration to an interface, receive only interface types will ignore the
        output group while transmit only interfaces will ignore the input group.

        All decorated interfaces implement the generic decorated interface API:
        \li senf::emu::InterfaceDecorator is implemented by every decorated interface
        \li senf::emu::ReceiverDecorator is implemented by receive enabled interfaces
        \li senf::emu::TransmitterDecorator is implemented by transmit enabled interfaces.

        In addition to the generic InterfaceDecorator API, there is a derived decorator base-class
        for each type of interface. This type is accessible as the \c Decorator member of the
        corresponding interface class:
        \li senf::emu::Interface::Decorator generic decorator API
        \li senf::emu::WirelessInterface::Decorator decorator API for wireless interfaces
        \li senf::emu::EmulatedWLANInterface::Decorator decorator API for EmulatedWLANInterface's

        The Receiver/Transmitter and EmulatedInterface classes to \e not have \c Decorator members
        to not make the inheritance hierarchy to complex. The only difference between those
        different Decorator classes is the return type of the \c interface() member which returns
        the wrapped interface reference.

        When defining a decoration, both the input and output module/group are optional. Thus every
        interface can always be decorated and the Interface::Decorator type is a very good candidate
        for a generic interface type (since it allows decorated or un- (null-) decorated interfaces
        to be referenced). Depending on the application requirements, use the correct base-class
        (e.g. WirelessInterface::Decorator to manage arbitrary decorated wireless interfaces).
     */

    /** \defgroup senf_emu_interface_implementations NetEmu interface implementations
        \ingroup senf_emu_interfaces
     */

    class Receiver;
    class Transmitter;

    /** \brief %Interface API base class

        All NetEmu Interfaces derive from this class. This class provides the basic generic
        %interface API.

        \see \ref senf_emu_interfaces

        \ingroup senf_emu_interfaces
     */
    class Interface
        : public detail::CommfaceAccess<Receiver,Transmitter>,
          private boost::noncopyable
    {
    public:
        typedef InterfaceDecorator Decorator; ///< %Interface base class specific decorator type

        using detail::CommfaceAccess<Receiver,Transmitter>::receiver;
        using detail::CommfaceAccess<Receiver,Transmitter>::transmitter;

        virtual ~Interface();

        bool canTransmit() const;        ///< \c true, if %interface is transmit capable
        bool canReceive() const;         ///< \c true, if %interface is receive capable

        void enable();                   ///< Enable %interface (aka <tt>ifconfig up</tt>)
        void disable();                  ///< Disable %interface (aka <tt>ifconfig down</tt>)
        bool enabled() const;            ///< \c true, if %interface enabled

        unsigned index() const;          ///< Get %interface index

        void id(MACAddress const & eui); ///< Change %interface MAC
        MACAddress id() const;           ///< Get %interface MAC Address

        static unsigned nodeId();        ///< Get unique node id
                                         /**< The nodeId defaults to the process id */
        static void nodeId(unsigned id); ///< Set unique node id

        void deviceId(InterfaceDeviceId id);
        InterfaceDeviceId deviceId() const;

        boost::uint8_t linkTypeId() const;
        std::string interfaceTypeName() const;

        console::DirectoryNode & consoleDir() const; ///< Access %interface console directory

        ///\name Statistic Related Member Functions
        //\{
        console::DirectoryNode & statisticsDir() const;
                                        ///< Access the %interface statistics directory

        template <class Stat>
        senf::Statistics & registerStatistics(std::string name, Stat & stat);
                                        ///< Register statistics source
                                        /**< Associates \a stat with a statistics collector and adds
                                             it to the statistics console directory of the
                                             %interface. \a stat can be an arbitrary statistics
                                             signal.
                                             \returns reference to the new created Statistics object
                                             \warning Be careful when saving the returned reference,
                                             since it is coupled to the lifetime of the %interface. */

        boost::optional<senf::Statistics &> statistic(std::string const & name);
                                        ///< Get registered statistic
                                        /**< \returns reference to registered statistic
                                             \see registerStatistics() */

#ifndef DOXYGEN
        boost::signals2::signal<void (ClockService::clock_type)> startStatistics;
#else
        void startStatistics(ClockService::clock_type inverval);
                                        ///< Start statistics generation
                                        /**< Starts the statistics generation timer in all
                                             statistics sources registered with this %interface
                                             (via registerStatistics()).

                                             \implementation This is not really a member function,
                                                 it's a boost signal which can be called like a
                                                 function.
                                             \see senf_emu_interface_statistics */
#endif
        //\}

        ///\name Signals
        //\{
        boost::signals2::signal<void (Interface &, MACAddress)> idChangedSignal;
        //\}

    protected:
        Interface();

        void init();                    ///< Initialize %interface
                                        /**< This member will be called by every derived class */

#ifndef DOXYGEN
    private:
#else
    protected:
#endif
        virtual void v_enable() = 0;    ///< Called to enable %interface
        virtual void v_disable() = 0;   ///< Called to disable %interface
        virtual bool v_enabled() const = 0; ///< Return \c true if %interface is enabled
        virtual void v_id(MACAddress const & id) = 0; ///< Called to set %interface mac address
        virtual MACAddress v_id() const = 0; ///< Return the %interface mac address
        virtual boost::uint8_t v_linkTypeId() const = 0;

    private:
        typedef boost::ptr_map<std::string, senf::Statistics> Statistics;
        Statistics statistics_;
        console::ScopedDirectory<> consoleDir_;
        unsigned index_;
        mutable MACAddress id_; // id cache (therefore mutable)
        static unsigned nodeId_;
        InterfaceDeviceId deviceId_;
    };


    /** \brief Interface API base class for receive capable interfaces
        \see \ref senf_emu_interfaces
        \ingroup senf_emu_interfaces
     */
    class Receiver
        : public detail::InterfaceAccess<Interface>
    {
    public:
        virtual ~Receiver();

        ppi::connector::ActiveOutputJack<EthernetPacket> output;

        bool annotationMode() const;    ///< Get interface annotationMode status
        void annotationMode(bool a);    ///< Enable/Disable AnnotationPackets for each frame
        bool promisc() const;           ///< Get interface promiscuous status
        void promisc(bool v);           ///< Change interface promiscuous status
        void mcAdd(senf::MACAddress const & address);
        void mcDrop(senf::MACAddress const & address);
        void flushRxQueues();           ///< Flush Rx Queues
        
#ifndef DOXYGEN
    protected:
#else
    private:
#endif
        Receiver(ppi::connector::ActiveOutput<> & output);
        Receiver(ppi::connector::ActiveOutput<EthernetPacket> & output);
        Receiver(ppi::connector::ActiveOutputJack<> & output);
        Receiver(ppi::connector::ActiveOutputJack<EthernetPacket> & output);

        void init();

        virtual bool v_annotationMode() const = 0;
        virtual void v_annotationMode(bool a) = 0;
        virtual bool v_promisc() const = 0;
        virtual void v_promisc(bool v) = 0;

        virtual void v_mcAdd(senf::MACAddress const & address) = 0;
        virtual void v_mcDrop(senf::MACAddress const & address) = 0;

        virtual void v_flushRxQueues() {};   
    };

    /** \brief Interface API base class for transmit capable interfaces
        \see \ref senf_emu_interfaces
        \ingroup senf_emu_interfaces
     */
    class Transmitter
        : public detail::InterfaceAccess<Interface>
    {
    public:
        virtual ~Transmitter();

        ppi::connector::PassiveInputJack<EthernetPacket> input;

        unsigned mtu() const;           ///< Get interface MTU
        void mtu(unsigned v);           ///< Set interface MTU
        void flushTxQueues();           ///< Flush Tx Queues

#ifndef DOXYGEN
    protected:
#else
    private:
#endif
        Transmitter(ppi::connector::PassiveInput<> & input);
        Transmitter(ppi::connector::PassiveInput<EthernetPacket> & input);
        Transmitter(ppi::connector::PassiveInputJack<> & input);
        Transmitter(ppi::connector::PassiveInputJack<EthernetPacket> & input);

        void init();

        virtual unsigned v_mtu() const = 0; ///< Called to get interface MTU
        virtual void v_mtu(unsigned v) = 0; ///< Called to set interface MTU
        virtual void v_flushTxQueues() {} ;
    };

    //-//////////////////////////////////////////////////////////////////////

    class WiredReceiver;
    class WiredTransmitter;

    /** \brief Wired interface base class
        \see \ref senf_emu_interfaces
        \ingroup senf_emu_interfaces
     */
    class WiredInterface
        : public Interface,
          public detail::CommfaceAccess<WiredReceiver,WiredTransmitter>
    {
    public:
        typedef TypedInterfaceDecorator<WiredInterface, Interface::Decorator> Decorator;

        using detail::CommfaceAccess<WiredReceiver,WiredTransmitter>::receiver;
        using detail::CommfaceAccess<WiredReceiver,WiredTransmitter>::transmitter;

    protected:
        void init();
    };


    /** \brief Wired receiver base class
        \see \ref senf_emu_interfaces
        \ingroup senf_emu_interfaces
     */
    class WiredReceiver
        : public Receiver,
          public detail::InterfaceAccess<WiredInterface>
    {
    protected:
        WiredReceiver(ppi::connector::ActiveOutput<> & output);
        WiredReceiver(ppi::connector::ActiveOutput<EthernetPacket> & output);
        WiredReceiver(ppi::connector::ActiveOutputJack<> & output);
        WiredReceiver(ppi::connector::ActiveOutputJack<EthernetPacket> & output);

        void init();

    private:
        using detail::InterfaceAccess<WiredInterface>::interface;
    };


    /** \brief Wired transmitter base class
        \see \ref senf_emu_interfaces
        \ingroup senf_emu_interfaces
     */
    class WiredTransmitter
        : public Transmitter,
          public detail::InterfaceAccess<WiredInterface>
    {
    protected:
        WiredTransmitter(ppi::connector::PassiveInput<> & input);
        WiredTransmitter(ppi::connector::PassiveInput<EthernetPacket> & input);
        WiredTransmitter(ppi::connector::PassiveInputJack<> & input);
        WiredTransmitter(ppi::connector::PassiveInputJack<EthernetPacket> & input);

        void init();

    private:
        using detail::InterfaceAccess<WiredInterface>::interface;
    };

    //-//////////////////////////////////////////////////////////////////////

    /** \brief Hardware interface base class

        This base class must be inherited by all real, non-emulated interface classes.

        \see \ref senf_emu_interfaces
        \ingroup senf_emu_interfaces
     */
    class HardwareInterface
        : public detail::InterfaceAccess<Interface>
    {
    protected:
        void init();
    };

    //-//////////////////////////////////////////////////////////////////////

    console::DirectoryNode & interfaceDir();

    boost::uint16_t perturb(boost::uint16_t value);

    void macIdSeed(boost::uint16_t seed); ///< Re-seed the MACAddress address generator
                                        /**< To support interface implementations, the uniqueMAC()
                                             function can be used to get a pseudo-random mac
                                             address. The Addresses are pseudo random. The default
                                             seed is calculated from the process id and the current
                                             time, the seed may however be changed explicitly. */
    MACAddress uniqueMAC(boost::uint32_t magic);
                                        ///< Get pseudo-random but unique MACAddress
                                        /**< This function generates random MACAddress. The first four
                                             bytes will be taken from \a magic while the last four
                                             bytes are generated randomly. */

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "InterfaceAPI.cci"
#include "InterfaceAPI.ct"
#include "InterfaceAPI.cti"
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
