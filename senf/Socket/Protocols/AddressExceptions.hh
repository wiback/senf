// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fit.fraunhofer.de/license.html
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
//   Stefan Bund <senf@g0dil.de>

/** \file
    \brief AddressExceptions public header */

#ifndef HH_SENF_Socket_Protocols_AddressExceptions_
#define HH_SENF_Socket_Protocols_AddressExceptions_ 1

// Custom includes
#include <senf/Utils/Exception.hh>

//#include "AddressExceptions.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
        AddressSyntaxException(std::string const & addr)
            : AddressException("Invalid address syntax (\"" + addr + "\")") {}
    };

    /** \brief Resolver failure */
    struct UnknownHostnameException : public AddressException
    {
        UnknownHostnameException() : AddressException("failed to resolve hostname") {}
        UnknownHostnameException(std::string const & hostname)
            : AddressException("Failed to resolve hostname (\"" + hostname + "\")") {}
    };

    /** \brief Unknown interface */
    struct UnknownInterfaceException : public AddressException
    {
        UnknownInterfaceException() : AddressException("Unknown interface") {}
        UnknownInterfaceException(std::string const & interface)
            : AddressException("Unknown interface (\"" + interface + "\")") {}
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
