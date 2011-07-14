// $Id$
//
// Copyright (C) 2006 Stefan Bund <g0dil@berlios.de>
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

// Copyright (C) 2006
/** \file
    \brief WriteHelper public header */

#ifndef HH_SENF_Scheduler_WriteHelper_
#define HH_SENF_Scheduler_WriteHelper_ 1

// Custom includes
#include <string>
#include <boost/intrusive_ptr.hpp>
#include <boost/function.hpp>
#include <senf/Utils/intrusive_refcount.hh>
#include "FdEvent.hh"

//#include "WriteHelper.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Asynchronous writing helper

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
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::intrusive_ptr<WriteHelper> ptr; ///< Smart pointer type for this class
        typedef boost::function<void (ptr)> Callback; ///< Callback type

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        static ptr dispatch(Handle handle, std::string const & data, Callback callback);
                                        ///< Register new WriteHelper instance
                                        /**< The registered callback will be called after all \a
                                             data has been sent or when some error condition is
                                             encountered.
                                             \param[in] handle file descriptor or handle providing
                                                 the Handle interface defined above.
                                             \param[in] data data to send
                                             \param[in] callback callback
                                             \returns smart pointer to new WriteHelper instance */

        //\}
        //-////////////////////////////////////////////////////////////////////////

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
        WriteHelper(Handle handle, std::string const & data, Callback callback);

        static void dispatchProcess(ptr helper, Handle handle,
                                    senf::scheduler::FdEvent::Events event);
        void process(Handle handle, senf::scheduler::FdEvent::Events event);
        void done();

        Handle handle_;
        scheduler::FdEvent fde_;
        mutable std::string data_;
        Callback callback_;

        mutable std::string::iterator offset_;
        int errno_;
    };


}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
