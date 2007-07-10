// $Id$
//
// Copyright (C) 2006 Stefan Bund <g0dil@berlios.de>
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
// Copyright (C) 2006

/** \file
    \brief WriteHelper public header */

#ifndef HH_WriteHelper_
#define HH_WriteHelper_ 1

// Custom includes
#include <string>
#include <boost/intrusive_ptr.hpp>
#include <boost/function.hpp>
#include "Utils/intrusive_refcount.hh"
#include "Scheduler.hh"

//#include "WriteHelper.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Asyncronous writing helper

        This class provides a simple asynchronous writing facility. This helper will register with
        the Scheduler to write the requested data. It will stay registered until the data has been
        completely sent or some error condition is encountered. As soon as the WriteHelper is done,
        the callback will be called.

        The WriteHelper accepts the same flexible file handle interfaces as the Scheduler.

        The callback must take a WriteHelper::ptr argument. Using this WriteHelper instance, the
        callback can access the state information and check the termination status.

        \todo Add additional interface to better access the intermediate status (data sent so far)
     */
    template <class Handle>
    class WriteHelper
        : public senf::intrusive_refcount
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::intrusive_ptr<WriteHelper> ptr; ///< Smart pointer type for this class
        typedef boost::function<void (ptr)> Callback; ///< Callback type

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        static ptr dispatch(Handle handle, std::string data, Callback callback);
                                        ///< Register new WriteHelper instance
                                        /**< The registered callback will be called after all \a
                                             data has been sent or when some error condition is
                                             encountered.
                                             \param[in] handle file descriptor or handle providing
                                                 the Handle interface defined above.
                                             \param[in] data data to send
                                             \param[in] callback callback
                                             \returns smart pointer to new WriteHelper instance */

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        Handle handle() const;

        std::string const & data() const; ///< Return the data
                                        /**< After all data has been sent, this member will return
                                             an empty string. Until then, the complete string will
                                             be returned. */

        bool complete() const;          ///< Check whether the write has completed successfully
        bool error() const;             ///< Check for error condition
        void throw_error() const;       ///< If an error occurred, throw it

        void revoke();                  ///< Remove the WriteHelper from the scheduler

    protected:

    private:
        WriteHelper(Handle handle, std::string data, Callback callback);

        static void dispatchProcess(ptr helper, Handle handle, senf::Scheduler::EventId event);
        void process(Handle handle, senf::Scheduler::EventId event);
        void done();

        Handle handle_;
        mutable std::string data_;
        Callback callback_;

        mutable std::string::iterator offset_;
        int errno_;
    };


}

///////////////////////////////hh.e////////////////////////////////////////
//#include "WriteHelper.cci"
#include "WriteHelper.ct"
#include "WriteHelper.cti"
//#include "WriteHelper.mpp"
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
