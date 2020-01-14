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

prefix_ bool senf::ppi::IPv4SourceForcingDgramWriter::operator()(Handle & handle,
                                                                 Packet const & packet)
{
    return sendtoandfrom(
            handle.fd(),
            reinterpret_cast<void*> (&*packet.data().begin()),
            packet.size(),
            reinterpret_cast<in_addr*> (&destination_),
            protocolId_,
            reinterpret_cast<in_addr*> (&source_)) > 0;
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
    struct cmsghdr *c = reinterpret_cast<cmsghdr *>(cbuf);
    struct in_pktinfo *pi;
    struct iovec iov;
    struct msghdr h;

    c->cmsg_len = CMSG_LEN(sizeof(struct in_pktinfo));
    c->cmsg_level = IPPROTO_IP;
    c->cmsg_type = IP_PKTINFO;

    pi = reinterpret_cast<in_pktinfo *>(CMSG_DATA(c));
    pi->ipi_ifindex = 0;
    memcpy(&pi->ipi_addr, &src->s_addr, sizeof(src->s_addr));

    iov.iov_base = const_cast<void *>(data);
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

prefix_ bool senf::ppi::IPv6SourceForcingDgramWriter::operator()(Handle & handle,
                                                                 Packet const & packet)
{
    return sendtoandfrom(
            handle.fd(),
            reinterpret_cast<void*> (&*packet.data().begin()),
            packet.size(),
            reinterpret_cast<in6_addr*> (&destination_),
            protocolId_,
            reinterpret_cast<in6_addr*> (&source_)) > 0;
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
    struct cmsghdr *c = reinterpret_cast<cmsghdr *>(cbuf);
    struct in6_pktinfo *pi;
    struct iovec iov;
    struct msghdr h;

    c->cmsg_len = CMSG_LEN(sizeof(struct in6_pktinfo));
    c->cmsg_level = IPPROTO_IPV6;
    c->cmsg_type = IPV6_PKTINFO;

    pi = reinterpret_cast<in6_pktinfo *>(CMSG_DATA(c));
    pi->ipi6_ifindex = 0;
    memcpy(&pi->ipi6_addr, src, 16);

    iov.iov_base = const_cast<void *>(data);
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


prefix_ void senf::ppi::LLSocketWriter::source(Handle & handle, LLSocketAddress source)
{
    ::bind(handle.fd(),
           const_cast<sockaddr *>(source.sockaddr_p()),
           source.socklen());
}

prefix_ void senf::ppi::TargetLLSocketWriter::target(LLSocketAddress const & target)
{
    target_ = target;
}

prefix_ senf::ppi::TargetLLSocketWriter::TargetLLSocketWriter()
{}

#undef prefix_
