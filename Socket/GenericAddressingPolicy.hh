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

#ifndef HH_GenericAddressingPolicy_
#define HH_GenericAddressingPolicy_ 1

// Custom includes
#include "SocketHandle.hh"
#include "FileHandle.hh"
#include "SocketPolicy.hh"
#include "CommunicationPolicy.hh"

//#include "GenericAddressingPolicy.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {


    struct GenericAddressingPolicy_Base
    {
        static void do_local(FileHandle handle, struct sockaddr * addr, unsigned len);
        static void do_peer(FileHandle handle, struct sockaddr * addr, unsigned len);
        static void do_bind(FileHandle handle, struct sockaddr const * addr, unsigned len);
        static void do_connect(FileHandle handle, struct sockaddr const * addr, unsigned len);
    };

    template <class Address>
    struct GenericAddressingPolicy
        : private GenericAddressingPolicy_Base
    {
        template <class Policy>
        static void peer(SocketHandle<Policy> handle, Address & addr,
                         typename IfCommunicationPolicyIs<Policy,ConnectedCommunicationPolicy>::type * = 0);
        static void local(FileHandle handle, Address & addr);

        template <class Policy>
        static void connect(SocketHandle<Policy> handle, Address const & addr,
                            typename IfCommunicationPolicyIs<Policy,ConnectedCommunicationPolicy>::type * = 0);
        static void bind(FileHandle handle, Address const & addr);
    };

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "GenericAddressingPolicy.cci"
//#include "GenericAddressingPolicy.ct"
#include "GenericAddressingPolicy.cti"
//#include "GenericAddressingPolicy.mpp"
#endif


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
