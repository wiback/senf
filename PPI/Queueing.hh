// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief Queueing public header */

#ifndef HH_Queueing_
#define HH_Queueing_ 1

// Custom includes
#include "predecl.hh"

//#include "Queueing.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief Queueing discipline base class

        QueueingDescipline derived classes define the generation of throttling notifications. The
        QueueingDiscipline is called whenever the packets are entered or removed from the queue. The
        queueing discipline then determines the new throttling state of the queue.

        \note The QueueingDiscipline will \e never drop packets explicitly. This is left to the
        operating system by sending throttling events. The PPI will never loose a packet internally
        (if not a module explicitly does so), however it may disable reception of new incoming
        packets which will then probably be dropped by the operating system.
     */
    class QueueingDiscipline
    {
    public:
        virtual ~QueueingDiscipline();

        enum Event { ENQUEUE, DEQUEUE }; ///< Possible queueing events
        enum State { THROTTLED, UNTHROTTLED }; ///< Possible queueing states
        
        virtual State update(connector::PassiveInput & input, Event event) = 0;
                                        ///< Calculate new queueing state
                                        /**< Whenever the queue is manipulated, this member is
                                             called to calculate the new throttling state.
                                             
                                             \param[in] input Connector holding the queue
                                             \param[in] event Type of event triggering the update
                                             \returns new throttling state */
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Queueing.cci"
//#include "Queueing.ct"
//#include "Queueing.cti"
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
