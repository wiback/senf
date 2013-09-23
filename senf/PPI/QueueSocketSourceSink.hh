// $Id$
//
// Copyright (C) 2013
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
#include "Module.hh"
#include "Connectors.hh"
#include "IOEvent.hh"

//#include "QueueSocketSourceSink.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {

    /** \brief Reader for module::ActiveSocketSource (read from socket packet queue)

        This read helper will read a datagram from a socket providing the queue read API. The Packet
        will be instantiated utilizing the queue memory using the external packet memory manager
        support.

        \warning You must call \c release() explicitly when done. You must ensure, that when calling
            \c release() no live packet handle still references the packet data.

        \see \ref senf::QueueReadPolicy
     */
    template <class Packet=DataPacket>
    class ActiveQueueSocketSource
        : public Module
    {
        SENF_PPI_MODULE(ActiveQueueSocketSource);
    public:
        typedef Packet PacketType;
        typedef senf::ClientSocketHandle<
            senf::MakeSocketPolicy< senf::QueueReadPolicy,
                                    senf::DatagramFramingPolicy >::policy > Handle;

        connector::ActiveOutput<Packet> output;

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

        connector::PassiveInput<> input;

        PassiveQueueSocketSink();
        explicit PassiveQueueSocketSink(Handle const & handle);

        Handle handle() const;
        void handle(Handle const & handle);

        void send();

#ifdef SENF_DEBUG
        unsigned burstMax();
#endif

    private:
        void write();

        Handle handle_;
        scheduler::EventHook event_;

#ifdef SENF_DEBUG
        unsigned burst_;
        unsigned burstMax_;
#endif
    };
}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "QueueSocketSourceSink.cci"
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
