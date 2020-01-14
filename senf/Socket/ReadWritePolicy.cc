//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
                SENF_THROW_SYSTEM_EXCEPTION("::read");
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
                continue;
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
                continue;
            case ENOTCONN:
            case ENETDOWN:
            case ENXIO:
                // TODO: The below might not be the best solution for all use cases, but it's ok for WiBACK 
                // Pretend that we have written out such frames
                rv = size;
                break;
            case EAGAIN:
            case ENOBUFS:
                // According to the man page this should not happen, since packets are just silently being dropped.
                // It does happen in NetEmu using small TxQueues on WLAN interfaces 
                rv = 0;
                break;
            default:
                std::stringstream a, b;
                senf::hexdump( reinterpret_cast<char const *>(addr) , reinterpret_cast<char const *>(addr + len), a);
                senf::hexdump( buffer , buffer + size , b);
                SENF_THROW_SYSTEM_EXCEPTION("::sendto (" + b.str() + ") to " + a.str());
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
