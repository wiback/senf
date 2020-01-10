//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief FileHandle unit tests */

#include "FileHandle.hh"

// Custom includes
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    class FHandle : public senf::FileHandle
    {
    public:
        FHandle() {}
        FHandle(int fd)
            : senf::FileHandle(std::unique_ptr<senf::FileBody>(new senf::FileBody(fd))) {}
        FHandle(std::string const & name)
            : senf::FileHandle(std::unique_ptr<senf::FileBody>(new senf::FileBody()))
            {
                int rv = ::open(name.c_str(),O_RDWR|O_NONBLOCK) ;
                if (rv<0)
                    throw senf::SystemException();
                fd(rv);
            }
    };
}

SENF_AUTO_TEST_CASE(fileHandle)
{
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
    } {
        FHandle fh("/dev/zero");
        BOOST_CHECK(fh.readable());
    } {
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
