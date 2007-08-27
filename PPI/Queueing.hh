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

        \attention Notifications may be forwarded to the QueueingDiscipline implementation
            out-of-order: A dequeue event may be notified before the corresponding enqueue
            event (this happens to optimize away transient throttling state changes which would
            otherwise occur if a packet is entered into the queue and then removed from it in the
            same processing step).
     */
    class QueueingDiscipline
    {
    public:
        virtual ~QueueingDiscipline();

        enum Event { ENQUEUE, DEQUEUE }; ///< Possible queueing events
        
        virtual void update(connector::PassiveInput & input, Event event) = 0;
                                        ///< Calculate new queueing state
                                        /**< Whenever the queue is manipulated, this member is
                                             called to calculate the new throttling state. The
                                             member must call \a input's \c throttle() or \c
                                             unthrottle() member to set the new throttling state.
                                             
                                             \param[in] input Connector holding the queue
                                             \param[in] event Type of event triggering the update */
    };

    /** \brief Simple queueing discipline with high and low threshold

        The ThresholdQueueing QueueingDiscipline is a simple queueing discipline which throttles the
        input as soon the number of packets in the queue reaches the \a high threshold. The input
        will be unthrottled when the number of packets drops to the \a low threshold. 

        The default queueing discipline is ThresholdQueueing(1,0).
     */
    class ThresholdQueueing
        : public QueueingDiscipline
    {
    public:
        ThresholdQueueing(unsigned high, unsigned low);

        virtual void update(connector::PassiveInput & input, Event event);

    private:
        unsigned high_;
        unsigned low_;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Queueing.cci"
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
