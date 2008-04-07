// $Id$
//
// Copyright (C) 2008 
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
    \brief Binding public header */

#ifndef HH_Binding_
#define HH_Binding_ 1

// Custom includes
#include <boost/utility.hpp>
#include "Scheduler.hh"

//#include "Binding.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Manage scheduler handle binding

        This class will manage the binding of an arbitrary handle to the scheduler: The handle will
        automatically be removed from the Scheduler when this instance is destroyed. Example using a
        SocketHandle instance for the handle:
        \code
        class Foo
        {
        public:
            Foo(SocketHandle handle) : binding_ (handle, senf::membind(&cb, this),
                                                 senf::Scheduler::EV_READ) {}

            void blarf() { binding_.disable(); }

        private:
           void cb(Scheduler::EventId event);

           senf::SchedulerBinding binding_;
        };
        \endcode

        The handle will be registered automatically in the constructor and will be unregistered in
        the destructor. Additionally, the helper has enable() and disable() members to register or
        remove the handle to/from the Scheduler.
     */
    class SchedulerBinding
        : boost::noncopyable
    {
    public:
        template <class Handle>
        SchedulerBinding(Handle const & handle, Scheduler::FdCallback cb, 
                         int eventMask = Scheduler::EV_ALL, bool enabled = true);
                                        ///< Register handle with the Scheduler
                                        /**< This constructor is like calling Scheduler::add()
                                             unless \a enabled is \c false, in which case the handle
                                             is \e not initially registered (it may be registered by
                                             caling enable() 
                                             \param[in] handle Handle to register
                                             \param[in] cb Callback
                                             \param[in] eventMask type of events to register for
                                             \param[in] enabled wether to add handle to Scheduler
                                                 automatically  */

        ~SchedulerBinding();            ///< Remove scheduler registration
        
        void enable();                  ///< Add handle to Scheduler
                                        /**< Adds the handle if it is not already registered */
        void disable();                 ///< Remove handle from Scheduler
                                        /**< Remove handle from Scheduler if registered */

        bool enabled();                 ///< \c true, if handle is registered

    protected:

    private:
        int fd_;
        Scheduler::FdCallback cb_;
        int eventMask_;
        bool enabled_;
    };
}

///////////////////////////////hh.e////////////////////////////////////////
#include "Binding.cci"
//#include "Binding.ct"
#include "Binding.cti"
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
