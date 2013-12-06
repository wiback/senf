// $Id$
//
// Copyright (C) 2010
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief QueueingSocketSink public header */

#ifndef HH_SENF_PPI_QueueingSocketSink_
#define HH_SENF_PPI_QueueingSocketSink_ 1

// Custom includes
#include "QueueingAlgorithm.hh"
#include "SocketSink.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
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
        console::ScopedDirectory<PassiveQueueingSocketSink<Writer> > dir;

        explicit PassiveQueueingSocketSink(Handle const & handle, QueueingAlgorithm::ptr qAlgorithm);

        Writer & writer();              ///< Access the Writer
        Handle & handle();              ///< Access handle
        void handle(Handle const & handle);
                                        ///< Set handle
                                        /**< Assigning an empty or in-valid() handle will disable
                                             the module until a new valid handle is assigned. */
        QueueingAlgorithm & qAlgorithm() const;
        void qAlgorithm(QueueingAlgorithm::ptr qAlgorithm);

    private:
        void write();
        void writable();
        void checkThrottle();
        void setQAlgorithm(std::string const & key);

        Handle handle_;
        Writer writer_;
        boost::scoped_ptr<QueueingAlgorithm> qAlgo_;
        IOEvent event_;
    };

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
