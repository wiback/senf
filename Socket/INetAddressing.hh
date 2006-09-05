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

#ifndef HH_INetAddressing_
#define HH_INetAddressing_ 1

// Custom includes
#include <string>
#include <exception>
#include <netinet/in.h>
#include "SocketPolicy.hh"
#include "ClientSocketHandle.hh"
#include "CommunicationPolicy.hh"

//#include "INetAddressing.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    class INet4Address
    {
    public:
        INet4Address();
        INet4Address(char const * address);
        INet4Address(std::string address);
        INet4Address(std::string host, unsigned port);

        bool operator==(INet4Address const & other) const;

        std::string str() const;
        std::string host() const;
        unsigned port() const;

        // TODO: Interface

        void clear();

        struct sockaddr * sockaddr_p();
        struct sockaddr const * sockaddr_p() const;
        unsigned sockaddr_len() const;

    private:
        void assignString(std::string addr);
        
        struct ::sockaddr_in addr_;
    };

    std::ostream & operator<<(std::ostream & os, INet4Address const & addr);

    class INet6Address
    {
        // TODO: Implement
    };
    
    struct INet4AddressingPolicy : public AddressingPolicyBase
    {
        typedef INet4Address Address;

        template <class Policy>
        static void peer(ClientSocketHandle<Policy> handle, Address & addr,
                         typename IfCommunicationPolicyIs<Policy,ConnectedCommunicationPolicy>::type * = 0);
        static void local(FileHandle handle, Address & addr);

        template <class Policy>
        static void connect(ClientSocketHandle<Policy> handle, Address const & addr,
                            typename IfCommunicationPolicyIs<Policy,ConnectedCommunicationPolicy>::type * = 0);
        static void bind(FileHandle handle, Address const & addr);

    private:
        static void do_peer(FileHandle handle, Address & addr);
        static void do_connect(FileHandle handle, Address const & addr);
    };

    struct INet6AddressingPolicy : public AddressingPolicyBase
    {
        typedef INet6Address Address;

        // TODO: Implement
    };

    struct InvalidINetAddressException : public std::exception
    { char const * what() const throw() { return "invalid inet address"; } };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "INetAddressing.cci"
//#include "INetAddressing.ct"
//#include "INetAddressing.cti"
//#include "INetAddressing.mpp"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
