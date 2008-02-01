// $Id$
//
// Copyright (C) 2006
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
    \brief BSDSocketProtocol public header */

#ifndef HH_BSDSocketProtocol_
#define HH_BSDSocketProtocol_ 1

// Custom includes
#include "../../Socket/SocketProtocol.hh"
#include <boost/cstdint.hpp>

//#include "BSDSocketProtocol.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_facets_group
    /// @{

    /** \brief Protocol facet providing basic BSD socket functionality

        BSDSocketProtocol provides the basic BSD socket API as shared by all BSD sockets.
     */
    class BSDSocketProtocol
        : public virtual SocketProtocol
    {
    public:
        std::pair<bool,unsigned> linger() const; ///< Return current linger status
                                        /**< The value is returned in an std:pair. the first element
                                             is \c true, if linger is active. The second value is
                                             the linger timeout in seconds.
                                             \returns linger state (enable disabled) and linger
                                             timeout */
        void linger(bool enable, unsigned timeout=0) const; ///< Change linger status
                                        /**< If linger is enabled, the timeout value specifies, how
                                             long to wait before returning while data is unsent in
                                             seconds. If this value is 0, a close() might wait
                                             forever.
                                             \param[in] enable \c true to activate linger
                                             \param[in] timeout linger timeout in seconds */

    };

    /** \brief Protocol facet providing basic connection oriented BSD socket functions

        AddressableBSDSocketProtocol provides the BSD socket API as it generically applies to
        addressable (connection oriented) sockets.
     */
    class AddressableBSDSocketProtocol
        : public virtual SocketProtocol
    {
    public:
        bool reuseaddr() const;         ///< Return current reuseaddr state
                                        /**< \returns \c true if \c SO_REUSEADDR is currently
                                             enabled, \c false otherwise*/
        void reuseaddr(bool value) const; ///< Set reuseraddr state
                                        /**< A \c true value enables \c SO_REUSEADDR, \c false will
                                             disable it.
                                             \param[in] value new \c SO_REUSEADDR state */

        boost::uint8_t priority() const;  ///< Get packet priority assigned to outgoing packets
                                        /**< This call will return the priority value assigned to
                                             packets sent via this socket. Depending on the
                                             protocol, this value may also be placed inside the
                                             packet headers (for IPv4, this is the TOS value).
                                             \returns current socket priority */
        void priority(boost::uint8_t value) const; ///< Set packet priority (e.g. TOS)
                                        /**< Changes the packet queueing priority. Additionally may
                                             set protocol specific options. For IPv4 sockets, it
                                             sets the TOS field.
                                             \param[in] value new socket priority */

        unsigned rcvbuf() const;        ///< Check receive buffer size
                                        /**< \returns size of receive buffer in bytes 
                                             \internal Linux doubles the buffer size internally when
                                                 changing it to cater for additional space needed by
                                                 the linux kernel. This call will therefore return
                                                 only half the value reported by the kernel. */
        void rcvbuf(unsigned size) const; ///< Change receive buffer size
                                        /**< \param[in] size new receive buffer size */

        unsigned sndbuf() const;        ///< Check send buffer size
                                        /**< \returns size of send buffer in bytes 
                                             \internal Linux doubles the buffer size internally when
                                                 changing it to cater for additional space needed by
                                                 the linux kernel. This call will therefore return
                                                 only half the value reported by the kernel. */
        void sndbuf(unsigned size) const; ///< Change size of send buffer
                                        /**< \param[in] size new send buffer size */
        
    };

    /// @}

}


///////////////////////////////hh.e////////////////////////////////////////
//#include "BSDSocketProtocol.cci"
//#include "BSDSocketProtocol.ct"
//#include "BSDSocketProtocol.cti"
//#include "BSDSocketProtocol.mpp"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
