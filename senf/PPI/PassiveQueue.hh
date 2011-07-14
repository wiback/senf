// $Id$
//
// Copyright (C) 2007
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

        template <class QDiscipline>
        void qdisc(QDiscipline const & disc); ///< Change the queueing discipline
                                        /**< This call changes the queueing discipline of the
                                             queue. This call is just forwarded to the \a input
                                             connector.

                                             \see connector::GenericPassiveInput::qdisc() */

    private:
        virtual void v_init();

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
