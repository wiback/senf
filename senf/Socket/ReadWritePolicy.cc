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
    \brief ReadPolicy and WritePolicy non-inline non-template implementation
 */

#include "ReadWritePolicy.hh"
#include "senf/Utils/hexdump.hh"
#include "senf/Utils/String.hh"
//#include "ReadWritePolicy.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

//#include "ReadWritePolicy.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ unsigned senf::ReadablePolicy::read(FileHandle & handle, char * buffer,
                                                   unsigned size)
{
    int rv = -1;
    do {
        rv = ::read(handle.fd(), buffer, size);
        if (rv < 0)
            switch(errno) {
            case EINTR:
                break;
            case EAGAIN:
                // This means, the socket is non-blocking an no data was available
                rv = 0;
                break;
            default:
                SENF_THROW_SYSTEM_EXCEPTION(":::read");
            }
    } while (rv<0);
    return rv;
}

prefix_ unsigned senf::ReadablePolicy::do_readfrom(FileHandle & handle, char * buffer,
                                                          unsigned size,
                                                          struct ::sockaddr * addr, socklen_t * len)
{
    int rv = -1;
    do {
        rv = ::recvfrom(handle.fd(),buffer, size, 0, addr, len);
        if (rv < 0)
            switch (errno) {
            case EINTR:
                break;
            case EAGAIN:
                rv = 0;
                break;
            default:
                SENF_THROW_SYSTEM_EXCEPTION("::recvfrom");
            }
    } while (rv<0);
    return rv;
}

prefix_ unsigned senf::WriteablePolicy::do_write(FileHandle & handle, char const * buffer,
                                                        unsigned size)
{
    int rv = -1;
    do {
        rv = ::write(handle.fd(), buffer, size);
        if (rv < 0)
            switch (errno) {
            case EINTR:
                break;
            case EAGAIN:
            case ENOBUFS:
                // According to the man page this should not happen, since packets are just silently being dropped.
                // It does happen in NetEmu using small TxQueues on WLAN interfaces 
            case ECONNREFUSED:
                // Writing to a UDP socket seems return this error code if a corresponding ICMP
                // error code has been received before (at least on linux). This is inconsistent
                // since I cannot rely on getting ECONNREFUSED. I therefore ignore this error. TCP
                // sockets will return this error on connect() and not on write(). Therefore we can
                // unconditionally ignore this error here.
                rv = 0;
                break;
            default:
                SENF_THROW_SYSTEM_EXCEPTION("::write");
            }
    } while (rv<0);
    return rv;
}

prefix_ unsigned senf::WriteablePolicy::do_writeto(FileHandle & handle,
                                                          char const * buffer, unsigned size,
                                                          struct sockaddr const * addr, socklen_t len)
{
    int rv = -1;
    do {
        rv = ::sendto(handle.fd(), buffer, size, 0, addr, len);
        if (rv < 0)
            switch (errno) {
            case EINTR:
                break;
            case EAGAIN:
            case ENOBUFS:
                // According to the man page this should not happen, since packets are just silently being dropped.
                // It does happen in NetEmu using small TxQueues on WLAN interfaces 
                rv = 0;
                break;
            default:
	        std::stringstream a, b;
                senf::hexdump( (char*) addr , ((char*) addr) + len , a);
		senf::hexdump( (char*) buffer , ((char*) buffer) + size , b);
                SENF_THROW_SYSTEM_EXCEPTION("::sendto(" + b.str() + ") to " + a.str());
            }
    } while (rv<0);
    return rv;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "ReadWritePolicy.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
