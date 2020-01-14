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
    \brief InterfaceAPIBase public header */

#ifndef HH_SENF_Ext_NetEmu_InterfaceAPIBase_
#define HH_SENF_Ext_NetEmu_InterfaceAPIBase_ 1

// Custom includes
#include "WirelessInterfaceAPI.hh"
#include "EmulatedInterface.hh"
#include "EmulatedWirelessInterface.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {
namespace interface {

    // Transport
    struct Hardware {};
    struct Emulated {};

    // Medium
    struct Wired {};
    struct Wireless {};

    // Direction
    struct Bidirectional {};
    struct ReceiveOnly {};
    struct TransmitOnly {};

}

    /** \brief Interface implementation base-class

        InterfaceAPIBase is the helper base-class for implementing interface types. Depending on the
        template arguments, the class will select the correct set of base classes.

        \tparam Transport Transport type, interface::Hardware or interface::Emulated
        \tparam Medium Type of network medium, interface::Wired or interface::Wireless
        \tparam Direction Supported data directions, interface::Bidirectional (default),
            interface::ReceiveOnly or Interface::TransmitOnly

        The real interface implementation will be some PPI network which produces received packets
        on an output connector or jack and will receive packets on an input connector to be sent
        via the network device.

        For emulated interfaces, the 'hardware device' is provided by the base classes via two
        jacks, \a receiverJack for incoming packets and \a transmitterJack for outgoing packets. You
        will need to connect these jacks to your internal network.

        \code
        struct MyInterfaceNet
        {
            ppi::connector::ActiveOutputJack<EthernetPacket> myOutput; // receive capable
            ppi::connector::PassiveInputJack<EthernetPacket> myInput;  // transmit capable

            // Emulated interfaces only
            ppi::connector::PassiveInputJack<> myReceiveInput;         // receive capable
            ppi::connector::ActiveOutputJack<> myTransmitOutput;       // transmit capable

            // modules and code to send/receive data to/from the network device (or the emulation
            // layer)
        };

        // Adjust the InterfaceAPIBase template arguments accordingly

        class MyInterface
            : private MyInterfaceNet,
              public senf::emu::InterfaceAPIBase<senf::emu::interface::Emulated,
                                                 senf::emu::interface::Wireless>
        {
            typedef senf::emu::InterfaceAPIBase<senf::emu::interface::Hardware,
                                                senf::emu::interface::Wireless> Base;
        public:

            // The arguments to the Base constructor depends on the 'Direction' template
            // parameter:
            //     ReceiveOnly interfaces skip the 'input' argument
            //     TransmitOnly interfaces skip the 'output' argument
            // Your constructor may take arguments to configure the device.

            MyInterface()
                : Base (myOutput, myInput)
            {
                // Only emulated interfaces
                senf::ppi::connect(receiverJack, myReceiveInput);            // receive capable
                senf::ppi::connect(myTransmitOutput, transmitterJack);       // transmit capable

                // Only hardware wireless interfaces
                // call registerFrequency and registerParameter (once or multiple times) to
                // register all valid parameter and frequency combinations.
                registerFrequency(minFreq, maxFreq, minBw, maxBw);
                registerFrequency(freq, bw);
                registerParameter(label, efficiency, minrssi);

                // Only hardware wireless transmit capable interfaces
                // call register TxPower (once or multiple times) to register valid
                // txPower values
                registerTxPower(minPower, maxPower);
                registerTxPower(power);

                init();
            }

        private:
            // Abstract API implementation

            // All interfaces
            virtual void v_id(senf::MACAddress const & id);
            virtual senf::MACAddress v_id() const;

            // Only hardware interfaces
            virtual void v_enable();
            virtual void v_disable();
            virtual bool v_enabled() const;

            // Only hardware transmit capable interfaces
            virtual unsigned v_mtu() const;
            virtual void v_mtu(unsigned v);

            // Only hardware receive capable interfaces
            virtual bool v_promisc() const;
            virtual void v_promisc(bool v);
            virtual bool v_annotationMode() const;
            virtual void v_annotationMode(bool a);

            // Only hardware wireless interfaces
            virtual unsigned v_frequency() const;
            virtual unsigned v_bandwidth() const;
            virtual void v_frequency(unsigned freq, unsigned bw);

            // Only wireless transmit capable interfaces
            virtual unsigned v_modulationId() const;
            virtual void v_modulationId(unsigned id);
            virtual int v_txPower() const;
            virtual void v_txPower(int power);

            // Only emulated transmit capable interfaces (optional)
            virtual Packet::size_type v_emulatedPayloadSize(Packet packet);

            // Only emulated wireless receive capable interfaces (optional)
            virtual bool v_emulatedWirelessQuality(senf::Packet packet,
                                                   senf::emu::annotations::Quality & quality);
        };
        \endcode
     */
    template <class Medium, class Direction, class Transport=void, class Base=void>
    class InterfaceAPIBase
    {};

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // senf::emu::InterfaceAPIBase< Wired,Bidirectional,void >

    template <>
    class InterfaceAPIBase<interface::Wired, interface::Bidirectional>
        : public WiredInterface, public WiredReceiver, public WiredTransmitter
    {
    public:
        template <class Output, class Input>
        InterfaceAPIBase(Output & output, Input & input);

    protected:
        void init();
    };

    typedef InterfaceAPIBase<interface::Wired, interface::Bidirectional> BidirectionalWiredInterface;

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // senf::emu::InterfaceAPIBase< Wired,Bidirectional,Hardware >

    template <>
    class InterfaceAPIBase<interface::Wired, interface::Bidirectional, interface::Hardware>
        : public BidirectionalWiredInterface,
          public HardwareInterface
    {
    public:
        template <class Output, class Input>
        InterfaceAPIBase(Output & output, Input & input);

    protected:
        void init();
    };

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // senf::emu::InterfaceAPIBase< Wired,ReceiveOnly,Hardware >

    template <>
    class InterfaceAPIBase<interface::Wired, interface::ReceiveOnly, interface::Hardware>
        : public WiredInterface, public WiredReceiver,
          public HardwareInterface
    {
    public:
        template <class Output>
        explicit InterfaceAPIBase(Output & output);

    protected:
        void init();
    };

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // senf::emu::InterfaceAPIBase< Wired,TransmitOnly,Hardware >

    template <>
    class InterfaceAPIBase<interface::Wired, interface::TransmitOnly, interface::Hardware>
        : public WiredInterface, public WiredTransmitter,
          public HardwareInterface
    {
    public:
        template <class Input>
        explicit InterfaceAPIBase(Input & input);

    protected:
        void init();
    };

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // senf::emu::InterfaceAPIBase< Wireless,Bidirectional,void >

    template <>
    class InterfaceAPIBase<interface::Wireless, interface::Bidirectional>
        : public WirelessInterface, public WirelessReceiver, public WirelessTransmitter
    {
    public:
        template <class Output, class Input>
        InterfaceAPIBase(Output & output, Input & input);

    protected:
        void init();
    };

    typedef InterfaceAPIBase<interface::Wireless, interface::Bidirectional> BidirectionalWirelessInterface;

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // senf::emu::InterfaceAPIBase< Wireless,Bidirectional,Hardware >

    template <>
    class InterfaceAPIBase<interface::Wireless, interface::Bidirectional, interface::Hardware>
        : public BidirectionalWirelessInterface,
          public HardwareInterface
    {
    public:
        template <class Output, class Input>
        InterfaceAPIBase(Output & output, Input & input);

    protected:
        void init();
    };

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // senf::emu::InterfaceAPIBase< Wireless,ReceiveOnly,Hardware >

    template <>
    class InterfaceAPIBase<interface::Wireless, interface::ReceiveOnly, interface::Hardware>
        : public WirelessInterface, public WirelessReceiver,
          public HardwareInterface
    {
    public:
        template <class Output>
        explicit InterfaceAPIBase(Output & output);

    protected:
        void init();
    };

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // senf::emu::InterfaceAPIBase< Wireless,TransmitOnly,Hardware >

    template <>
    class InterfaceAPIBase<interface::Wireless, interface::TransmitOnly, interface::Hardware>
        : public WirelessInterface, public WirelessTransmitter,
          public HardwareInterface
    {
    public:
        template <class Input>
        explicit InterfaceAPIBase(Input & input);

    protected:
        void init();
    };

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // senf::emu::InterfaceAPIBase< Wired,Bidirectional,Emulated >

    template < class Base>
    class InterfaceAPIBase<interface::Wired, interface::Bidirectional, interface::Emulated, Base>
        : public Base,
          public EmulatedWiredInterface, public EmulatedWiredReceiver, public EmulatedWiredTransmitter
    {
    public:
        template <class Output, class Input>
        InterfaceAPIBase(Output & output, Input & input);

    protected:
        void init();

    private:
        void v_enable();
        void v_disable();
        bool v_enabled() const;
        void v_initEmulationInterface();
        void v_deinitEmulationInterface();
        bool v_promisc() const;
        void v_promisc(bool v);
        bool v_annotationMode() const;
        void v_annotationMode(bool a);
        void v_mcAdd(senf::MACAddress const & address);
        void v_mcDrop(senf::MACAddress const & address);
        unsigned v_mtu() const;
        void v_mtu(unsigned v);
    };

    template <>
    class InterfaceAPIBase<interface::Wired, interface::Bidirectional, interface::Emulated>
        : InterfaceAPIBase<interface::Wired, interface::Bidirectional, interface::Emulated, BidirectionalWiredInterface>
    { };

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // senf::emu::InterfaceAPIBase< Wired,ReceiveOnly,Emulated >

    template <>
    class InterfaceAPIBase<interface::Wired, interface::ReceiveOnly, interface::Emulated>
        : public WiredInterface, public WiredReceiver,
          public EmulatedWiredInterface, public EmulatedWiredReceiver
    {
    public:
        template <class Output>
        explicit InterfaceAPIBase(Output & output);

    protected:
        void init();

    private:
        void v_enable();
        void v_disable();
        bool v_enabled() const;
        void v_initEmulationInterface();
        void v_deinitEmulationInterface();
        bool v_promisc() const;
        void v_promisc(bool v);
        bool v_annotationMode() const;
        void v_annotationMode(bool a);
        void v_mcAdd(senf::MACAddress const & address);
        void v_mcDrop(senf::MACAddress const & address);
    };

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // senf::emu::InterfaceAPIBase< Wired,TransmitOnly,Emulated >

    template <>
    class InterfaceAPIBase<interface::Wired, interface::TransmitOnly, interface::Emulated>
        : public WiredInterface, public WiredTransmitter,
          public EmulatedWiredInterface, public EmulatedWiredTransmitter
    {
    public:
        template <class Input>
        explicit InterfaceAPIBase(Input & input);

    protected:
        void init();

    private:
        void v_enable();
        void v_disable();
        bool v_enabled() const;
        void v_initEmulationInterface();
        void v_deinitEmulationInterface();
        unsigned v_mtu() const;
        void v_mtu(unsigned v);
    };

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // senf::emu::InterfaceAPIBase< Wireless,Bidirectional,Emulated >

    template <class Base>
    class InterfaceAPIBase<interface::Wireless, interface::Bidirectional, interface::Emulated, Base>
        : public Base,
          public EmulatedWirelessInterface, public EmulatedWirelessReceiver, public EmulatedWirelessTransmitter
    {
    public:
        template <class Output, class Input>
        InterfaceAPIBase(Output & output, Input & input);

    protected:
        void init();
        // why the heck ??
        using EmulatedWirelessReceiver::receiverJack;
        using EmulatedWirelessTransmitter::transmitterJack;

    private:
        void v_enable();
        void v_disable();
        bool v_enabled() const;
        void v_initEmulationInterface();
        void v_deinitEmulationInterface();
        bool v_promisc() const;
        void v_promisc(bool v);
        bool v_annotationMode() const;
        void v_annotationMode(bool a);
        void v_mcAdd(senf::MACAddress const & address);
        void v_mcDrop(senf::MACAddress const & address);
        unsigned v_mtu() const;
        void v_mtu(unsigned v);
        unsigned v_frequency() const;
        unsigned v_bandwidth() const;
        void v_frequency(unsigned frequency, unsigned bandwidth);
        void v_coverageRange(unsigned distance);
    };

    template <>
    class InterfaceAPIBase<interface::Wireless, interface::Bidirectional, interface::Emulated>
        : InterfaceAPIBase<interface::Wireless, interface::Bidirectional, interface::Emulated, BidirectionalWirelessInterface>
    { };

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // senf::emu::InterfaceAPIBase< Wireless,ReceiveOnly,Emulated >

    template <class Base>
    class InterfaceAPIBase<interface::Wireless, interface::ReceiveOnly, interface::Emulated, Base>
        : public Base, public WirelessReceiver,
          public EmulatedWirelessInterface, public EmulatedWirelessReceiver
    {
    public:
        template <class Output>
        explicit InterfaceAPIBase(Output & output);

    protected:
        void init();
        // why the heck ??
        using EmulatedWirelessReceiver::receiverJack;

    private:
        void v_enable();
        void v_disable();
        bool v_enabled() const;
        void v_initEmulationInterface();
        void v_deinitEmulationInterface();
        bool v_promisc() const;
        void v_promisc(bool v);
        bool v_annotationMode() const;
        void v_annotationMode(bool a);
        void v_mcAdd(senf::MACAddress const & address);
        void v_mcDrop(senf::MACAddress const & address);
        unsigned v_frequency() const;
        unsigned v_bandwidth() const;
        void v_frequency(unsigned frequency, unsigned bandwidth);
        void v_coverageRange(unsigned distance);
    };

    template <>
    class InterfaceAPIBase<interface::Wireless, interface::ReceiveOnly, interface::Emulated>
        : InterfaceAPIBase<interface::Wireless, interface::ReceiveOnly, interface::Emulated, WirelessInterface>
    { };

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // senf::emu::InterfaceAPIBase< Wireless,TransmitOnly,Emulated >

    template <class Base>
    class InterfaceAPIBase<interface::Wireless, interface::TransmitOnly, interface::Emulated, Base>
        : public Base, public WirelessTransmitter,
          public EmulatedWirelessInterface, public EmulatedWirelessTransmitter
    {
    public:
        template <class Input>
        explicit InterfaceAPIBase(Input & input);

    protected:
        void init();
        // why the heck ??
        using EmulatedWirelessTransmitter::transmitterJack;

    private:
        void v_enable();
        void v_disable();
        bool v_enabled() const;
        void v_initEmulationInterface();
        void v_deinitEmulationInterface();
        unsigned v_mtu() const;
        void v_mtu(unsigned v);
        unsigned v_frequency() const;
        unsigned v_bandwidth() const;
        void v_frequency(unsigned frequency, unsigned bandwidth);
        void v_coverageRange(unsigned distance);
    };

    template <>
    class InterfaceAPIBase<interface::Wireless, interface::TransmitOnly, interface::Emulated>
        : InterfaceAPIBase<interface::Wireless, interface::TransmitOnly, interface::Emulated, WirelessInterface>
    { };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "InterfaceAPIBase.cci"
//#include "InterfaceAPIBase.ct"
#include "InterfaceAPIBase.cti"
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
