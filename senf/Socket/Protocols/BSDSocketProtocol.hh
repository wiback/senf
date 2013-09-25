// $Id$
//
// Copyright (C) 2006
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
    \brief BSDSocketProtocol public header */

#ifndef HH_SENF_Socket_Protocols_BSDSocketProtocol_
#define HH_SENF_Socket_Protocols_BSDSocketProtocol_ 1

// Custom includes
#include <senf/Socket/SocketProtocol.hh>
#include <boost/cstdint.hpp>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

struct sock_filter;

namespace senf {

    /// \addtogroup protocol_facets_group
    //\{

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

        int error() const;              ///< Get and clear pending socket error
                                        /**< This call will get <em>and clear</em> a pending socket
                                             error. This includes asynchronous errors received via
                                             the network (e.g. via ICMP). */

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

        template <unsigned short N>
        void attachSocketFilter(::sock_filter (&filter)[N]);
        void detachSocketFilter();

    private:
        void do_attachSocketFilter(::sock_filter * filter, unsigned short len);
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
    };

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "BSDSocketProtocol.cci"
#include "BSDSocketProtocol.ct"
//#include "BSDSocketProtocol.cti"
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
