// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

// Definition of non-inline non-template functions

//#include "scheduler.hh"
//#include "scheduler.ih"

// Custom includes
#include <errno..h>
#include <sys/epoll.h>

static const int EPollInitialSize = 16;

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ Scheduler & satcom::lib::Scheduler::instance()
{
    static Scheduler instance;
    return instance;
}

prefix_ satcom::lib::Scheduler::Scheduler()
    : epollFd_(epoll_create(EPollInitialSize))
{
    
}

prefix_ char const * satcom::SystemException::what()
    const throw()
{
    return strerror(this->errno);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
