// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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

#ifndef HH_SocketPolicy_test_
#define HH_SocketPolicy_test_ 1

// Custom includes
#include "SocketPolicy.hh"
#include "FileHandle.hh"

//#include "SocketPolicy.test.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

namespace test {

    struct SomeAddressingPolicy : public senf::AddressingPolicyBase
    {
        typedef unsigned Address;

        static void peer(FileHandle handle, unsigned & addr)
            { addr=1; }
        static void local(FileHandle, unsigned & addr)
            { addr=2; }
        static void connect(FileHandle, unsigned addr)
            {}
        static void bind(FileHandle, unsigned addr)
            {}
    };

    struct SomeFramingPolicy : public senf::FramingPolicyBase
    {};

    struct SomeCommunicationPolicy : public senf::CommunicationPolicyBase
    {
        static int accept(FileHandle handle, unsigned & addr)
            { addr = 3; return -1; }
        static int accept(FileHandle handle)
            { return -1; }
    };

    struct SomeReadPolicy : public senf::ReadPolicyBase
    {
        static unsigned const TEST_SIZE = 9;

        static unsigned read(FileHandle handle, char * buffer, unsigned size)
            {
                int const n (size<TEST_SIZE?size:TEST_SIZE);
                ::memcpy(buffer,"TEST-READ",n);
                return n;
            }

        static unsigned readfrom(FileHandle handle, char * buffer, unsigned size,
                                 unsigned & address)
            {
                return read(handle,buffer,size);
            }
    };

    struct SomeWritePolicy : public senf::WritePolicyBase
    {
        static unsigned write(FileHandle handle, char const * buffer, unsigned size)
            {
                if (size == 10 && ::strncmp(buffer,"TEST-WRITE",10) == 0)
                    return size;
                else
                    return 0;
            }

        static unsigned writeto(FileHandle handle, unsigned address,
                                char const * buffer, unsigned size)
            {
                return write(handle,buffer,size);
            }
    };

    typedef senf::MakeSocketPolicy<
        SomeAddressingPolicy,
        SomeFramingPolicy,
        SomeCommunicationPolicy,
        SomeReadPolicy,
        SomeWritePolicy
        >::policy SomeSocketPolicy;

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "SocketPolicy.test.cci"
//#include "SocketPolicy.test.ct"
//#include "SocketPolicy.test.cti"
//#include "SocketPolicy.test.mpp"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
