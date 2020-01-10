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
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
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
        mutable QueueReadPolicy::Buffer const * value;
        QueueReadPolicy::Buffer const * operator->() const;
    };

    std::ostream & operator<<(std::ostream & os, QueueBufferAnnotation const & annotation);

    typedef std::function<bool (senf::Packet const & pkt, std::uint32_t usage)> REDFilterCallback;
    
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

        void flush();

#ifdef SENF_DEBUG
        unsigned burstMax();
#endif
        
        void setREDFilterCallback(REDFilterCallback const & cb);

        std::pair<unsigned,unsigned> dropped();

    private:
        Handle handle_;
        IOEvent event_;
        REDFilterCallback redFilterCallback_;
        unsigned maxBurst_;
        unsigned burst_;
        bool flushPending_;
        unsigned red_;
        
#ifdef SENF_DEBUG
        unsigned burstMax_;
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

        std::tuple<unsigned,unsigned,unsigned> dropped();
        
        void send();
        void flush();

#ifdef SENF_DEBUG
        unsigned burstMax();
#endif

        void setREDFilterCallback(REDFilterCallback const & cb);

    private:
        void write();
        void writable();
        void writePacket(senf::Packet const & packet, QueueReadPolicy::Buffer & buf);

        Handle handle_;
        scheduler::EventHook eventHook_;
        IOEvent writeEvent_;
        REDFilterCallback redFilterCallback_;
        unsigned red_;
        unsigned dropped_;
        
#ifdef SENF_DEBUG
        unsigned burst_;
        unsigned burstMax_;
#endif
    };

    class QueueEthVLanFilter
        : public Module
    {
        SENF_PPI_MODULE(QueueEthVLanFilter);
    public:
        connector::PassiveInput<EthernetPacket> input;
        connector::ActiveOutput<EthernetPacket> output;

        QueueEthVLanFilter();

    private:
        void request();
    };
}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "QueueSocketSourceSink.cci"
#include "QueueSocketSourceSink.ct"
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
