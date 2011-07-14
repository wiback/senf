// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief FileHandle unit tests */

//#include "FileHandle.test.hh"
//#include "FileHandle.test.ih"

// Custom includes
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include "FileHandle.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    class FHandle : public senf::FileHandle
    {
    public:
        FHandle() {}
        FHandle(int fd)
            : senf::FileHandle(std::auto_ptr<senf::FileBody>(
                                          new senf::FileBody(fd))) {}
        FHandle(std::string const & name)
            : senf::FileHandle(std::auto_ptr<senf::FileBody>(
                                          new senf::FileBody()))
            {
                int rv = ::open(name.c_str(),O_RDWR|O_NONBLOCK) ;
                if (rv<0)
                    throw senf::SystemException();
                fd(rv);
            }
    };
}

SENF_AUTO_UNIT_TEST(fileHandle)
{
    try {
        {
            FHandle fh("/dev/null");
            BOOST_CHECK(fh.fd() != -1);
            BOOST_CHECK(fh.valid());
            BOOST_CHECK(fh);
            BOOST_CHECK(!!fh);

            FHandle fh2;
            BOOST_CHECK( ! fh2.valid() );
            fh2 = fh;
            BOOST_CHECK_EQUAL(fh.fd(), fh2.fd());

            BOOST_CHECK(fh.writeable());
            SENF_CHECK_NO_THROW(fh.close());
            BOOST_CHECK_THROW(fh.close(),senf::SystemException);
            SENF_CHECK_NO_THROW(fh.terminate());
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
            ::memset(buffer, 0, sizeof(buffer));
            while (write(fds[1],buffer,1024) == 1024) ;

            FHandle fh2(fds[1]);
            BOOST_CHECK(!fh.writeable());
        }
    }
    catch (std::exception const & ex) {
        BOOST_FAIL(ex.what());
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
