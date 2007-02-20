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
    \brief GenericSockAddr public header */

/** \defgroup addr_group Socket Addressing

    To support the core socket functionality we need a collection of classing providing addressing
    for the different protocols.

    Every Address Implementation used with the standard policy classes Has to have a set of minimum
    members. These members are documented in GenericSockAddr. However, these members are \e neither
    abstract \e nor virtual and other address classes do \e not inherit from GenericSockAddr. The
    address classes are not usable polymorphically for performance reasons.

    The interface defined above forces the implementation to be directly based on the corresponding
    sockaddr data structures provided by the BSD socket API. These structures however are wrapped
    into more flexible and more easy to use classes. Using the sockaddr representation increases the
    performance since any intermediarre conversions are skipped.
 */

#ifndef HH_GenericSockAddr_
#define HH_GenericSockAddr_ 1

// Custom includes
#include <sys/socket.h>

//#include "GenericSockAddr.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup addr_group
    /// @{

    /** \brief Generic protocol-independant socket address

        This address type does not depend on the protocol of the socket. It does nowever not support
        any protocol specific API, so access to the address is very limited.
     */
    class GenericSockAddr
    {
    public:
        GenericSockAddr();

        /// \name Generic Address Interface
        /// @{

        struct sockaddr * sockaddr_p(); ///< Return a pointer to sockaddr structure
                                        /**< The exact structure pointed to depends on the address
                                             family. The data pointed to must be \e mutable. The
                                             value must be changeable and any change of the value
                                             through this pointer must be reflected in the visible
                                             address interface.
                                             \returns non-const (!) pointer to sockaddr structure */
        struct sockaddr const * sockaddr_p() const; ///< Return a pointer to sockaddr structure
                                        /**< This member is like sockaddr_p(), hoewever it does not
                                             allow changing the address.
                                             \returns const pointer to sockaddr structure */
        unsigned sockaddr_len() const;  ///< Return size of address
                                        /**< This meember return the size of the socket address
                                             structure as returned by sockaddr_p() in bytes.
                                             \returns size of respective sockaddr structure in bytes
                                          */
        /// @}

    private:
        struct ::sockaddr_storage addr_;
    };

    /// @}
}

///////////////////////////////hh.e////////////////////////////////////////
#include "GenericSockAddr.cci"
//#include "GenericSockAddr.ct"
//#include "GenericSockAddr.cti"
//#include "GenericSockAddr.mpp"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
