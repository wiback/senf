// $Id$
//
// Copyright (C) 2006 

#ifndef HH_SafeBool_
#define HH_SafeBool_ 1

// Custom includes

//#include "SafeBool.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {
    
    // This is a direct copy of a safe bool solution by Bjorn Karlsson 
    // from http://www.artima.com/cppsource/safebool.html
    //
    // Usage:
    //    class TestableWithVirtual 
    //        : public safe_bool<> 
    //    {
    //    protected:
    //        bool boolean_test() const 
    //        {
    //            // Perform Boolean logic here
    //        }
    //    };
    //
    //    class TestableWithoutVirtual 
    //        : public safe_bool <TestableWithoutVirtual> 
    //    {
    //    public:
    //        bool boolean_test() const 
    //        {
    //            // Perform Boolean logic here
    //        }
    //    };

    class SafeBoolBase 
    {
    protected:
	typedef void (SafeBoolBase::*bool_type)() const;
	void this_type_does_not_support_comparisons() const;

	SafeBoolBase();
	SafeBoolBase(const SafeBoolBase&);
	SafeBoolBase& operator=(const SafeBoolBase&);
	~SafeBoolBase();
    };

    template <typename T=void> 
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

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "SafeBool.cci"
//#include "SafeBool.ct"
#include "SafeBool.cti"
//#include "SafeBool.mpp"
#endif


// Local Variables:
// mode: c++
// End:
