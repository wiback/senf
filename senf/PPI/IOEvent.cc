// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// http://senf.fokus.fraunhofer.de.de/license.html
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
    \brief IOEvent non-inline non-template implementation */

#include "IOEvent.hh"
//#include "IOEvent.ih"

// Custom includes
#include <senf/Utils/senfassert.hh>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <net/if.h>

//#include "IOEvent.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::IOEvent

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// private members

prefix_ void senf::ppi::IOEvent::v_enable()
{
    if (fd_ != -1)
        event_.enable();
}

prefix_ void senf::ppi::IOEvent::v_disable()
{
    if (fd_ != -1)
        event_.disable();
}

prefix_ void senf::ppi::IOEvent::cb(int event)
{
    if (SENF_UNLIKELY((event & ~event_.events()) != 0)) {
        if (event & Err) {
            int err (0);
            socklen_t len (sizeof(err));
            if (::getsockopt(fd_, SOL_SOCKET, SO_ERROR, &err, &len) < 0)
                err = 0;
            char buf[1024];
            sockaddr *addr = reinterpret_cast<sockaddr*>(buf);
            len = sizeof(buf);
            std::string msg;
            if (::getsockname(fd_, addr, &len) == 0) {
                if (addr->sa_family == AF_PACKET) {
                    sockaddr_ll *sa = reinterpret_cast<sockaddr_ll*>(addr);
                    if (sa->sll_ifindex > 0) {
                        char name[256];
                        ::bzero(name, 256);
                        if (::if_indextoname(sa->sll_ifindex, name))
                            msg = name;
                        else
                            msg = "if_index=" + senf::str(sa->sll_ifindex);
                    } else {
                        msg = "if_index is 0";
                    }  
                } else {
                    msg = "sa_family=" + senf::str(addr->sa_family);
                }
            } else {
                msg = "getsockname() failed";
            }
            // ENETDOWN seems to result from a race condition inside SENF. Let's disable events for this FD
            if (err == ENETDOWN) {
                SENF_LOG ( (senf::log::IMPORTANT) ("ENETDOWN on fd " << fd_ << " (" << msg << ") received. Disabling event. This might be due to a race condition.") ); 
                enabled(false);
                return;
            } else {
                throw ErrorException(fd_, err, msg);
            }
        }
        if (event & Hup)
            throw HangupException();
        SENF_ASSERT(false, "Internal failure: IOEvent::cb() called with invalid flag??");
    } else {
        IOEventInfo info = { event };
        callback(info);
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "IOEvent.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
