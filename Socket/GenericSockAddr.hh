// $Id$
//
// Copyright (C) 2006 

#ifndef HH_GenericSockAddr_
#define HH_GenericSockAddr_ 1

// Custom includes
#include <sys/socket.h>

//#include "GenericSockAddr.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    class GenericSockAddr
    {
    public:
	GenericSockAddr();
	
	struct sockaddr * sockaddr_p();
	struct sockaddr const * sockaddr_p() const;
	unsigned sockaddr_len() const;
	
    private:
	struct ::sockaddr_storage addr_;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "GenericSockAddr.cci"
//#include "GenericSockAddr.ct"
//#include "GenericSockAddr.cti"
//#include "GenericSockAddr.mpp"
#endif


// Local Variables:
// mode: c++
// End:
