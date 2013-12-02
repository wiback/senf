// $Id$
//
// Copyright (C) 2013
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// http://senf.berlios.de/license.html
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief QueueSocketSourceSink public header */

#ifndef HH_SENF_PPI_QueueSocketSourceSink_
#define HH_SENF_PPI_QueueSocketSourceSink_ 1

// Custom includes
#include <senf/Socket/SocketPolicy.hh>
#include <senf/Socket/ClientSocketHandle.hh>
#include <senf/Socket/QueueReadWritePolicy.hh>
#include <senf/Socket/FramingPolicy.hh>
#include <senf/Socket/CommunicationPolicy.hh>
#include <senf/Scheduler/EventHook.hh>
#include "QueueingSocketSink.hh"
#include "Module.hh"
#include "Connectors.hh"
#include "IOEvent.hh"

#include "QueueSocketSourceSink.ih"
//#include "QueueSocketSourceSink.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {

    struct QueueBufferAnnotation
    {
        QueueReadPolicy::Buffer const * value;
    };

    std::ostream & operator<<(std::ostream & os, QueueBufferAnnotation const & annotation);

namespace module {

    /** \brief Reader for module::ActiveSocketSource (read from socket packet queue)

        This read helper will read a datagram from a socket providing the queue read API. The Packet
        will be instantiated utilizing the queue memory using the external packet memory manager
        support.

        \see \ref senf::QueueReadPolicy
     */
    template <class Packet=DataPacket,
              class Connector=connector::ActiveOutput<Packet> >
    class ActiveQueueSocketSource
        : public Module
    {
        SENF_PPI_MODULE(ActiveQueueSocketSource);
    public:
        typedef Packet PacketType;
        typedef senf::ClientSocketHandle<
            senf::MakeSocketPolicy< senf::QueueReadPolicy,
                                    senf::DatagramFramingPolicy >::policy > Handle;

        Connector output;

        explicit ActiveQueueSocketSource(unsigned burst=1);
        explicit ActiveQueueSocketSource(Handle const & handle, unsigned burst=1);

        Handle handle() const;
        void handle(Handle const & handle);

        unsigned maxBurst() const;
        void maxBurst(unsigned burst);

        void release();

#ifdef SENF_DEBUG
        unsigned burstMax();
        unsigned sharedPackets();
#endif

    private:
        Handle handle_;
        IOEvent event_;
        unsigned maxBurst_;

#ifdef SENF_DEBUG
        unsigned burstMax_;
        unsigned sharedPackets_;
#endif

        void read();
    };

    template <class Connector=connector::PassiveInput<> >
    class PassiveQueueSocketSink
        : public Module
    {
        SENF_PPI_MODULE(PassiveQueueSocketSink);
    public:
        typedef Packet PacketType;
        typedef senf::ClientSocketHandle<
            senf::MakeSocketPolicy< senf::QueueWritePolicy,
                                    senf::DatagramFramingPolicy,
                                    senf::ConnectedCommunicationPolicy >::policy > Handle;

        Connector input;

        PassiveQueueSocketSink();
        explicit PassiveQueueSocketSink(Handle const & handle);

        Handle handle() const;
        void handle(Handle const & handle);

        QueueingAlgorithm & qAlgorithm() const;
        void qAlgorithm(QueueingAlgorithm::ptr qAlgorithm);

        void send();

#ifdef SENF_DEBUG
        unsigned burstMax();
#endif

    private:
        void write();
        void writable();
        void writePacket(senf::Packet const & packet, QueueReadPolicy::Buffer & buf);

        Handle handle_;
        scheduler::EventHook eventHook_;
        IOEvent ioEvent_;
        boost::scoped_ptr<QueueingAlgorithm> qAlgo_;

#ifdef SENF_DEBUG
        unsigned burst_;
        unsigned burstMax_;
#endif
    };
}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "QueueSocketSourceSink.cci"
//#include "QueueSocketSourceSink.ct"
#include "QueueSocketSourceSink.cti"
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
