// $Id$
//
// Copyright (C) 2006 

// TODO: Move all not Handle dependent members to a ReadHandleBase class

#ifndef HH_ReadHelper_
#define HH_ReadHelper_ 1

// Custom includes
#include <string>
#include <boost/function.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "Utils/intrusive_refcount.hh"
#include "Scheduler.hh"

//#include "ReadHelper.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    template <class Handle>
    class ReadHelper
	: public satcom::lib::intrusive_refcount
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types
	
	typedef boost::intrusive_ptr<ReadHelper> ptr;
	typedef boost::function<void (ptr)> Callback;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

	static ptr dispatch(Handle handle, std::string::size_type maxSize, 
			    Callback callback);

	template <class Predicate>
	static ptr dispatch(Handle handle, std::string::size_type maxSize, Predicate predicate,
			    Callback callback);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

	Handle handle() const;
	unsigned maxSize() const;

	std::string const & data() const;
	std::string const & tail() const;

	bool complete() const;
	bool error() const;
	void throw_error() const;

	void revoke();

    protected:

    private:
	struct InternalPredicate;

        ReadHelper(Handle handle, unsigned maxSize,  InternalPredicate * predicate, Callback cb);

	static void dispatchProcess(ptr helper, Handle handle, satcom::lib::Scheduler::EventId event);
	void process(Handle handle, satcom::lib::Scheduler::EventId event);
	void done();

	Handle handle_;
	std::string::size_type maxSize_;
	boost::scoped_ptr<InternalPredicate> predicate_;
	Callback callback_;

	std::string data_;
	std::string tail_;
	int errno_;
	bool complete_;
    };

    struct ReadUntil
    {
        ReadUntil(std::string target);
	std::string::size_type operator()(std::string data);
	std::string target;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "ReadHelper.cci"
#include "ReadHelper.ct"
#include "ReadHelper.cti"
//#include "ReadHelper.mpp"
#endif


// Local Variables:
// mode: c++
// End:
