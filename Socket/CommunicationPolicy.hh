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

#ifndef HH_CommunicationPolicy_
#define HH_CommunicationPolicy_ 1

// Custom includes
#include "SocketPolicy.hh"
#include "AddressingPolicy.hh"
#include "FileHandle.hh"

//#include "CommunicationPolicy.mpp"
///////////////////////////////hh.p////////////////////////////////////////

struct sockaddr;

namespace satcom {
namespace lib {
    
    template <class Policy> class ServerSocketHandle;

    struct ConnectedCommunicationPolicy : public CommunicationPolicyBase
    {
        static void listen(FileHandle handle, unsigned backlog);
        template <class Policy>
        static int accept(ServerSocketHandle<Policy> handle, 
                          typename ServerSocketHandle<Policy>::Address & address,
                          typename IfAddressingPolicyIsNot<Policy,NoAddressingPolicy>::type * = 0);
    private:
        static int do_accept(FileHandle handle, struct sockaddr * addr, unsigned len);
    };

    struct UnconnectedCommunicationPolicy : public CommunicationPolicyBase
    {};

}}


///////////////////////////////hh.e////////////////////////////////////////
//#include "CommunicationPolicy.cci"
//#include "CommunicationPolicy.ct"
#include "CommunicationPolicy.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
