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

#ifndef HH_SocketProtocol_test_
#define HH_SocketProtocol_test_ 1

// Custom includes
#include "SocketProtocol.hh"
#include "SocketPolicy.test.hh"
#include "ProtocolClientSocketHandle.hh"
#include "../Utils/Logger/SenfLog.hh"

//#include "SocketProtocol.test.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace test {

    class SomeProtocol
        : public ConcreteSocketProtocol<SomeSocketPolicy,SomeProtocol>
    {
    public:
        ~SomeProtocol() {}

        void init_client() const { fd(0); }
        void init_server() const { fd(0); }

        unsigned available() const
            { return Policy::ReadPolicy::TEST_SIZE; }
        bool eof() const
            { return false; }

        virtual void close() const {
            SENF_LOG(( "Closing socket ..." ));
            closeCount(1);
        }

        static unsigned closeCount(unsigned inc=0) {
            static unsigned counter (0);
            counter += inc;
            return counter;
        }
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "SocketProtocol.test.cci"
//#include "SocketProtocol.test.ct"
//#include "SocketProtocol.test.cti"
//#include "SocketProtocol.test.mpp"
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
