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
