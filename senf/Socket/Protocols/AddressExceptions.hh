//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
