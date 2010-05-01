// $Id: SocketSink.cc 661 2008-02-05 09:53:54Z dw6 $
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//    David Wagner <dw6@berlios.de>
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
    \brief SocketSink non-inline non-template implementation */

// Custom includes
#include "SocketSink.hh"
#include <senf/Socket/ClientSocketHandle.hh>

#define prefix_

prefix_ senf::ppi::IPv4SourceForcingDgramWriter::IPv4SourceForcingDgramWriter()
{
    source_ = senf::INet4Address::from_string("0.0.0.0");
    destination_ = senf::INet4Address::from_string("0.0.0.0");
    protocolId_ = 0;
}

prefix_ senf::ppi::IPv4SourceForcingDgramWriter::
IPv4SourceForcingDgramWriter(senf::INet4Address sourceAddr, senf::INet4SocketAddress destAddr)
{
    source(sourceAddr);
    destination(destAddr);
}

prefix_ void senf::ppi::IPv4SourceForcingDgramWriter::source(senf::INet4Address & source)
{
    source_ = source;
}

prefix_ void senf::ppi::IPv4SourceForcingDgramWriter::destination(senf::INet4SocketAddress & dest)
{
    destination_ = dest.address();
    protocolId_ = dest.port();
}

prefix_ void senf::ppi::IPv4SourceForcingDgramWriter::operator()(Handle handle,
                                                                 Packet const & packet)
{
    sendtoandfrom(
            handle.fd(),
            reinterpret_cast<void*> (&*packet.data().begin()),
            packet.size(),
            reinterpret_cast<in_addr*> (&destination_),
            protocolId_,
            reinterpret_cast<in_addr*> (&source_));
}

prefix_ int senf::ppi::IPv4SourceForcingDgramWriter::sendtoandfrom(
        int sock,
        const void *data,
        size_t dataLen,
        const in_addr *dst,
        int port,
        const in_addr *src)
{
    uint8_t cbuf[CMSG_SPACE(sizeof(struct in_pktinfo))];
    struct cmsghdr *c = (struct cmsghdr *)cbuf;
    struct in_pktinfo *pi;
    struct iovec iov;
    struct msghdr h;

    c->cmsg_len = CMSG_LEN(sizeof(struct in_pktinfo));
    c->cmsg_level = IPPROTO_IP;
    c->cmsg_type = IP_PKTINFO;

    pi = (struct in_pktinfo *)CMSG_DATA(c);
    pi->ipi_ifindex = 0;
    memcpy(&pi->ipi_addr, src, 16);

    iov.iov_base = (void *)data;
    iov.iov_len = dataLen;

    sockaddr_in dstpeer;
    memset(&dstpeer, 0, sizeof(dstpeer));
    dstpeer.sin_family = AF_INET;
    dstpeer.sin_addr = *dst;
    dstpeer.sin_port = htons(port);

    memset(&h, 0, sizeof(h));
    h.msg_name = (struct sockaddr *)&dstpeer;
    h.msg_namelen = sizeof(dstpeer);
    h.msg_iov = &iov;
    h.msg_iovlen = 1;
    h.msg_control = c;
    h.msg_controllen = sizeof(cbuf);

    return sendmsg(sock, &h, 0);
}

prefix_ senf::ppi::IPv6SourceForcingDgramWriter::IPv6SourceForcingDgramWriter()
{
    source_ = senf::INet6Address::from_string("0::0");
    destination_ = senf::INet6Address::from_string("0::0");
    protocolId_ = 0;
}

prefix_ senf::ppi::IPv6SourceForcingDgramWriter::
IPv6SourceForcingDgramWriter(senf::INet6Address sourceAddr, senf::INet6SocketAddress destAddr)
{
    source(sourceAddr);
    destination(destAddr);
}

prefix_ void senf::ppi::IPv6SourceForcingDgramWriter::source(senf::INet6Address & source)
{
    source_ = source;
}

prefix_ void senf::ppi::IPv6SourceForcingDgramWriter::destination(senf::INet6SocketAddress & dest)
{
    destination_ = dest.address();
    protocolId_ = dest.port();
}

prefix_ void senf::ppi::IPv6SourceForcingDgramWriter::operator()(Handle handle,
                                                                 Packet const & packet)
{
    sendtoandfrom(
            handle.fd(),
            reinterpret_cast<void*> (&*packet.data().begin()),
            packet.size(),
            reinterpret_cast<in6_addr*> (&destination_),
            protocolId_,
            reinterpret_cast<in6_addr*> (&source_));
}

prefix_ int senf::ppi::IPv6SourceForcingDgramWriter::sendtoandfrom(
        int sock,
        const void *data,
        size_t dataLen,
        const in6_addr *dst,
        int port,
        const in6_addr *src)
{
    uint8_t cbuf[CMSG_SPACE(sizeof(struct in6_pktinfo))];
    struct cmsghdr *c = (struct cmsghdr *)cbuf;
    struct in6_pktinfo *pi;
    struct iovec iov;
    struct msghdr h;

    c->cmsg_len = CMSG_LEN(sizeof(struct in6_pktinfo));
    c->cmsg_level = IPPROTO_IPV6;
    c->cmsg_type = IPV6_PKTINFO;

    pi = (struct in6_pktinfo *)CMSG_DATA(c);
    pi->ipi6_ifindex = 0;
    memcpy(&pi->ipi6_addr, src, 16);

    iov.iov_base = (void *)data;
    iov.iov_len = dataLen;

    sockaddr_in6 dstpeer;
    memset(&dstpeer, 0, sizeof(dstpeer));
    dstpeer.sin6_family = AF_INET6;
    dstpeer.sin6_addr = *dst;
    dstpeer.sin6_port = htons(port);

    memset(&h, 0, sizeof(h));
    h.msg_name = (struct sockaddr *)&dstpeer;
    h.msg_namelen = sizeof(dstpeer);
    h.msg_iov = &iov;
    h.msg_iovlen = 1;
    h.msg_control = c;
    h.msg_controllen = sizeof(cbuf);

    return sendmsg(sock, &h, 0);
}

#undef prefix_
