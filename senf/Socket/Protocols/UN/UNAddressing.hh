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
    \brief UNAddressing public header */

#ifndef HH_SENF_Socket_Protocols_UN_UNAddressing_
#define HH_SENF_Socket_Protocols_UN_UNAddressing_ 1

// Custom includes
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <senf/Socket/Protocols/BSDAddressingPolicy.hh>
#include <senf/Socket/Protocols/BSDSocketAddress.hh>

//#include "UNAddressing.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Unix domain socket address

        UNSocketAddress wraps the standard sockaddr_un datatype. It provides simple accessor methods
        to access the path.

        \implementation This implementation is based on sockaddr_un.

        \ingroup addr_group
     */
    class UNSocketAddress
        : public BSDSocketAddress
    {
    public:
        static short const addressFamily = AF_UNIX;

        UNSocketAddress();

        explicit UNSocketAddress(std::string const & path);
                                        ///< Construct an address constant from given path

        UNSocketAddress(const UNSocketAddress& other);
        UNSocketAddress& operator=(const UNSocketAddress& other);

        std::string path() const ;      ///< Return path as string
        void path(std::string const & path);

        using BSDSocketAddress::sockaddr_p;
        using BSDSocketAddress::socklen_p;

    private:
        struct sockaddr_un addr_;
    };

    /** \brief Write path to os

        \related UNSocketAddress
     */
    std::ostream & operator<<(std::ostream & os, UNSocketAddress const & addr);

    /// \addtogroup policy_impl_group
    //\{

    /** \brief Addressing policy supporting unix domain addressing

        \par Address Type:
            UNSocketAddress

        This addressing policy implements addressing using unix domain
        addresses.

        The various members are directly imported from
        BSDAddressingPolicyMixin which see for a detailed
        documentation.
     */
    struct UNAddressingPolicy
        : public BSDAddressingPolicy,
          private BSDAddressingPolicyMixin<UNSocketAddress>
    {
        typedef UNSocketAddress Address;

        using BSDAddressingPolicyMixin<UNSocketAddress>::peer;
        using BSDAddressingPolicyMixin<UNSocketAddress>::local;
        using BSDAddressingPolicyMixin<UNSocketAddress>::connect;
        using BSDAddressingPolicyMixin<UNSocketAddress>::bind;
    };

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "UNAddressing.cci"
//#include "UNAddressing.ct"
//#include "UNAddressing.cti"
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
