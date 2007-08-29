// $Id$
//
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
    \brief IOEvent public header */

#ifndef HH_IOEvent_
#define HH_IOEvent_ 1

// Custom includes
#include "Scheduler/Scheduler.hh"
#include "Events.hh"

//#include "IOEvent.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief IOEvent event information

        Information passed to the IOEvent event handler
     */
    struct IOEventInfo
    {
        unsigned events;                ///< Type of event signaled
                                        /**< The value is a combination of the flags from
                                             IOEvent::EventFlags */
    };

    /** \brief FileHandle based I/O read/write event

        An IOEvent is signaled, whenever the FileHandle \a handle becomes readable or writable. The
        type of event is specified using the \a events mask with values from EventFlags.

        \see IOEventInfo

        \ingroup event_group
      */
    class IOEvent
        : public EventImplementation<IOEventInfo>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        // This is stupid, however there is no way to import the Scheduler::EventId enum together
        // with the enumeration symbols

        enum EventFlags { Read  = Scheduler::EV_READ, 
                          Prio = Scheduler::EV_PRIO,
                          Write = Scheduler::EV_WRITE,
                          Hup = Scheduler::EV_HUP,
                          Err = Scheduler::EV_ERR };

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        template <class Handle>
        IOEvent(Handle handle, unsigned events);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        /** \brief Unhandled error condition */
        struct ErrorException : public std::exception
        { virtual char const * what() const throw() 
                { return "senf::ppi::IOEvent::ErrorException"; } };

        /** \brief Unhandled hangup condition */
        struct HangupException : public std::exception
        { virtual char const * what() const throw() 
                { return "senf::ppi::IOEvent::HangupException"; } };

    protected:

    private:
        virtual void v_enable();
         virtual void v_disable();
        
        void cb(int, Scheduler::EventId event);

        int fd_;
        unsigned events_;
    };

    
}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "IOEvent.cci"
//#include "IOEvent.ct"
#include "IOEvent.cti"
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
