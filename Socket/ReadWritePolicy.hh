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

/** \file

    \todo ReadWritePolicy.test.cc 
 */

#ifndef HH_ReadWritePolicy_
#define HH_ReadWritePolicy_ 1

// Custom includes
#include "SocketPolicy.hh"
#include "ClientSocketHandle.hh"
#include "CommunicationPolicy.hh" 

//#include "ReadWritePolicy.mpp"
///////////////////////////////hh.p////////////////////////////////////////


struct sockaddr;

namespace senf {


    struct ReadablePolicy : public ReadPolicyBase
    {
        static unsigned read(FileHandle handle, char * buffer, unsigned size);
        template <class Policy>
        static unsigned readfrom(ClientSocketHandle<Policy> handle, char * buffer, unsigned size,
                                 typename Policy::AddressingPolicy::Address & address,
                                 typename IfCommunicationPolicyIs<Policy,UnconnectedCommunicationPolicy>::type * = 0);

    private:
        static unsigned do_readfrom(FileHandle handle, char * buffer, unsigned size,
                                    struct ::sockaddr * addr, socklen_t len);
    };

    struct NotReadablePolicy : public ReadPolicyBase
    {};

    struct WriteablePolicy : public WritePolicyBase
    {
        template <class Policy>
        static unsigned write(ClientSocketHandle<Policy> handle, char const * buffer, unsigned size,
                              typename IfCommunicationPolicyIs<Policy,ConnectedCommunicationPolicy>::type * = 0);
        template <class Policy>
        static unsigned writeto(ClientSocketHandle<Policy> handle, 
                                typename boost::call_traits<typename Policy::AddressingPolicy::Address>::param_type addr,
                                char const * buffer, unsigned size,
                                typename IfCommunicationPolicyIs<Policy,UnconnectedCommunicationPolicy>::type * = 0);

    private:
        static unsigned do_write(FileHandle handle, char const * buffer, unsigned size);
        static unsigned do_writeto(FileHandle handle, char const * buffer, unsigned size,
                                   struct sockaddr * addr, socklen_t len);
    };
    
    struct NotWriteablePolicy : public WritePolicyBase
    {};

}


///////////////////////////////hh.e////////////////////////////////////////
//#include "ReadWritePolicy.cci"
//#include "ReadWritePolicy.ct"
#include "ReadWritePolicy.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
