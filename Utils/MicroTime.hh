// $Id$
//
// Copyright (C) 2006 

#ifndef HH_MicroTime_
#define HH_MicroTime_ 1

// Custom includes
#include <boost/cstdint.hpp>

//#include "MicroTime.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    typedef boost::uint64_t MicroTime;

    MicroTime now();

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "MicroTime.cci"
//#include "MicroTime.ct"
//#include "MicroTime.cti"
//#include "MicroTime.mpp"
#endif


// Local Variables:
// mode: c++
// End:
