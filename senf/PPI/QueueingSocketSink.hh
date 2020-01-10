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
        console::ScopedDirectory<> dir;

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
