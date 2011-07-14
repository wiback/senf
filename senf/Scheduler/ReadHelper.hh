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
    \brief ReadHelper public header */

#ifndef HH_SENF_Scheduler_ReadHelper_
#define HH_SENF_Scheduler_ReadHelper_ 1

// Custom includes
#include <string>
#include <boost/function.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include <senf/Utils/intrusive_refcount.hh>
#include "FdEvent.hh"

//#include "ReadHelper.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {


    /** \brief Asynchronous reading helper

        This class provides a simple asynchronous reading facility. This helper will register with
        the Scheduler and read incoming data. It will collect the data until a specific number of
        bytes has been read or some Predicate evaluated on the data read thus far signals end of
        data.

        The ReadHelper accepts the same flexible file handle interfaces as the Scheduler.

        The callback must take a ReadHelper::ptr argument. Using this ReadHelper instance, the
        callback can access the data read or retrieve state information.

        The ReadHelper separates the data into two parts: data() will return the matched data,
        tail() will return any surplus data already read. If you don't specify a predicate, tail()
        will always be empty (there may however some data be left in the socket input buffer after
        the ReadHelper returns).

        The predicate is any class instance with an <tt>operator(std::string const &)</tt>. This
        operator is called, whenever some data has been read. If the data is not yet complete, the
        predicate must return \c std::string::npos. If the ReadHelper should stop reading more
        data, the predicate must return the number of bytes which are to be considered 'matched'.

        \todo Move all not Handle dependent members to a ReadHandleBase class
        \todo Add an optional <tt>std::string const & tail</tt> argument to the constructors which
            takes the tail() of a previous ReadHelper instance.
     */
    template <class Handle>
    class ReadHelper
        : public senf::intrusive_refcount
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::intrusive_ptr<ReadHelper> ptr; ///< Smart pointer type for this class
        typedef boost::function<void (ptr)> Callback; ///< Callback type

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        static ptr dispatch(Handle handle, std::string::size_type maxSize,
                            Callback callback); ///< Register new ReadHandler instance
                                        /**< The registered Callback will be called after \a maxSize
                                             bytes have been read or EOF or some error is
                                             encountered.
                                             \post The returned ReadHelper instance is registered
                                                 with the Scheduler to handle read events.
                                             \param[in] handle file descriptor or handle providing
                                                 the Handle interface defined above.
                                             \param[in] maxSize maximum number of bytes to read
                                             \param[in] callback callback
                                             \returns Smart pointer to new ReadHelper instance */

        template <class Predicate>
        static ptr dispatch(Handle handle, std::string::size_type maxSize, Predicate const & predicate,
                            Callback callback); ///< Register new ReadHelper instance
                                        /**< The registered Callback will be called after the \a
                                             predicate returns a Value other than \c
                                             std::string::npos, \a maxSize bytes have been read, or
                                             EOF or some error condition is encountered.
                                             \post The returned ReadHelper instance is registered
                                                 with the Scheduler to handle read events
.                                            \param[in] handle file descriptor or handle providing
                                                 the Handle interface defined above.
                                             \param[in] maxSize maximum number of bytes to read
                                             \param[in] predicate predicate to check
                                             \param[in] callback callback
                                             \returns smart pointer to new ReadHelper instance */

        //\}
        //-////////////////////////////////////////////////////////////////////////

        Handle handle() const;          ///< Access the handle object
        std::string::size_type maxSize() const; ///< Return maximum number of bytes to be read

        std::string const & data() const; ///< return data read
        std::string const & tail() const; ///< return data read but not matched by the predicate

        bool complete() const;          ///< Check whether the read has completed successfully
        bool error() const;             ///< Check for error condition
        void throw_error() const;       ///< If an error occurred, throw it

        void revoke();                  ///< Remove the ReadHelper from the scheduler

    protected:

    private:
        struct InternalPredicate;

        ReadHelper(Handle handle, std::string::size_type maxSize,
                   InternalPredicate * predicate, Callback cb);

        static void dispatchProcess(ptr helper, Handle handle, int event);
        void process(Handle handle, int event);
        void done();

        Handle handle_;
        scheduler::FdEvent fde_;
        std::string::size_type maxSize_;
        boost::scoped_ptr<InternalPredicate> predicate_;
        Callback callback_;

        std::string data_;
        std::string tail_;
        int errno_;
        bool complete_;
    };

    /** \brief ReadHelper predicate matching an arbitrary string

        This predicate will terminate the read when the data read matches a given fixed string. All
        data up to and including the string matched is considered to be part of the data() portion,
        everything after the matched string is placed into the tail().

        \see ReadHelper
     */
    struct ReadUntil
    {
        ReadUntil(std::string const & target);
        std::string::size_type operator()(std::string const & data);
        std::string target;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "ReadHelper.cci"
#include "ReadHelper.ct"
#include "ReadHelper.cti"
//#include "ReadHelper.mpp"
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
