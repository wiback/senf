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
    \brief BSDSocketAddress public header */

#ifndef HH_SENF_Socket_Protocols_BSDSocketAddress_
#define HH_SENF_Socket_Protocols_BSDSocketAddress_ 1

// Custom includes
#include <boost/type_traits/alignment_of.hpp>
#include <boost/type_traits/type_with_alignment.hpp>
#include "../../Utils/safe_bool.hh"
#include <sys/socket.h>
#include <iostream>

//#include "BSDSocketAddress.mpp"
///////////////////////////////hh.p////////////////////////////////////////

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
        : public senf::comparable_safe_bool<BSDSocketAddress>
    {
    public:
        bool operator==(BSDSocketAddress const & other) const; ///< Compare two arbitrary addresses
                                        /**< For addresses to be considered equal, they must have
                                             the same family, length and the data must be
                                             identical. */
        bool operator!=(BSDSocketAddress const & other) const; ///< Inverse of operator==

        bool boolean_test() const;      ///< Return \c true, if address is not empty
                                        /**< An address is considered empty if
                                             \li the family is AF_UNSPEC
                                             \li or the size is 0
                                             \li or all data bytes are 0 */

        short family() const;           ///< Return the address family.
                                        /**< This value is found in the \c addressFamily member of
                                             each typed derived class
                                             (e.g. INet4Address::addressFamily) */

        ///////////////////////////////////////////////////////////////////////////
        ///\name Generic sockaddr interface
        ///\{

        struct sockaddr const * sockaddr_p() const;
        socklen_t socklen() const;
        socklen_t const * socklen_p() const;

        ///\}

    protected:
        BSDSocketAddress(socklen_t len, short family);
        BSDSocketAddress(BSDSocketAddress const & other);
        BSDSocketAddress & operator=(BSDSocketAddress const & other);

        struct sockaddr * sockaddr_p();
        socklen_t * socklen_p();

        void socklen(socklen_t len);

    private:

        union {
            socklen_t len_;
            boost::type_with_alignment<boost::alignment_of<struct sockaddr_storage>::value> _;
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
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        GenericBSDSocketAddress();
        GenericBSDSocketAddress(BSDSocketAddress const & other);
        GenericBSDSocketAddress& operator=(const BSDSocketAddress & other);

        GenericBSDSocketAddress(const GenericBSDSocketAddress& other);
        GenericBSDSocketAddress& operator=(const GenericBSDSocketAddress& other);
        
        ///@}
        ///////////////////////////////////////////////////////////////////////////
        ///\name Generic sockaddr interface
        ///\{

        struct sockaddr const * sockaddr_p() const;
        struct sockaddr * sockaddr_p();

        using BSDSocketAddress::socklen_p;

        ///\}

    protected:

    private:
        struct sockaddr_storage addr_;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
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
