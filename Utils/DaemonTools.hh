// $Id$
//
// Copyright (C) 2006 

#ifndef HH_DaemonTools_
#define HH_DaemonTools_ 1

// Custom includes
#include <string>

//#include "DaemonTools.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {


    void daemonize();
    void redirect_stdio(std::string const & path = "/dev/null");

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "DaemonTools.cci"
//#include "DaemonTools.ct"
//#include "DaemonTools.cti"
//#include "DaemonTools.mpp"
#endif


// Local Variables:
// mode: c++
// End:
