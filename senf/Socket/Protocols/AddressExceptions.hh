// $Id$
//
// Copyright (C) 2008 
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

/** \file
    \brief AddressExceptions public header */

#ifndef HH_SENF_Socket_Protocols_AddressExceptions_
#define HH_SENF_Socket_Protocols_AddressExceptions_ 1

// Custom includes
#include <senf/Utils/Exception.hh>

//#include "AddressExceptions.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Base-class for Address exceptions */
    struct AddressException : public senf::Exception
    {
    protected:
        AddressException(std::string const & msg)
            : senf::Exception(msg) {}
    };

    /** \brief Invalid address syntax */
    struct AddressSyntaxException : public AddressException
    {
        AddressSyntaxException() : AddressException("invalid address syntax") {}
        AddressSyntaxException(const std::string &addr)
            : AddressException("Invalid address syntax (\"" + addr + "\")") {}
    };

    /** \brief Resolver failure */
    struct UnknownHostnameException : public AddressException
    { 
        UnknownHostnameException() : AddressException("failed to resolve hostname") {} 
        UnknownHostnameException(const std::string &hostname)
            : AddressException("Failed to resolve hostname (\"" + hostname + "\")") {}
    };

}        

///////////////////////////////hh.e////////////////////////////////////////
//#include "AddressExceptions.cci"
//#include "AddressExceptions.ct"
//#include "AddressExceptions.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
