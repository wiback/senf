//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


#ifndef HH_SENF_Socket_SocketPolicy_test_
#define HH_SENF_Socket_SocketPolicy_test_ 1

// Custom includes
#include "SocketPolicy.hh"
#include "FileHandle.hh"

//#include "SocketPolicy.test.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
