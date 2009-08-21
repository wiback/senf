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
    \brief DebugModules public header */

#ifndef HH_SENF_PPI_DebugModules_
#define HH_SENF_PPI_DebugModules_ 1

// Custom includes
#include <deque>
#include "../Utils/safe_bool.hh"
#include "../Packets/Packets.hh"
#include "MonitorModule.hh"
#include "ActiveFeeder.hh"
#include "../Utils/Logger/SenfLog.hh"

//#include "DebugModules.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {
namespace debug {

    /** \namespace senf::ppi::module::debug
        \brief Debug modules

        This namespace collects several modules helpful for PPI debugging. The modules allow to
        manually pass packets into a network and read back the output packets.

        There are three categories of modules:

        \li <i>Active modules</i> (ActiveSource, ActiveSink) are triggered by external
            calls. Calling \c submit() / \c request() will send the request into the module network
            synchronously. From this it follows, that senf::ppi::run() should \e not be
            called. Instead senf::ppi::init() is used to initialize the network and explicit calls
            to the active debug modules drive the execution.
        \li <i>Passive modules</i> (PassiveSource, PassiveSink) contain a queue to save packets
            (either packets to be transmitted or packets received) and are driven by the network.
        \li <i>Feeder (almost-)modules</i> (ActiveFeederSource, ActiveFeederSink) are a hybrid of
            both types of modules: They contain a packet queue but actively drive the network and
            are used together with senf::ppi::run(). senf::ppi::run() will automatically terminate
            if all available packets have been processed. These are not modules, they are
            collections combining a passive debug module and a senf::ppi::ActiveFeeder.
     */

    /** \brief Debug packet source with active output

        This module provides packets into the network. Each call to submit() will process the packet
        synchronously.

        \note This module should not be used together with senf::ppi::run(). Instead use
            senf::ppi::init() and explicit submit() calls. It follows, that <em>no events will be
            signaled in the network</em>.
     */
    class ActiveSource
        : public Module,
          public safe_bool<ActiveSource>
    {
        SENF_PPI_MODULE(ActiveSource);

    public:
        connector::ActiveOutput<> output;

        ActiveSource();

        void submit(Packet const & packet);     ///< Submit packet
                                        /**< \pre boolean_test() is \c true */

        bool boolean_test() const;      ///< \c true if \a output is not throttled
    };

    /** \brief Debug packet source with passive output

        This module provides a queue of packets for reading by the network. Each submit() call adds
        a packet to the queue which will be sent into the network when requested. The output is
        automatically throttled when the queue becomes empty.
     */
    class PassiveSource
        : public Module
    {
        SENF_PPI_MODULE(PassiveSource);

        typedef std::deque<Packet> Queue;

    public:
        typedef Queue::size_type size_type;

        connector::PassiveOutput<> output;

        PassiveSource();

        void throttle();                ///< Throttle output connector
        void unthrottle();              ///< Unthrottle output connector

        void submit(Packet const & packet);     ///< Enqueue packet

        bool empty();                   ///< \c true if queue is empty
        size_type size();               ///< Number of packets in queue

    private:
        void request();
        virtual void v_init();

        Queue packets_;
    };

    /** \brief Debug packet sink with active input

        This module requests packets from the network. Each call to request() will pass a packet
        request into the network.

        \note This module should not be used together with senf::ppi::run(). Instead use
            senf::ppi::init() and explicit request() calls. It follows, that <em>no events will be
            signaled in the network</em>.
     */
    class ActiveSink
        : public Module,
          public safe_bool<ActiveSink>
    {
        SENF_PPI_MODULE(ActiveSink);

    public:
        connector::ActiveInput<> input;

        ActiveSink();

        Packet request();               ///< Request packet
                                        /**< \pre boolean_test() is \c true */

        bool boolean_test() const;      ///< \c true, if \a input is not throttled
    };

    /** \brief Debug packet sink with passive input

        This module provides a queue for the network to write packets into. The packets can then
        later be analyzed.
     */
    class PassiveSink
        : public Module
    {
        SENF_PPI_MODULE(PassiveSink);

        typedef std::deque<Packet> Queue;

    public:
        typedef Queue::size_type size_type;
        typedef Queue::const_iterator iterator;

        connector::PassiveInput<> input;

        PassiveSink();

        void throttle();                ///< Throttle input connection
        void unthrottle();              ///< Unthrottle input connection

        bool empty();                   ///< \c true, if queue is empty
        size_type size();               ///< number of packets in the queue
        iterator begin();               ///< begin iterator of packets in the queue
        iterator end();                 ///< past-the-end iterator of packets in the queue

        Packet front();                 ///< first packet in the queue
        Packet pop_front();             ///< remove and return first packet in the queue

        void clear();                   ///< clear the queue

    private:
        void request();
        
        Queue packets_;
    };

    /** \brief Active, queue-based packet source

        The ActiveFeederSource contains a packet queue containing the packets to be precessed. These
        packets are actively fed into the network when it is run with senf::ppi::run() until it is
        empty, when senf::ppi::run() will return.

        \note senf::ppi::run will return as soon as no events are active. If want you want is to
            precess a set of packets placed into the ActiveFeederSource queue you must make sure,
            that eventually all events in the module are disabled by throttling or other
            activities. Otherwise, senf::ppi::run() will \e not return.

        ActiveFeederSource is not a module but a collection of two modules: a PassiveSource and an
        ActiveFeeder.
     */
    class ActiveFeederSource
    {
    private:
        PassiveSource source;
        ActiveFeeder feeder;

    public:
        typedef PassiveSource::size_type size_type;

        connector::ActiveOutput<> & output;

        ActiveFeederSource();

        void submit(Packet packet);     ///< enqueue packet
        bool empty();                   ///< \c true, if queue is empty
        size_type size();               ///< number of packets in the queue
    };

    /** \brief Active, queue-based packet sink

        The ActiveFeederSink contains a packet queue to receive the packets from the network. The
        ActiveFeederSink will actively request packets from the network until it's input is
        throttled.

        \note ActiveFeederSink does \e not have a termination condition like ActiveFeederSource, it
            relies on the network to throttle it's input. Additionally, the restrictions of
            ActiveFeederSource apply here too: You need to ensure, that no (additional) events are
            active (eventually) or senf::ppi::run will not return.

        ActiveFeederSink is not a module but a collection of two modules: a PassiveSink and an
        ActiveFeeder.
     */
    class ActiveFeederSink
    {
    private:
        PassiveSink sink;
        ActiveFeeder feeder;

    public:
        typedef PassiveSink::size_type size_type;
        typedef PassiveSink::iterator iterator;

        connector::ActiveInput<> & input;

        ActiveFeederSink();

        bool empty();
        size_type size();
        iterator begin();
        iterator end();

        Packet front();
        Packet pop_front();

        void clear();
    };

    /** \brief Log received packets

        This module will log all packets sent to it's input using SENF_LOG to the given log
        \a Stream, \a Area and \a Level.
     */
    template < class Stream = log::Debug,
               class Area   = log::DefaultArea,
               class Level  = log::VERBOSE >
    class Logger
        : public MonitorModule<>
    {
        SENF_PPI_MODULE(Logger);
    public:
        Logger();
        explicit Logger(std::string label);

    private:
        virtual void v_handlePacket(Packet const & packet);

        std::string label_;
    };

}}}}

///////////////////////////////hh.e////////////////////////////////////////
#include "DebugModules.cci"
//#include "DebugModules.ct"
#include "DebugModules.cti"
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
