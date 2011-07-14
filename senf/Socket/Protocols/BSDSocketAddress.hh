// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief BSDSocketAddress public header */

#ifndef HH_SENF_Socket_Protocols_BSDSocketAddress_
#define HH_SENF_Socket_Protocols_BSDSocketAddress_ 1

// Custom includes
#include <boost/type_traits/alignment_of.hpp>
#include <boost/type_traits/type_with_alignment.hpp>
#include <boost/operators.hpp>
#include <senf/Utils/safe_bool.hh>
#include <sys/socket.h>
#include <iostream>

//#include "BSDSocketAddress.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Socket addressing, BSD style

        BSDSocketAddress is the base class of all BSD \c sockaddr based addressing classes. The \c
        sockaddr addressing interface is split into several parts

        \li The BSDSocketAddress provides a read-only and generic \c sockaddr interface
        \li Address family specific derived classes implement addressing of a specific type. These
            are INet4SocketAddress (\c AF_INET), INet6SocketAddress (\c AF_INET6), UNSocketAddress
            (\c AF_UNIX) and LLSocketAddress (\c AF_PACKET)
        \li GenericBSDSocketAddress provides writable support for generic addresses.

        It is \e not possible to create or store BSDSocketAddress instances: You must either store
        an address in one of the specifically typed subclasses or using GenericBSDSocketAddress.

        A BSDSocketAddress or GenericBSDSocketAddress can be cast (like a downcast) to (the correct)
        type specific cast using sockaddr_cast:

        \code
        void foo(senf::BSDSOcketAddress const & addr)
        {
            if (addr.family() == senf::INet4SocketAddress::addressFamily) {
                senf::INet4SocketAddress i4addr (
                    senf::sockaddr_cast<senf::INet4SocketAddress>(addr) );
                ...
            }
        }
        \endcode

        All these classes provide a generic \c sockaddr API to interface with legacy \c sockaddr
        based code (e.g. the BSD socket API). In this base-class, this interface is read-only, the
        derived classes however provide a read-write interface.

        \ingroup addr_group
      */
    class BSDSocketAddress
        : public senf::comparable_safe_bool<BSDSocketAddress>,
          public boost::less_than_comparable<BSDSocketAddress>,
          public boost::equality_comparable<BSDSocketAddress>
    {
    public:
        bool operator==(BSDSocketAddress const & other) const; ///< Compare two arbitrary addresses
                                        /**< For addresses to be considered equal, they must have
                                             the same family, length and the data must be
                                             identical. */
        bool operator<(BSDSocketAddress const & other) const; ///< Compare two arbitrary addresses
                                        /**< Ordering is based on the in-memory representation.  It
                                             is primarily useful to use addresses as keys in a map
                                             or set. */

        bool boolean_test() const;      ///< Return \c true, if address is not empty
                                        /**< An address is considered empty if
                                             \li the family is AF_UNSPEC
                                             \li or the size is 0
                                             \li or all data bytes are 0 */

        short family() const;           ///< Return the address family.
                                        /**< This value is found in the \c addressFamily member of
                                             each typed derived class
                                             (e.g. INet4Address::addressFamily) */

        //-////////////////////////////////////////////////////////////////////////
        ///\name Generic sockaddr interface
        //\{

        struct sockaddr const * sockaddr_p() const;
        socklen_t socklen() const;
        socklen_t const * socklen_p() const;

        //\}

    protected:
        BSDSocketAddress(socklen_t len, short family);
        BSDSocketAddress(BSDSocketAddress const & other);
        BSDSocketAddress & operator=(BSDSocketAddress const & other);

        struct sockaddr * sockaddr_p();
        socklen_t * socklen_p();

        void socklen(socklen_t len);

    private:
        // The following incantation is needed to fix the alignment of the sockaddr data members
        // which will be added by the derived classes later: The alignment must be forced
        // to coincide with the struct sockaddr_storage alignment (which must have the largest
        // alignment of all sockaddr types).
        union {
            socklen_t len_;
            boost::type_with_alignment<boost::alignment_of<struct sockaddr_storage>::value> a_;
            char _b[boost::alignment_of<struct sockaddr_storage>::value];
        };
    };

    /** \brief Safe socket address down-cast

        sockaddr_cast allows to safely cast a socket address to it's derived type. Only the family
        specific derived addressing classes are permissible for \a Target.

        This cast is especially useful to cast a GenericBSDSocketAddress to it's concrete type.

        \related BSDSocketAddress
     */
    template <class Target>
    Target & sockaddr_cast(BSDSocketAddress & source);

    /** \brief Safe socket address down-cast (const)
        \see sockaddr_cast()
        \related BSDSocketAddress
     */
    template <class Target>
    Target const & sockaddr_cast(BSDSocketAddress const & source);

    /** \brief Output generic socket address

        This stream operator will output a generic BSDSocketAddress in a family depending format.

        \related BSDSocketAddress
     */
    std::ostream & operator<<(std::ostream & os, BSDSocketAddress const & addr);

    /** \brief Generic BSD \c sockaddr storage

        While BSDSocketAddress provides read-only generic \c sockaddr access,
        GenericBSDSocketAddress allows to store (write) arbitrary socket addresses. (It is
        internally based on \c sockaddr_storage).

        To access the stored address, use sockaddr_cast to cast the GenericBSDSocketAddress to the
        correct family specific address class.

        \ingroup addr_group
      */
    class GenericBSDSocketAddress
        : public BSDSocketAddress
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        GenericBSDSocketAddress();
        GenericBSDSocketAddress(BSDSocketAddress const & other);
        GenericBSDSocketAddress& operator=(const BSDSocketAddress & other);

        GenericBSDSocketAddress(const GenericBSDSocketAddress& other);
        GenericBSDSocketAddress& operator=(const GenericBSDSocketAddress& other);

        //\}
        //-////////////////////////////////////////////////////////////////////////
        ///\name Generic sockaddr interface
        //\{

        struct sockaddr const * sockaddr_p() const;
        struct sockaddr * sockaddr_p();

        using BSDSocketAddress::socklen_p;

        //\}

    protected:

    private:
        struct sockaddr_storage addr_;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "BSDSocketAddress.cci"
//#include "BSDSocketAddress.ct"
//#include "BSDSocketAddress.cti"
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
