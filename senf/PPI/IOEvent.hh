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
    \brief IOEvent public header */

#ifndef HH_SENF_PPI_IOEvent_
#define HH_SENF_PPI_IOEvent_ 1

// Custom includes
#include <senf/Scheduler/FdEvent.hh>
#include <senf/Utils/Exception.hh>
#include <senf/Utils/String.hh>
#include "Events.hh"

//#include "IOEvent.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief IOEvent event information

        Information passed to the IOEvent event handler
     */
    struct IOEventInfo
    {
        int events;                     ///< Type of event signaled
                                        /**< The value is a combination of the flags from
                                             IOEvent::EventFlags */
    };

    /** \brief FileHandle based I/O read/write event

        An IOEvent is signaled, whenever the FileHandle \a handle becomes readable or writable. The
        type of event is specified using the \a events mask with values from EventFlags.

        There are two types of flags:

        \li <em>Event flags</em> (\ref Read, \ref Prio, \ref Write) specify the type of event. The
            callback will be called whenever one of the specified events occurs on the filehandle
        \li <em>Error flags</em> (\ref Hup, \ref Err) specify some type of error condition on the
            filehandle. If you specify an error flag when registering the event, the error condition
            will be passed to the callback, otherwise an ErrorException or HangupException will be
            thrown.

        \see IOEventInfo

        \ingroup event_group
      */
    class IOEvent
        : public EventImplementation<IOEventInfo>
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        // This is stupid, however there is no way to import the Scheduler::EventId enum together
        // with the enumeration symbols

        enum EventFlags {
              Read  = scheduler::FdEvent::EV_READ   /**< FileHandle is readable */
            , Prio = scheduler::FdEvent::EV_PRIO    /**< FileHandle priority data is readable */
            , Write = scheduler::FdEvent::EV_WRITE  /**< FileHandle is writable */
            , Hup = scheduler::FdEvent::EV_HUP      /**< Hangup condition on FileHandle */
            , Err = scheduler::FdEvent::EV_ERR      /**< Some other error condition on FileHandle */
        };

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        IOEvent();

        template <class Handle>
        IOEvent(Handle handle, unsigned events);

        //\}
        //-////////////////////////////////////////////////////////////////////////

        template <class Handle>
        void set(Handle handle, unsigned events);

        /** \brief Unhandled error condition */
        struct ErrorException : public senf::SystemException
        { explicit ErrorException(int fd, int code, std::string msg)
            : senf::SystemException("error in senf::ppi::IOEvent callback on file descriptor " + senf::str(fd) + " : " + msg + " : ", code) {} };

        /** \brief Unhandled hangup condition */
        struct HangupException : public senf::Exception
        { HangupException() : senf::Exception("senf::ppi::IOEvent::HangupException") {} };

    private:
        virtual void v_enable();
        virtual void v_disable();

        void cb(int event);

        int fd_;
        scheduler::FdEvent event_;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "IOEvent.cci"
#include "IOEvent.ct"
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
