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
    \brief InterfaceDecorator public header */

#ifndef HH_SENF_Ext_NetEmu_InterfaceDecorator_
#define HH_SENF_Ext_NetEmu_InterfaceDecorator_ 1

// Custom includes
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/utility/base_from_member.hpp>
#include <senf/PPI/Jack.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Utils/mpl.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    class Interface;
    class Receiver;
    class Transmitter;

    /** \brief Decorator for Receivers

        \see senf::emu::ApplyInterfaceDecorator
     */
    class ReceiverDecorator
    {
    public:
        virtual ~ReceiverDecorator();

        ppi::connector::ActiveOutputJack<EthernetPacket> output;

    protected:
        template <class Connector>
        ReceiverDecorator(Connector & connector);
    };

    /** \brief Decorator for Transmitters

        \see senf::emu::ApplyInterfaceDecorator
     */
    class TransmitterDecorator
    {
    public:
        virtual ~TransmitterDecorator();

        ppi::connector::PassiveInputJack<EthernetPacket> input;

    protected:
        template <class Connector>
        TransmitterDecorator(Connector & connector);
    };

    /** \brief Generic Interface decorator base-class

        This class provides the generic API for a decorated interface.

        \see senf::emu::ApplyInterfaceDecorator
     */
    class InterfaceDecorator
        : private boost::noncopyable
    {
    public:
        virtual ~InterfaceDecorator();

        Interface & interface();
        Interface const & interface() const;

        Interface * operator->();
        Interface const * operator->() const;

        ReceiverDecorator & receiverDecorator();
        ReceiverDecorator const & receiverDecorator() const;
        TransmitterDecorator & transmitterDecorator();
        TransmitterDecorator const & transmitterDecorator() const;

        bool canReceive() const;
        bool canTransmit() const;
        MACAddress id() const;

    protected:
        InterfaceDecorator(Interface & interface);

    private:
        Interface & interface_;
    };

#ifndef DOXYGEN

    template <class InterfaceType, class Base=InterfaceDecorator>
    class TypedInterfaceDecorator
        : public Base
    {
    public:
        InterfaceType & interface();
        InterfaceType const & interface() const;

    protected:
        TypedInterfaceDecorator(InterfaceType & interface);
   };

#else

    /** \brief Specialized Interface decorator base-class

        This class specializes the interface type and provides a correctly type interface accessor.

        \see senf::emu::ApplyInterfaceDecorator

        \implementation This class is for exposition only. The real implementation inherits from \a
            Base and not InterfaceDecorator. \a Base must be either another TypedInterfaceDecorator
            or InterfaceDecorator.
     */
    template <class InterfaceType, class Base>
    class TypedInterfaceDecorator
        : public InterfaceDecorator
    {
    public:
        InterfaceType & interface();
        InterfaceType const & interface() const;

    protected:
        TypedInterfaceDecorator(InterfaceType & interface);
   };

#endif

    /** \brief Implementing the receiver decorator

        This class will connects the \a ReceiveModule (which may be a module or a module group) to
        the interface.

        If the \a ReceiveModule is senf::mpl::nil, no module/group will be connected to the
        interface receiver.

        \see senf::emu::ApplyInterfaceDecorator
     */
    template <class ReceiveModule>
    class ReceiverDecoratorImplementation
        : private boost::base_from_member<ReceiveModule>,
          public ReceiverDecorator
    {
        typedef boost::base_from_member<ReceiveModule> ModuleMember;
    public:
        ReceiveModule & receiveModule();
        ReceiveModule const & receiveModule() const;

    protected:
        template <class Interface>
        ReceiverDecoratorImplementation(Interface & interface);
    };

#ifndef DOXYGEN

    template <>
    class ReceiverDecoratorImplementation<senf::mpl::nil>
        : public ReceiverDecorator
    {
    protected:
        template <class Interface>
        ReceiverDecoratorImplementation(Interface & interface);
    };

#endif

    /** \brief Implementing the transmitter decorator

        This class will connects the \a TransmitModule (which may be a module or a module group) to
        the interface.

        If the \a TransmitModule is senf::mpl::nil, no module/group will be connected to the
        interface transmitter.

        \see senf::emu::ApplyInterfaceDecorator
     */
    template <class TransmitModule>
    class TransmitterDecoratorImplementation
        : private boost::base_from_member<TransmitModule>,
          public TransmitterDecorator
    {
        typedef boost::base_from_member<TransmitModule> ModuleMember;
    public:
        TransmitModule & transmitModule();
        TransmitModule const & transmitModule() const;

    protected:
        template <class Interface>
        TransmitterDecoratorImplementation(Interface & interface);
    };

#ifndef DOXYGEN

    template <>
    class TransmitterDecoratorImplementation<senf::mpl::nil>
        : public TransmitterDecorator
    {
    protected:
        template <class Interface>
        TransmitterDecoratorImplementation(Interface & interface);
    };

#endif

namespace detail {

#ifndef DOXYGEN

    struct NoDecorator
    {
        template <class Interface>
        NoDecorator(Interface & interface);
    };

    template <class InterfaceType, class CheckBase, class Implementation>
    struct CheckDecoratorBase
        : public boost::mpl::if_<
            boost::is_base_of<CheckBase, InterfaceType>,
            Implementation,
            NoDecorator >
    {};

#endif

}

#ifndef DOXYGEN

    template <class InterfaceType, class ReceiveModule, class TransmitModule>
    class InterfaceDecoratorImplementation
        : private boost::base_from_member<InterfaceType>,
          public InterfaceType::Decorator,
          public detail::CheckDecoratorBase<
              InterfaceType,
              Receiver,
              ReceiverDecoratorImplementation<ReceiveModule>
              >::type,
          public detail::CheckDecoratorBase<
              InterfaceType,
              Transmitter,
              TransmitterDecoratorImplementation<TransmitModule>
              >::type
    {
        typedef boost::base_from_member<InterfaceType> InterfaceBase;
        typedef typename detail::CheckDecoratorBase<
            InterfaceType,
            Receiver,
            ReceiverDecoratorImplementation<ReceiveModule> >::type ReceiverDecoratorBase;
        typedef typename detail::CheckDecoratorBase<
            InterfaceType,
            Transmitter,
            TransmitterDecoratorImplementation<TransmitModule> >::type TransmitterDecoratorBase;

    public:
        InterfaceDecoratorImplementation();
        template <class A1> explicit InterfaceDecoratorImplementation(A1 const & a1);
        template <class A1, class A2> explicit InterfaceDecoratorImplementation(A1 const & a1, A2 const & a2);
    };

#else

    /** \brief Decorate InterfaceType with additional processing modules

        This class adds \a ReceiveModule and \a TransmitModule before/after the \a InterfaceType
        input/output.

        If \a InterrfaceType can receive data, the class will inherit from
        ReceiverDecoratorImplementation. If \a InterfaceType can transmit data, the class will
        inherit from TransmitterDecoratorImplementation. The decorator will therefore have the same
        input and output connectors as the \a InterfaceType.

        \see senf::emu::ApplyInterfaceDecorator
     */
    template <class InterfaceType, class ReceiveModule, class TransmitModule>
    class InterfaceDecoratorImplementation
        : public TypedInterfaceDecorator<InterfaceType>,
          public ReceiverDecoratorImplementation<ReceiveModule>,
          public TransmitterDecoratorImplementation<TransmitModule>
    {};

#endif

    /** \brief Apply additional modules to an interface

        Interface decoration allows to add arbitrary additional modules to the input or output chain
        of an interface. Decorated interfaces share a generic API and provide fine-grained access to
        the underlying interface API at every level of specialization (Interface,
        WirelessInterface, WiredInterface etc).

        However, a decorated interface is \e not an interface itself. Since the input and output
        decoration are each optional, it is possible to convert any interface into the decorated
        interface API by a null-wrapping. The decorated interface can thus be used as a generic,
        extensible and wrappable interface API.

        Adding decoration to an interface is performed in two steps: First the decoration is defined
        as a typedef, then this decoration is applied to a specific type of interface:

        \code
        // Define the decoration typedef
        typedef senf::emu::ApplyInterfaceDecorator<MyReceiveGroup, MyTransmitGroup> MyDecoration;

        // Apply the typedef to a specific interface type
        typedef MyDecoration::apply<senf::emu::EmulatedWLANInterface>::type
            MyDecoratedWLANInterface;
        \endcode

        This way, the type of decoration can easily be declared in a generic way and be applied to
        any number of different interface types.

        If \a InterfaceType is receive-only, only the \a ReceiveModule will be applied. In the same
        way, if \a InterfaceType is transmit-only, only the \a TransmitModule will be applied.

        \section senf_emu_apply_decorator_modules Receive and Transmit module/group

        The decorator groups \a ReceiveModule and \a TransmitModule are arbitrary Modules or group
        which take an interface reference as constructor argument. The concrete (derived) argument
        type depends on the type of interface wrapped.

        \code
        // Example module taking a MAC address as constructor argument
        class MyModule : public senf::ppi::module::Module
        {
            SENF_PPI_MODULE(MyModule);
        public:
            MyModule(senf::MACAddress const & mac) { ... }
        };

        // Now define the decorator group
        struct MyReceiveGroup
        {
            senf::emu::RateAnalyzer analyzer;
            MyModule module;

            senf::ppi::connector::PassiveInputJack<EthernetPacket> input;
            senf::ppi::connector::ActiveOutputJack<EthernetPacket> output;

            MyReceiveGroup(senf::emu::Interface & iface)
                : module (iface.macAddress()), input (analyzer.input), output (module.output)
            {
                senf::ppi::connect(analyzer, module);

                // Use the passed in interface to add additional commands to the interfaces
                // console directory
                iface.consoleDir().add("frobble", senf::membind(&MyReceiveGroup::frooble, this));
            }

            void frobble() { ... };
        };
        \endcode

        This group can be applied to any type of interface since the constructor argument is
        senf::emu::Interface &.

        \section senf_emu_apply_decorator_access API access

        \subsection senf_emu_apply_decorator_access_interface Interface API

        Every decorated interface inherits from senf::emu::InterfaceDecorator. This class provides
        access to the basic senf::emu::Interface API and to the input and output connector (when
        enabled depending on the \c canReceive() and \c canTransmit() properties of the underlying
        interface).

        If you need access to a more derived interface API (e.g. senf::emu::WirelessInterface), this
        access is provided by corresponding senf::emu::TypedInterfaceDecorator classes. You do not
        need to define these yourself, each Interface API class has a \c Decorator type member with
        the correct decorator type. Thus, in above case all the following conversions are valid:

        \code
        // Create a decorated WLAN interface instance
        MyDecoratedWLANInterface wlanIface;

        // Generalize this to the more generic WirelessInterface
        WirelessInterface::Decorator & wirelessIface (wlanIface);

        // wirelessIface now provides access to the senf::emu::WirelessInterface API
        wirelessIface.interface().propertyIndex(0);

        // Generalize a step further to the generic Interface API
        Interface::Decorator & iface (wlanIface);

        // iface provides access only to the basic senf::emu::Interface API
        iface.interface().canReceive();
        \endcode

        \subsection senf_emu_apply_decorator_access_modules Wrapper modules

        To access the wrapper modules, the decoration typedef (\c MyDecoration in this example) has
        additional members which access the transmit and/or receive module of a wrapped interface.

        \code
        // Get the receive module of our interface
        MyReceiveGroup & group (MyDecoration::receiveModule(iface));
        \endcode

        Here \a iface can be an \e arbitrary interface wrapped with \c MyDecoration. If the
        interface is transmit-only or is not wrapped with \c MyDecoration, the above command will
        throw \c std::bad_cast. The transmit module/group works the same.

        \see senf::emu::InterfaceDecoratorImplementation / <b>List of all members</b> for a
            documentation of the decorated interface API.
     */
    template <class ReceiveModule=senf::mpl::nil, class TransmitModule=senf::mpl::nil>
    struct ApplyInterfaceDecorator
    {
#ifndef DOXYGEN
        template <class InterfaceType>
        struct apply
        {
            typedef InterfaceDecoratorImplementation<
                InterfaceType, ReceiveModule, TransmitModule> type;

        };
#endif

        static ReceiveModule & receiveModule(InterfaceDecorator & interface);
        static ReceiveModule const & receiveModule(InterfaceDecorator const & interface);

        static TransmitModule & transmitModule(InterfaceDecorator & interface);
        static TransmitModule const & transmitModule(InterfaceDecorator const & interface);
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "InterfaceDecorator.cci"
//#include "InterfaceDecorator.ct"
#include "InterfaceDecorator.cti"
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
