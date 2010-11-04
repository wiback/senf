// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief Connectors public header */

#ifndef HH_SENF_PPI_Connectors_
#define HH_SENF_PPI_Connectors_ 1

// Custom includes
#include <deque>
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>
#include <senf/Utils/safe_bool.hh>
#include <senf/Utils/Exception.hh>
#include <senf/Packets/Packets.hh>
#include "predecl.hh"
#include "detail/Callback.hh"
#include "Queueing.hh"
#include "ModuleManager.hh"

//#include "Connectors.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
namespace connector {

    /** \namespace senf::ppi::connector
        \brief Connector classes

        A connector has three independent properties
        \li it may be \e active or \e passive
        \li it may be an \e input or an \e output
        \li it has an (optional) packet type

        \e Active connectors are activated from within the module, \e passive connectors are
        signaled by the external framework. \e Input connectors receive packets, \e output
        connectors send packets.

        All passive connectors call some onRequest callback whenever I/O needs to be performed. All
        input connectors possess a packet queue.

        We therefore have 4 connector types each of which is parameterized by the type of packet
        traversing the connector:
        \li senf::ppi::connector::ActiveInput
        \li senf::ppi::connector::ActiveOutput
        \li senf::ppi::connector::PassiveInput
        \li senf::ppi::connector::PassiveOutput.

        Connectors are declared as module data members and are then externally connected to other
        modules.

        The connectors each take an optional template argument. If this argument is specified, it
        must be the type of packet expected or sent on this connector. If it is not specified,
        packets will be passed using the generic Packet handle.

        \code
        class IpFilter : public senf::ppi::module::Module
        {
            SENF_PPI_MODULE(SomeModule);

        public:
            senf::ppi::connector::ActiveInput<senf::EthernetPacket> input;
            senf::ppi::connector::PassiveOutput<senf::IpPacket> output;

            IpFilter() {
                route(input, output);
                input.onRequest(&IpFilter::onRequest);
            }

        private:
            void onRequest() {
                // 'input()' will return a senf::EthernetPacket packet handle
                try { output( input().find<senf::IpPacket>() ); }
                catch (senf::InvalidPacketChainException & ex) { ; }
            }
        };
        \endcode


        \section ppi_jacks Jacks

        A Jack is a packet type aware and possibly packet type converting reference to an arbitrary
        connector of the same type. Jacks are used in groups to indirectly declare the input's and
        output's

        \code
        class MyGroup
        {
        private:
            senf::ppi::module::PassiveQueue queue;
            senf::ppi::module::RateAnalyzer analyzer;

        public:
            senf::ppi::connector::ActiveInputJack<senf::EthernetPacket> input;
            senf::ppi::connector::ActiveOutputJack<senf::EthernetPacket> output;

            MyGroup()
                : queue (), analyzer (), input (queue.input), output (analyzer.output)
            {
                senf::ppi::connect(queue, analyzer);
            }
        };
        \endcode

        The jacks are initialized by passing an arbitrary compatible connector to the jack
        constructor. A connector is compatible, if
        \li It has the same input/output active/passive specification
        \li Either the Jack or the Connector are generic (senf::Packet) or Jack and Connector have
            the same packet type

        Jacks can be used wherever connectors may be used. Jacks may be defined anywhere, not only
        in modules. It is however important to ensure that the lifetime of the jack does not exceed
        the lifetime of the referenced connector.

        \see
            senf::ppi::module::Module \n
            senf::ppi::connect() \n
            \ref ppi_connectors
     */

    /** \brief Incompatible connectors connected

        This exception is thrown, when two incompatible connectors are connected. This happens if
        both connectors of a senf::ppi::connect() statement declare a packet type (the connector
        template argument) but they don't declare the same packet type.

        You need to ensure, that both connectors use the same packet type.

        \see senf::ppi::connect()
     */
    struct IncompatibleConnectorsException : public senf::Exception
    { IncompatibleConnectorsException() : senf::Exception("Incompatible connectors") {} };

    /** \brief Connector base-class

        This connector provides access to the generic connector facilities. This includes the
        connection management (access to the connected peer) and the containment management (access
        to the containing module)
     */
    class Connector
        : ModuleManager::Initializable, boost::noncopyable
    {
        SENF_LOG_CLASS_AREA();
        SENF_LOG_DEFAULT_LEVEL(senf::log::NOTICE);
    public:
        Connector & peer() const;       ///< Get peer connected to this connector
        module::Module & module() const; ///< Get this connectors containing module

        bool connected() const;         ///< \c true, if connector connected, \c false otherwise

        void disconnect();              ///< Disconnect connector from peer

        enum TraceState { NO_TRACING, TRACE_IDS, TRACE_CONTENTS };

        static void tracing(TraceState state);
        static TraceState tracing();

    protected:
        Connector();
        virtual ~Connector();

        void connect(Connector & target);

        void trace(Packet const & p, char const * label);
        void throttleTrace(char const * label, char const * type);

        void unregisterConnector();

        virtual void v_disconnected();
        virtual void v_connected();

    private:
        virtual std::type_info const & packetTypeID();

        void setModule(module::Module & module);

        Connector * peer_;
        module::Module * module_;

        static TraceState traceState_;

        friend class module::Module;
    };

    /** \brief Passive connector base-class

        A passive connector is a connector which is activated externally whenever an I/O request
        occurs. Passive connectors are the origin of throttling notifications. Depending on the type
        of connector (output or input) the respective throttling is called forward or backward
        throttling.

        Passive connectors always handle two throttling states:

        - The \e native throttling state is set manually by the module. It is the throttling state
            originating in the current module
        - The \e forwarded throttling state is the state as it is received by throttling
            notifications

        The accumulative throttling state is generated by combining all sub-states.
     */
    class PassiveConnector
        : public virtual Connector
    {
    public:
        ~PassiveConnector();

        template <class Handler>
        void onRequest(Handler handler);///< Register I/O event handler
                                        /**< The registered handler will be called, whenever packets
                                             arrive or should be generated by the module depending
                                             on the connector type (input or output). The \a handler
                                             argument is either an arbitrary callable object or it
                                             is a pointer-to-member to a member of the class which
                                             holds this input. In the second case, the pointer will
                                             automatically be bound to the containing instance.

                                             \param[in] handler Handler to call, whenever an I/O
                                                 operation is to be performed. */


        bool throttled() const;         ///< Get accumulative throttling state
        bool nativeThrottled() const;   ///< Get native throttling state

        void throttle();                ///< Set native throttling
        void unthrottle();              ///< Revoke native throttling

        ActiveConnector & peer() const;

    protected:
        PassiveConnector();

        void emit();

        virtual void v_disconnected();
        virtual void v_connected();

    private:
        virtual void v_init();

        // Called by the routing to change the throttling state from forwarding routes
        void notifyThrottle();          ///< Forward a throttle notification to this connector
        void notifyUnthrottle();        ///< Forward an unthrottle notification to this connector

        // Internal members to emit throttling notifications to the connected peer
        void emitThrottle();
        void emitUnthrottle();

        // Called after unthrottling the connector
        virtual void v_unthrottleEvent();

        // called by ForwardingRoute to register a new route
        void registerRoute(ForwardingRoute & route);
        void unregisterRoute(ForwardingRoute & route);

        ActiveConnector * peer_;

        typedef ppi::detail::Callback<>::type Callback;
        Callback callback_;

        bool remoteThrottled_;
        bool nativeThrottled_;

        typedef std::vector<ForwardingRoute*> Routes;
        Routes routes_;

        friend class senf::ppi::ForwardingRoute;
    };

    /** \brief Active connector base-class

        An active connector is a connector which emits I/O requests. Active connectors receive
        throttling notifications. Depending on the type of connector (input or output) the
        respective throttling is called forward or backward throttling.

        Active connectors do not handle any throttling state, they just receive the
        notifications. These notifications should then either be processed by the module or be
        forwarded to other connectors.
     */
    class ActiveConnector
        : public virtual Connector
    {
        typedef ppi::detail::Callback<>::type Callback;
    public:
        ~ActiveConnector();

        template <class Handler>
        void onThrottle(Handler handler); ///< Register throttle notification handler
                                        /**< The handler register here will be called, whenever a
                                             throttle notification comes in. The \a handler argument
                                             is either an arbitrary callable object or it is a
                                             pointer-to-member to a member of the class which holds
                                             this input. In the second case, the pointer will
                                             automatically be bound to the containing instance.

                                             \param[in] handler Handler to call on throttle
                                                 notifications. */
        void onThrottle();              ///< Clear throttle notification handler

        template <class Handler>
        void onUnthrottle(Handler handler); ///< Register unthrottle notification handler
                                        /**< The handler register here will be called, whenever an
                                             unthrottle notification comes in. The \a handler
                                             argument is either an arbitrary callable object or it
                                             is a pointer-to-member to a member of the class which
                                             holds this input. In the second case, the pointer will
                                             automatically be bound to the containing instance.

                                             \param[in] handler Handler to call on unthrottle
                                                 notifications. */
        void onUnthrottle();            ///< Clear unthrottle notification handler

        bool throttled() const;         ///< \c true, if peer() is throttled

        PassiveConnector & peer() const;

    protected:
        ActiveConnector();

        virtual void v_disconnected();
        virtual void v_connected();

    private:
        virtual void v_init();

        // called by the peer() to forward throttling notifications
        void notifyThrottle();
        void notifyUnthrottle();

        // called by ForwardingRoute to register a new route
        void registerRoute(ForwardingRoute & route);
        void unregisterRoute(ForwardingRoute & route);

        PassiveConnector * peer_;

        Callback throttleCallback_;
        Callback unthrottleCallback_;

        typedef std::vector<ForwardingRoute*> NotifyRoutes;
        NotifyRoutes notifyRoutes_;

        bool throttled_;

        friend class senf::ppi::ForwardingRoute;
        friend class PassiveConnector;
    };

    /** \brief Input connector base-class

        An input connector receives packets. It may be either an ActiveConnector or a
        PassiveConnector. An input connector contains a packet queue. This queue enables processing
        packets in batches or generating multiple output packets from a single input packet. The
        queues have the potential to greatly simplify the module implementations.

        \implementation Which container to use?
            \li list has good insertion and deletion properties on both ends but it costs a dynamic
                memory allocation for every insertion. A very good property is, that iterators stay
                valid across insertions/deletions
            \li vector is fast and has good amortized dynamic allocation properties. However, it is
                quite unusable as a queue
            \li deque has comparable dynamic allocation properties as vector but also has good
                insertion/removal properties on both ends.

            So probably we will use a deque. I'd like a container which keeps iterators intact on
            insertion/deletion but I believe that list is just to expensive since every packet will
            be added to the queue before it can be processed.
     */
    class InputConnector
        : public virtual Connector
    {
        typedef std::deque<Packet> Queue;
    public:
        typedef Queue::const_iterator queue_iterator; ///< Iterator type of the embedded queue
        typedef Queue::size_type size_type; ///< Unsigned type for counting queue elements


        Packet operator()();            ///< Get a packet
                                        /**< This member is the primary method to access received
                                             data. On passive connectors, this operator will just
                                             dequeue a packet from the packet queue. If the
                                             connector is active, the connector will request new
                                             packets from the connected module. If the packet
                                             request cannot be fulfilled an in-valid Packet is
                                             returned. */

        Packet read();                  ///< Alias for operator()()

        OutputConnector & peer() const;

        queue_iterator begin() const;   ///< Access queue begin (head)
        queue_iterator end() const;     ///< Access queue past-the-end (tail)
        Packet peek() const;            ///< Return head element from the queue

        size_type queueSize() const;    ///< Return number of elements in the queue
        bool empty() const;             ///< Return queueSize() == 0

    protected:
        InputConnector();

        virtual void v_disconnected();
        virtual void v_connected();

    private:
        void enqueue(Packet const & p);

        virtual void v_requestEvent();
        virtual void v_enqueueEvent();
        virtual void v_dequeueEvent();

        OutputConnector * peer_;
        Queue queue_;

        friend class OutputConnector;
    };

    /** \brief Output connector base-class

        An output connector sends out packets. It may be either an ActiveConnector or a
        PassiveConnector. An output connector does \e not have an built-in queueing, it relies on
        the queueing of the connected input.
     */
    class OutputConnector
        : public virtual Connector
    {
    public:
        void operator()(Packet const & p);      ///< Send out a packet

        void write(Packet const & p);           ///< Alias for operator()(Packet p)

        InputConnector & peer() const;

    protected:
        OutputConnector();

        virtual void v_disconnected();
        virtual void v_connected();

    private:
        InputConnector * peer_;
    };

    /** \brief Combination of PassiveConnector and InputConnector

        The GenericPassiveInput automatically controls the connectors throttling state using a
        queueing discipline. The standard queueing discipline is ThresholdQueueing, which throttles
        the connection whenever the queue length reaches the high threshold and unthrottles the
        connection when the queue reaches the low threshold. The default queueing discipline is
        <tt>ThresholdQueueing(1,0)</tt> which will throttle the input whenever the queue is
        non-empty.
     */
    class GenericPassiveInput
        : public PassiveConnector, public InputConnector,
          public safe_bool<GenericPassiveInput>
    {
    public:
        GenericActiveOutput & peer() const;

        bool boolean_test() const;      ///< \c true, if ! empty()

        template <class QDisc>
        void qdisc(QDisc const & disc); ///< Change the queueing discipline
                                        /**< The queueing discipline is a class which provides the
                                             QueueingDiscipline interface.

                                             \param[in] disc New queueing discipline */
        void qdisc(QueueingDiscipline::None_t);
                                        ///< Disable queueing discipline

    protected:
        GenericPassiveInput();

        virtual void v_disconnected();
        virtual void v_connected();

    private:
        void v_enqueueEvent();
        void v_dequeueEvent();
        void v_unthrottleEvent();

        GenericActiveOutput * peer_;
        boost::scoped_ptr<QueueingDiscipline> qdisc_;
    };

    /** \brief Combination of PassiveConnector and OutputConnector
     */
    class GenericPassiveOutput
        : public PassiveConnector, public OutputConnector,
          public safe_bool<GenericPassiveOutput>
    {
    public:
        GenericActiveInput & peer() const;

        bool boolean_test() const;      ///< Always \c true

        void connect(GenericActiveInput & target); ///< Internal: Use senf::ppi::connect() instead

        friend class GenericActiveInput;

    protected:
        GenericPassiveOutput();

        virtual void v_disconnected();
        virtual void v_connected();

    private:
        GenericActiveInput * peer_;
    };

    /** \brief Combination of ActiveConnector and InputConnector
     */
    class GenericActiveInput
        : public ActiveConnector, public InputConnector,
          public safe_bool<GenericActiveInput>
    {
    public:
        GenericPassiveOutput & peer() const;

        bool boolean_test() const;      ///< \c true, if ! empty() or ! throttled()

        void request();                 ///< request more packets without dequeuing any packet

    protected:
        GenericActiveInput();

        virtual void v_disconnected();
        virtual void v_connected();

    private:
        void v_requestEvent();

        GenericPassiveOutput * peer_;
    };

    /** \brief Combination of ActiveConnector and OutputConnector
     */
    class GenericActiveOutput
        : public ActiveConnector, public OutputConnector,
          public safe_bool<GenericActiveOutput>
    {
    public:
        GenericPassiveInput & peer() const;

        bool boolean_test() const;      ///< \c true if peer() is ! throttled()

        void connect(GenericPassiveInput & target); ///< Internal: Use senf::ppi::connect() instead

    protected:
        GenericActiveOutput();

        virtual void v_disconnected();
        virtual void v_connected();

    private:
        GenericPassiveInput * peer_;
    };


#ifndef DOXYGEN

#   define TypedConnector_Input read
#   define TypedConnector_Output write
#   define TypedConnector(pType, dir)                                                             \
        template <class PacketType>                                                               \
        class pType ## dir                                                                        \
            : public Generic ## pType ## dir,                                                     \
              private detail::Typed ## dir ## Mixin<pType ## dir <PacketType>, PacketType>        \
        {                                                                                         \
            typedef detail::Typed ## dir ## Mixin<pType ## dir <PacketType>, PacketType> mixin;   \
        public:                                                                                   \
            using mixin::operator();                                                              \
            using mixin::TypedConnector_ ## dir ;                                                 \
        private:                                                                                  \
            virtual std::type_info const & packetTypeID()                                         \
                { return typeid(typename PacketType::type); }                                     \
            friend class detail::Typed ## dir ## Mixin<pType ## dir <PacketType>, PacketType>;    \
        };                                                                                        \
        template <>                                                                               \
        class pType ## dir <Packet> : public Generic ## pType ## dir                              \
        {}

    TypedConnector( Passive, Input  );
    TypedConnector( Passive, Output );
    TypedConnector( Active,  Input  );
    TypedConnector( Active,  Output );

#   undef TypedConnector
#   undef TypedConnector_Input
#   undef TypedConnector_Output

#else

    /** \brief Connector actively reading packets

        \tparam PacketType Type of packet to read. Defaults to senf::Packet

        The %ActiveInput %connector template reads data actively from a connected %module. This
        class is completely implemented via it's base-class, GenericActiveInput, the only
        difference is that read packets are returned as \a PacketType instead of generic
        senf::Packet references.

        \see GenericActiveInput \n
            senf::ppi::connector
     */
    template <class PacketType=Packet>
    class ActiveInput : public GenericActiveInput
    {
    public:
        PacketType operator()();        ///< Read packet
                                        /**< \throws std::bad_cast if the %connector receives a
                                             Packet which is not of type \a PacketType.
                                             \returns newly read packet reference. */
        PacketType read();              ///< Alias for operator()
    };

    /** \brief Connector passively receiving packets

        \tparam PacketType Type of packet to read. Defaults to senf::Packet

        The %PassiveInput %connector template receives packets sent to it from a connected
        %module. This class is completely implemented via it's base-class, GenericPassiveInput,
        the only difference is that read packets are returned as \a PacketType instead of generic
        senf::Packet references.

        \see GenericPassiveInput \n
            senf::ppi::connector
     */
    template <class PacketType=Packet>
    class PassiveInput : public GenericPassiveInput
    {
    public:
        PacketType operator()();        ///< Read packet
                                        /**< \throws std::bad_cast if the %connector receives a
                                             Packet which is not of type \a PacketType.
                                             \returns newly read packet reference. */
        PacketType read();              ///< Alias for operator()
    };

    /** \brief Connector actively sending packets

        \tparam PacketType Type of packet to send. Defaults to senf::Packet

        The %ActiveOutput %connector template sends data actively to a connected %module. This
        class is completely implemented via it's base-class, GenericActiveOutput, the only
        difference is that it only sends packets of type \a PacketType.

        \see GenericActiveOutput \n
            senf::ppi::connector
     */
    template <class PacketType=Packet>
    class ActiveOutput : public GenericActiveOutput
    {
    public:
        void operator()(PacketType packet); ///< Send out a packet
        void write(PacketType packet);      ///< Alias for operator()
    };

    /** \brief Connector passively providing packets

        \tparam PacketType Type of packet to send. Defaults to senf::Packet

        The %PassiveOutput %connector template provides data passively to a connected %module
        whenever signaled. This class is completely implemented via it's base-class,
        GenericPassiveOutput, the only difference is that it only sends packets of type
        \a PacketType.

        \see GenericPassiveOutput \n
            senf::ppi::connector
     */
    template <class PacketType=Packet>
    class PassiveOutput : public GenericPassiveOutput
    {
    public:
        void operator()(PacketType packet); ///< Send out a packet
        void write(PacketType packet);      ///< Alias for operator()
    };

#endif

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Connectors.cci"
#include "Connectors.ct"
#include "Connectors.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
