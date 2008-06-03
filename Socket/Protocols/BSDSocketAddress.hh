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

#ifndef HH_BSDSocketAddress_
#define HH_BSDSocketAddress_ 1

// Custom includes
#include "../../Utils/safe_bool.hh"
#include <sys/socket.h>
#include <iostream>

//#include "BSDSocketAddress.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief
      */
    class BSDSocketAddress
        : public senf::comparable_safe_bool<BSDSocketAddress>
    {
    public:
        
        bool operator==(BSDSocketAddress const & other) const;
        bool operator!=(BSDSocketAddress const & other) const;

        bool boolean_test() const;
        short family() const;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Generic \c sockaddr interface
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

        socklen_t len_;
    };

    template <class Target>
    Target & sockaddr_cast(BSDSocketAddress & source);

    template <class Target>
    Target const & sockaddr_cast(BSDSocketAddress const & source);

    std::ostream & operator<<(std::ostream & os, BSDSocketAddress const & addr);

    /** \brief
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
        ///\name Generic \c sockaddr interface
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
