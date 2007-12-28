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
    \brief PassiveQueue public header */

#ifndef HH_PassiveQueue_
#define HH_PassiveQueue_ 1

// Custom includes
#include "Connectors.hh"
#include "Module.hh"
#include "predecl.hh"

//#include "PassiveQueue.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {
    
    /** \brief Adaptor to connect active a pair of active connectors.

        This adaptor queue is used to connect two active connectors to each other. The queue
        receives data in it's passive input and places them in it's queue. Whenever data is
        requested from the passive output, a packet is dequeued. 

        The PassiveQueue will automatically throttle in both directions. Throttling on the input
        connector is the standard throttling as implemented in connector::PassiveInput. Additional,
        forward throttling notifications are sent out whenever the queue is empty.

        \ingroup adapter_modules
     */
    class PassiveQueue 
        : public module::Module
    {
        SENF_PPI_MODULE(PassiveQueue);
    public:
        connector::PassiveInput input;
        connector::PassiveOutput output;
        
        PassiveQueue();

        template <class QDiscipline>
        void qdisc(QDiscipline const & disc); ///< Change the queueing discipline
                                        /**< This call changes the queueing discipline of the
                                             queue. This call is just forwarded to the \a input
                                             connector.
                                             
                                             \see connector::PassiveInput::qdisc() */

    private:
        void init();

        void onInput();
        void onOutput();
    };

}}}

///////////////////////////////hh.e////////////////////////////////////////
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
