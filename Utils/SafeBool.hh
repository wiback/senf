// $Id$
//
// Copyright (C) 2006 

#ifndef HH_SafeBool_
#define HH_SafeBool_ 1

// Custom includes

//#include "SafeBool.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief internal SafeBool base class
	\internal
     */
    class SafeBoolBase 
    {
    protected:
	typedef void (SafeBoolBase::*bool_type)() const;
	void this_type_does_not_support_comparisons() const;

	// Just here to make them protected ...

	SafeBoolBase();
	SafeBoolBase(const SafeBoolBase&);
	SafeBoolBase& operator=(const SafeBoolBase&);
	~SafeBoolBase();
    };

    /** \brief Mixin class for safe boolean conversion support
	
	This is a direct yet simplified copy of a safe bool solution
	by Bjorn Karlsson from
	http://www.artima.com/cppsource/safebool.html

	This mixin provides the client class with safe boolean
	testing. It is a safe replacement for <tt>operator
	bool</tt>. <tt>operator bool</tt> is problematic since \c bool
	is an integer type. This conversion operator makes the class
	usable in any numeric context, which can be quite
	dangerous. The <tt>operator void *</tt> solution is much
	better in this respect but still allows two instances of any
	class having such a <tt>void *</tt> conversion to be compared
	for equality. This again will produce absolutely unexpected
	results since it will not check wethere the objects are
	identical, it will only check, that both return the same
	boolean state.

	This solutions solves all these problems by returning a
	pointer-to-member which cannot be converted to any other
	type. By providing explicit implementations of \c operator==
	and \c operator!= which fail in an obvious way at compile
	time, this hazard is removed.

	To make a class boolean testable, just inherit from the mixin
	and implement \c boolean_test:
	
	\code
        class Testable 
            : public SafeBool<Testable> 
        {
        public:
            bool boolean_test() const 
            {
                // Perform Boolean logic here
            }
        };

	Testable t = ...;

	if (t) {
	   ...
	}
	\endcode

	\todo Either rename intrusive_refcount to IntrusiveRefcount or
	SafeBool to safe_bool (I tend to the latter ...)
     */
    template <typename T> 
    class SafeBool 
	: public SafeBoolBase 
    {
    public:
	operator bool_type() const;
	bool operator !() const;

    protected:
	~SafeBool();
    };

    template <typename T, typename U> 
    void operator==(const SafeBool<T>& lhs,const SafeBool<U>& rhs);

    template <typename T,typename U> 
    void operator!=(const SafeBool<T>& lhs,const SafeBool<U>& rhs);

}

///////////////////////////////hh.e////////////////////////////////////////
#include "SafeBool.cci"
//#include "SafeBool.ct"
#include "SafeBool.cti"
//#include "SafeBool.mpp"
#endif


// Local Variables:
// mode: c++
// End:
