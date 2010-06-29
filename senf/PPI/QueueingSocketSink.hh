// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief QueueingSocketSink public header */

#ifndef HH_SENF_PPI_QueueingSocketSink_
#define HH_SENF_PPI_QueueingSocketSink_ 1

// Custom includes
#include <queue>
#include "SocketSink.hh"

//#include "QueueingSocketSink.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    template <typename PacketType=Packet>
    class QueueingAlgorithm
    {
    public:
        virtual ~QueueingAlgorithm() {};
        virtual PacketType dequeue() = 0;
        virtual bool enqueue(PacketType const & packet) = 0;
        virtual unsigned size() const = 0;
        virtual void clear() = 0;
    };

    template <typename PacketType=Packet>
    class FIFOQueueingAlgorithm : public QueueingAlgorithm<PacketType>
    {
    public:
        FIFOQueueingAlgorithm(unsigned size);

        virtual PacketType dequeue();
        virtual bool enqueue(PacketType const & packet);
        virtual unsigned size() const;
        virtual void clear();

    private:
        std::queue<PacketType> queue_;
        unsigned size_;
    };


namespace module {

    /** \brief QueueingSocketSink

        \ingroup io_modules
     */
    template <class Writer=ConnectedDgramWriter>
    class PassiveQueueingSocketSink : public Module
    {
        SENF_PPI_MODULE(PassiveQueueingSocketSink);

    public:
        typedef typename Writer::Handle Handle; ///< Handle type requested by writer
        typedef typename Writer::PacketType PacketType;

        connector::PassiveInput<PacketType> input; ///< Input connector from which data is received

        template <class QAlgorithm>
        explicit PassiveQueueingSocketSink(Handle handle, QAlgorithm const & qAlgorithm);

        Writer & writer();              ///< Access the Writer
        Handle & handle();              ///< Access handle
        void handle(Handle handle);     ///< Set handle
                                        /**< Assigning an empty or in-valid() handle will disable
                                             the module until a new valid handle is assigned. */
        QueueingAlgorithm<PacketType> & qAlgorithm();

    private:
        void write();
        void writable();
        void checkThrottle();

        Handle handle_;
        Writer writer_;
        boost::scoped_ptr<QueueingAlgorithm<PacketType> > qAlgo_;
        IOEvent event_;
    };

}}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "QueueingSocketSink.cci"
#include "QueueingSocketSink.ct"
#include "QueueingSocketSink.cti"
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
