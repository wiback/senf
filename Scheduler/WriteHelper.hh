// $Id$
//
// Copyright (C) 2006 

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


    template <class Handle>
    class WriteHelper
	: public senf::intrusive_refcount
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

	typedef boost::intrusive_ptr<WriteHelper> ptr;
	typedef boost::function<void (ptr)> Callback;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

	static ptr dispatch(Handle handle, std::string data, Callback callback);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

	Handle handle() const;

	std::string const & data() const;

	bool complete() const;
	bool error() const;
	void throw_error() const;

	void revoke();

    protected:

    private:
	WriteHelper(Handle handle, std::string data, Callback callback);

	static void dispatchProcess(ptr helper, Handle handle, senf::Scheduler::EventId event);
	void process(Handle handle, senf::Scheduler::EventId event);
	void done();

	Handle handle_;
	mutable std::string data_;
	Callback callback_;

	mutable std::string::size_type offset_;
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
// End:
