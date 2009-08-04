// $Id$
//
// Copyright (C) 2008 
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
    \brief FdDispatcher public header */

#ifndef HH_SENF_Scheduler_FdEvent_
#define HH_SENF_Scheduler_FdEvent_ 1

// Custom includes
#include <boost/intrusive/iset_hook.hpp>
#include "../Utils/Exception.hh"
#include "FdManager.hh"
#include "FIFORunner.hh"

//#include "FdEvent.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace scheduler {

    namespace detail {
        struct FdSetTag;
        typedef boost::intrusive::iset_base_hook<FdSetTag> FdSetBase;
        struct FdSetCompare;
        struct FindFd;
        class FdDispatcher;
        class FileDispatcher;
    }

    /** \brief File descriptor event

        The FdEvent class registers a file descriptor for read or write events.

        There are a number of different event types supported for file descriptors. Those are
        specified using a bit mask. Possible events are

        \li \c EV_READ: File descriptor is readable (or at EOF)
        \li \c EV_PRIO: There is out-of-band data available to be read on the file descriptor
        \li \c EV_WRITE: File descriptor is writable

        The callback will be called with one additional argument. This argument is the event mask of
        type int. This mask will tell, which of the registered events are signaled. There are some
        additional flags which can be set when calling the handler callback:
        
        \li \c EV_HUP: The other end has closed the connection
        \li \c EV_ERR: Transport error

        Only a single handler may be registered for any combination of file descriptor and event
        otherwise a DuplicateEventRegistrationException is thrown.
        
        The file descriptor is specified using an arbitrary handle type which supports the \c
        retrieve_filehandle() protocol: There must be a global function \c retrieve_filehandle
        callable with the handle type. This function must return the file descriptor associated with
        the handle. Implementations for integer numbers (explicit file descriptors) and senf socket
        handles are provided.

        The FdEvent class is an implementation of the RAII idiom: The event will be automatically
        unregistered in the FdEvent destructor. The FdEvent instance should be created within the
        same scope or on a scope below where the callback is defined (e.g. if the callback is a
        member function it should be defined as a class member).
     */
    class FdEvent
        : public detail::FIFORunner::TaskInfo,
          public detail::FdSetBase,
          public detail::FdManager::Event
    {
    public: 
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void (int)> Callback;

        enum Events { 
            EV_NONE = 0                             ///< No event
          , EV_READ = detail::FdManager::EV_READ    ///< fd readable (or EOF)
          , EV_PRIO = detail::FdManager::EV_PRIO    ///< OOB data available for read
          , EV_WRITE = detail::FdManager::EV_WRITE  ///< fd writable
          , EV_HUP = detail::FdManager::EV_HUP      ///< remote end closed connection
          , EV_ERR = detail::FdManager::EV_ERR      ///< transport error
          , EV_ALL = (detail::FdManager::EV_READ 
                      | detail::FdManager::EV_WRITE 
                      | detail::FdManager::EV_PRIO) ///< register all events (read, prio and write)
        };

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        template <class Handle>
        FdEvent(std::string const & name, Callback const & cb, Handle const & handle, int events,
                bool initiallyEnabled = true);
                                        ///< Register a file descriptor event
                                        /**< Registers \a cb to be called when any of the \a events
                                             occurs on \a handle. If \a initiallyEnabled is set \c
                                             false, the callback will not be enabled
                                             automatically. Use enable() to do so.
                                             \param[in] name Descriptive event name (purely
                                                 informational)
                                             \param[in] cb Callback to call
                                             \param[in] handle Handle (file descriptor) to watch
                                             \param[in] events Events to watch for (see Events enum)
                                             \param[in] initiallyEnabled if set \c false, do not
                                                 enable callback automatically. */
        FdEvent(std::string const & name, Callback const & cb);
                                        ///< Create a file descriptor event
                                        /**< Creates a file descriptor event for callback \a cb. The
                                             event is initially disabled. Use the other members to
                                             set the missing parameters and enable the event.
                                             \param[in] name Descriptive event name (purely
                                                 informational)
                                             \param[in] cb Callback to call. This callback may \e
                                                 explicitly be set to \c 0 if the value cannot be
                                                 initialized. */
        ~FdEvent();
        
        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void disable();                 ///< Disable event
        void enable();                  ///< Enable event

        FdEvent & action(Callback const & cb); ///< Change event callback

        FdEvent & events(int events);   ///< Change event mask
        FdEvent & addEvents(int events); ///< Add additional events to event mask
        FdEvent & removeEvents(int events); ///< Remove events from event mask
        int events();                   ///< Current event mask
        
        template <class Handle>
        FdEvent & handle(Handle const & handle); ///< Change event file handle

        struct DuplicateEventRegistrationException : public Exception
        { DuplicateEventRegistrationException() : Exception("duplicate fd event registration") {} };

    protected:

    private:
        virtual void signal(int events);
        virtual void v_run();
        virtual char const * v_type() const;
        virtual std::string v_info() const;

        Callback cb_;
        int fd_;
        bool pollable_;
        int events_;
        int signaledEvents_;

        friend class detail::FdSetCompare;
        friend class detail::FindFd;
        friend class detail::FdDispatcher;
        friend class detail::FileDispatcher;
    };

    /** \brief Get file descriptor from handle object

        This function will query the \a handle for it's file descriptor. The real implementation
        must be provided by a freestanding function \c retrieve_filehandle(Handle const & h) within
        the namespace of \a Handle.
     */
    template <class Handle>
    int get_descriptor(Handle const & handle);
}}

///////////////////////////////hh.e////////////////////////////////////////
#include "FdEvent.cci"
#include "FdEvent.ct"
#include "FdEvent.cti"
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
