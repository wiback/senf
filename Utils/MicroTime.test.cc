// $Id$
//
// Copyright (C) 2006 

// Unit tests

//#include "MicroTime.test.hh"
//#include "MicroTime.test.ih"

// Custom includes
#include "MicroTime.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(microTime)
{
    BOOST_CHECK_NO_THROW( satcom::lib::now() );
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// End:
