// $Id$
//
// Copyright (C) 2006 

// Definition of non-inline non-template functions

#include "MicroTime.hh"
//#include "MicroTime.ih"

// Custom includes
#include <sys/time.h>
#include <time.h>
#include <errno.h>

#include "Utils/Exception.hh"

//#include "MicroTime.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ satcom::lib::MicroTime satcom::lib::now()
{
    struct timeval tv;
    if (gettimeofday(&tv,0) < 0)
	throw SystemException(errno);
    return 1000000*MicroTime(tv.tv_sec) + tv.tv_usec;
}


///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "MicroTime.mpp"


// Local Variables:
// mode: c++
// End:
