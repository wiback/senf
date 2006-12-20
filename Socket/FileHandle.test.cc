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

// Unit tests

//#include "FileHandle.test.hh"
//#include "FileHandle.test.ih"

// Custom includes
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include "FileHandle.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    class FHandle : public senf::FileHandle
    {
    public:
        FHandle(int fd=-1) 
            : senf::FileHandle(std::auto_ptr<senf::FileBody>(
                                          new senf::FileBody(fd))) {}
        FHandle(std::string name) 
            : senf::FileHandle(std::auto_ptr<senf::FileBody>(
                                          new senf::FileBody()))
            {
                int rv = ::open(name.c_str(),O_RDWR|O_NONBLOCK) ;
                if (rv<0)
                    throw senf::SystemException(errno);
                fd(rv);
            }
    };
}

BOOST_AUTO_UNIT_TEST(fileHandle)
{
    try {
        {
            FHandle fh("/dev/null");
            BOOST_CHECK(fh.fd() != -1);
            BOOST_CHECK(fh.valid());
            BOOST_CHECK(fh);
            BOOST_CHECK(!!fh);

            FHandle fh2(fh);
            BOOST_CHECK_EQUAL(fh.fd(), fh2.fd());
            
            BOOST_CHECK(fh.writeable());
            BOOST_CHECK_NO_THROW(fh.close());
            BOOST_CHECK_THROW(fh.close(),senf::SystemException);
            BOOST_CHECK_NO_THROW(fh.terminate());
        }
        
        {
            FHandle fh("/dev/zero");
            BOOST_CHECK(fh.readable());
        }

        {
            int fds[2];
            BOOST_REQUIRE(pipe(fds) == 0);

            FHandle fh(fds[0]);
            BOOST_CHECK(!fh.readable());

            // Set non-blocking IO and fill the pipe buffer
            int flags = fcntl(fds[1],F_GETFL,0);
            if (flags == -1)
                BOOST_FAIL(strerror(errno));
            if (fcntl(fds[1],F_SETFL,flags|O_NONBLOCK) == -1)
                BOOST_FAIL(strerror(errno));
            char buffer[1024];
            while (write(fds[1],buffer,1024) == 1024) ;

            FHandle fh2(fds[1]);
            BOOST_CHECK(!fh.writeable());
        }
    }
    catch (std::exception const & ex) {
        BOOST_FAIL(ex.what());
    }
}
    
///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
