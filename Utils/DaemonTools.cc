// $Id$
//
// Copyright (C) 2006 Stefan Bund <g0dil@senf.berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief DaemonTools  non-inline non-template implementation */

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

prefix_ void senf::daemonize()
{
    int pid = fork();
    if (pid < 0)
	throw senf::SystemException("fork",errno);
    if (pid > 0)
	::_exit(0);
    if (::setsid() < 0)
	throw senf::SystemException("setsid",errno);
}

prefix_ void senf::redirect_stdio(std::string const & path)
{
    int fd = ::open(path.c_str(),O_RDWR);
    if (fd < 0) throw senf::SystemException("open",errno);
    if (dup2(fd,0) < 0) throw senf::SystemException("dup2",errno);
    if (dup2(fd,1) < 0) throw senf::SystemException("dup2",errno);
    if (dup2(fd,2) < 0) throw senf::SystemException("dup2",errno);
    if (::close(fd) < 0) throw senf::SystemException("close",errno);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "DaemonTools.mpp"


// Local Variables:
// mode: c++
// End:
