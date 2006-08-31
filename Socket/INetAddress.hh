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

#ifndef HH_INetAddress_
#define HH_INetAddress_ 1

// Custom includes
#include <string>
#include <exception>
#include <netinet/in.h>

//#include "INetAddress.mpp"
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
        
        struct ::sockaddr_in addr;
    };

    std::ostream & operator<<(std::ostream & os, INet4Address const & addr);

    class INet6Address
    {
        // TODO: Implement
    };
    
    struct InvalidINetAddressException : public std::exception
    { char const * what() const throw() { return "invalid inet address"; } };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "INetAddress.cci"
//#include "INetAddress.ct"
//#include "INetAddress.cti"
//#include "INetAddress.mpp"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
