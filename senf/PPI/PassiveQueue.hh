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
    \brief PassiveQueue public header */

#ifndef HH_SENF_PPI_PassiveQueue_
#define HH_SENF_PPI_PassiveQueue_ 1

// Custom includes
#include "Connectors.hh"
#include "Module.hh"
#include "predecl.hh"

//#include "PassiveQueue.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {

    /** \brief Adaptor to connect active a pair of active connectors.

        This adaptor queue is used to connect two active connectors to each other. The queue
        receives data in it's passive input and places them in it's queue. Whenever data is
        requested from the passive output, a packet is dequeued.

        The PassiveQueue will automatically throttle in both directions. Throttling on the input
        connector is the standard throttling as implemented in
        connector::PassiveInput<>. Additional forward throttling notifications are sent out
        whenever the queue is empty.

        \ingroup adapter_modules
     */
    class PassiveQueue
        : public module::Module
    {
        SENF_PPI_MODULE(PassiveQueue);
    public:
        connector::PassiveInput<> input;
        connector::PassiveOutput<> output;

        PassiveQueue();

        template <class ThrottlingDisc>
        void throttlingDisc(ThrottlingDisc const & disc);
                                        ///< Change the throttling discipline
                                        /**< This call changes the throttling discipline of the
                                             queue. This call is just forwarded to the \a input
                                             connector.

                                             \see connector::GenericPassiveInput::throttlingDisc() */

    private:
        virtual void v_init() override;

        void onInput();
        void onOutput();
    };

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "PassiveQueue.cci"
//#include "PassiveQueue.ct"
#include "PassiveQueue.cti"
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
