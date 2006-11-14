// $Id$
//
// Copyright (C) 2006 

// Definition of non-inline non-template functions

#include "DaemonTools.hh"
//#include "DaemonTools.ih"

// Custom includes
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "Exception.hh"

//#include "DaemonTools.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void satcom::lib::daemonize()
{
    int pid = fork();
    if (pid < 0)
	throw satcom::lib::SystemException("fork",errno);
    if (pid > 0)
	::_exit(0);
    if (::setsid() < 0)
	throw satcom::lib::SystemException("setsid",errno);
}

prefix_ void satcom::lib::redirect_stdio(std::string const & path)
{
    int fd = ::open(path.c_str(),O_RDWR);
    if (fd < 0) throw satcom::lib::SystemException("open",errno);
    if (dup2(fd,0) < 0) throw satcom::lib::SystemException("dup2",errno);
    if (dup2(fd,1) < 0) throw satcom::lib::SystemException("dup2",errno);
    if (dup2(fd,2) < 0) throw satcom::lib::SystemException("dup2",errno);
    if (::close(fd) < 0) throw satcom::lib::SystemException("close",errno);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "DaemonTools.mpp"


// Local Variables:
// mode: c++
// End:
