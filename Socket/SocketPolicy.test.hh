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

#ifndef HH_SocketPolicy_test_
#define HH_SocketPolicy_test_ 1

// Custom includes
#include "SocketPolicy.hh"
#include "FileHandle.hh"

//#include "SocketPolicy.test.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {
namespace test {

    struct SomeAddressingPolicy : public satcom::lib::AddressingPolicyBase
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

    struct SomeFramingPolicy : public satcom::lib::FramingPolicyBase
    {};

    struct SomeCommunicationPolicy : public satcom::lib::CommunicationPolicyBase
    {
        static int accept(FileHandle handle, unsigned & addr)
            { addr = 3; return -1; }
    };

    struct SomeReadPolicy : public satcom::lib::ReadPolicyBase
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

    struct SomeWritePolicy : public satcom::lib::WritePolicyBase
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
    
    struct SomeBufferingPolicy : public satcom::lib::BufferingPolicyBase
    {
        static unsigned rcvbuf(FileHandle handle)
            { return 0; }
        static unsigned rcvbuf(FileHandle handle, unsigned size)
            { return 0; }

        static unsigned sndbuf(FileHandle handle)
            { return 0; }
        static unsigned sndbuf(FileHandle handle, unsigned size)
            { return 0; }
    };

    typedef satcom::lib::MakeSocketPolicy<
        SomeAddressingPolicy,
        SomeFramingPolicy,
        SomeCommunicationPolicy,
        SomeReadPolicy,
        SomeWritePolicy,
        SomeBufferingPolicy
        >::policy SomeSocketPolicy;

}}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "SocketPolicy.test.cci"
//#include "SocketPolicy.test.ct"
//#include "SocketPolicy.test.cti"
//#include "SocketPolicy.test.mpp"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
