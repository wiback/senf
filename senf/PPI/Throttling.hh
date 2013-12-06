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
    \brief Throttling public header */

#ifndef HH_SENF_PPI_Throttling_
#define HH_SENF_PPI_Throttling_ 1

// Custom includes
#include "predecl.hh"

//#include "Queueing.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief Throttling discipline base class

        ThrottlingDiscipline derived classes define the generation of throttling notifications. The
        ThrottlingDiscipline is called whenever the packets are entered or removed from the queue. The
        throttling discipline then determines the new throttling state of the queue.

        \note The ThrottlingDiscipline will \e never drop packets explicitly. This is left to the
        operating system by sending throttling events. The PPI will never loose a packet internally
        (if not a module explicitly does so), however it may disable reception of new incoming
        packets which will then probably be dropped by the operating system.

        \attention Notifications may be forwarded to the ThrottlingDiscipline implementation
            out-of-order: A dequeue event may be notified before the corresponding enqueue
            event (this happens to optimize away transient throttling state changes which would
            otherwise occur if a packet is entered into the queue and then removed from it in the
            same processing step).
     */
    class ThrottlingDiscipline
    {
    public:
        virtual ~ThrottlingDiscipline();

        enum Event { ENQUEUE, DEQUEUE }; ///< Possible queueing events
        enum None_t { NONE };            ///< Type to disable the throttling discipline

        virtual void update(connector::GenericPassiveInput & input, Event event) = 0;
                                        ///< Calculate new throttling state
                                        /**< Whenever the queue is manipulated, this member is
                                             called to calculate the new throttling state. The
                                             member must call \a input's \c throttle() or \c
                                             unthrottle() member to set the new throttling state.

                                             \param[in] input Connector holding the queue
                                             \param[in] event Type of event triggering the update */
    };

    /** \brief Simple throttling discipline with high and low threshold

        The ThresholdThrottling ThrottlingDiscipline is a simple throttling discipline which throttles the
        input as soon the number of packets in the queue reaches the \a high threshold. The input
        will be unthrottled when the number of packets drops to the \a low threshold.

        The default throttling discipline is ThresholdThrottling(1,0).
     */
    class ThresholdThrottling
        : public ThrottlingDiscipline
    {
    public:
        ThresholdThrottling(unsigned high, unsigned low);

        virtual void update(connector::GenericPassiveInput & input, Event event);

    private:
        unsigned high_;
        unsigned low_;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Throttling.cci"
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
