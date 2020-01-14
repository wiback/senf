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
    \brief MIH protocol non-inline non-template implementation */

#include "MIHPacket.hh"
//#include "MIHPacket.ih"

// Custom includes
#include <boost/io/ios_state.hpp>
#include <senf/Utils/String.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::EtherTypes, senf::MIHPacketType::etherType, senf::MIHPacket);
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::MIHPacket );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::MIHGenericPayloadPacket );

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// MIHPacketType

prefix_ void senf::MIHPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "MIH Packet:\n"
       << "  protocol header:\n"
       << senf::fieldName("  version")           << unsigned( p->version()) << "\n"
       << senf::fieldName("  ack request")       << p->ackRequest() << "\n"
       << senf::fieldName("  ack response")      << p->ackResponse() << "\n"
       << senf::fieldName("  UIR")               << p->uir() << "\n"
       << senf::fieldName("  more fragments")    << p->moreFragment() << "\n"
       << senf::fieldName("  fragment number")   << p->fragmentNr() << "\n"
       << senf::fieldName("  message ID (MID)")  << unsigned( p->messageId()) << "\n"
       << senf::fieldName("    sid")             << unsigned( p->sid()) << "\n"
       << senf::fieldName("    opcode")          << unsigned( p->opcode()) << "\n"
       << senf::fieldName("    aid")             << unsigned( p->aid()) << "\n"
       << senf::fieldName("  transaction id")    << unsigned( p->transactionId()) << "\n"
       << senf::fieldName("  payload length")    << unsigned( p->payloadLength()) << "\n";
    p->src_mihfId().dump( os);
    p->dst_mihfId().dump( os);
}

prefix_ void senf::MIHPacketType::finalize(packet p)
{
    p->src_mihfId().finalize();
    p->dst_mihfId().finalize();
    p->payloadLength_() << p.size() - 8u;
    p->messageId() << key(p.next(nothrow));
}

prefix_ senf::PacketInterpreterBase::factory_t senf::MIHPacketType::nextPacketType(packet p)
{
    if (p.data().size() < initSize())
        return no_factory();
    PacketRegistry<MIHMessageRegistry>::Entry const * e (
        PacketRegistry<MIHMessageRegistry>::lookup( p->messageId(), nothrow ));
    return e ? e->factory() : MIHGenericPayloadPacket::factory();
}

prefix_ void senf::MIHPacketType::validate(packet p)
{
    try {
        if (p.data().size() < initSize())
            throw InvalidMIHPacketException("truncated MIH message; packet size:") << p.data().size();
        if (p.data().size() < p->payloadLength() + 8u)
            throw InvalidMIHPacketException("truncated MIH message; packet size:") << p.data().size() << "; payloadLength:" << p->payloadLength();
        if (p->version() != 1)
            throw InvalidMIHPacketException("invalid MIH version: ") << p->version();
        if (p.next(senf::nothrow))
            MIHMessageRegistry::instance().validate( p->messageId(), p.next());
    } catch (TruncatedPacketException & e) {
        throw InvalidMIHPacketException("truncated MIH message");
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// MIHGenericPayloadPacketType

prefix_ void senf::MIHGenericPayloadPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "MIH Payload (service specific TLVs):\n";
    typedef parser::tlvList_t::container_type tlvListContainer_t;
    tlvListContainer_t tlvListContainer (p->tlvList());
    for (tlvListContainer_t::const_iterator i = tlvListContainer.begin(); i != tlvListContainer.end(); ++i)
        i->dump( os);
}

prefix_ void senf::MIHGenericPayloadPacketType::finalize(packet p)
{
    typedef parser::tlvList_t::container_type tlvContainer_t;
    tlvContainer_t tlvs (p->tlvList() );
    for (tlvContainer_t::iterator i (tlvs.begin()); i != tlvs.end(); ++i) {
        MIHGenericTLVParser p (*i);
        p.finalize();
    }
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
