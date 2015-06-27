// $Id:$
//
// Copyright (C) 2012
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
//       Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>

#include "EthernetFragmentation.hh"

// Custom includes

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::emu::EthernetFragmenter

prefix_ senf::emu::EthernetFragmenter::EthernetFragmenter(unsigned fragmentationThreshold)
    : fragmentationThreshold_(fragmentationThreshold), fragmentationCount_(0)
{
    route( input, output).autoThrottling( false);
    input.onRequest( &EthernetFragmenter::onRequest);
    input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
}

prefix_ bool senf::emu::EthernetFragmenter::fragmentationRequired(senf::EthernetPacket const & eth, unsigned _fragmentationThreshold)
{
    unsigned payloadSize (eth.size() - senf::EthernetPacketParser::fixed_bytes);
    if (eth->type_length() == senf::EthVLanPacketType::etherType)
        payloadSize -= senf::EthVLanPacketParser::fixed_bytes;

    return (payloadSize > _fragmentationThreshold) or (eth.size() < senf::EthernetPacketType::minPayloadSize);
}

prefix_ void senf::emu::EthernetFragmenter::fragmentFrame(senf::EthernetPacket const & pkt, unsigned _fragmentationThreshold, std::vector<senf::EthernetPacket> & frags)
{
    typedef senf::Packet::size_type size_type;
    unsigned fragmentNr (1);
    bool vlanPresent (pkt->type_length() == senf::EthVLanPacketType::etherType);
    size_type payloadSize (pkt.size() - senf::EthernetPacketParser::fixed_bytes);
    senf::PacketData::iterator payloadIter (std::next( pkt.data().begin(), senf::EthernetPacketParser::fixed_bytes));
    senf::PacketData::iterator payloadEnd (pkt.data().end());
    if (vlanPresent)
        std::advance(payloadIter, senf::EthVLanPacketParser::fixed_bytes);

    fragmentationCount_++;

    do {
        senf::EthernetPacket eth (pkt.clone());
        EthernetFragmentPacket fragmentHeader (EthernetFragmentPacket::createAfter(senf::EthOUIExtensionPacket::createAfter(vlanPresent ? eth.next() : eth) ));

        size_type fragmentPayloadSize (std::min( size_type(std::distance(payloadIter, payloadEnd)),
                _fragmentationThreshold - senf::EthOUIExtensionPacketParser::fixed_bytes - EthernetFragmentPacketParser::fixed_bytes));

        int fragmentSize (
                senf::EthernetPacketParser::fixed_bytes +
                (vlanPresent ? senf::EthVLanPacketParser::fixed_bytes : 0u) +
                senf::EthOUIExtensionPacketParser::fixed_bytes +
                EthernetFragmentPacketParser::fixed_bytes +
                fragmentPayloadSize );

        size_type padding (std::max(0, int(senf::EthernetPacketType::minPayloadSize) - fragmentSize));

        senf::DataPacket fragmentPayload (senf::DataPacket::createAfter(fragmentHeader, fragmentPayloadSize+padding, senf::noinit));

        std::copy( payloadIter, std::next(payloadIter, fragmentPayloadSize), fragmentPayload.data().begin());
        std::advance( payloadIter, fragmentPayloadSize);

        fragmentHeader->moreFragment() << (payloadIter != payloadEnd);
        fragmentHeader->size() << (fragmentNr == 1 ? payloadSize : fragmentPayloadSize);
        fragmentHeader->fragmentNr() << fragmentNr++;
        fragmentHeader->type_length() << (vlanPresent ? pkt.next<senf::EthVLanPacket>()->type_length() : pkt->type_length());

        eth.finalizeTo(fragmentHeader);

        frags.emplace_back(eth);

    } while (payloadIter != payloadEnd);
}

prefix_ void senf::emu::EthernetFragmenter::fragmentFrame(senf::EthernetPacket const & pkt)
{
    typedef senf::Packet::size_type size_type;

    size_type payloadSize (pkt.size() - senf::EthernetPacketParser::fixed_bytes);
    bool vlanPresent (pkt->type_length() == senf::EthVLanPacketType::etherType);

    if (vlanPresent)
        payloadSize -= senf::EthVLanPacketParser::fixed_bytes;

    // no fragmentation needed
    if( (payloadSize <= fragmentationThreshold_) and (pkt.size() >= senf::EthernetPacketType::minPayloadSize)) {
        output(pkt);
        return;
    }

    unsigned fragmentNr (1);
    senf::PacketData::iterator payloadIter (std::next( pkt.data().begin(), senf::EthernetPacketParser::fixed_bytes + (vlanPresent ? senf::EthVLanPacketParser::fixed_bytes : 0)));
    senf::PacketData::iterator payloadEnd (pkt.data().end());

    fragmentationCount_++;

    do {
        senf::EthernetPacket eth (pkt.clone());
        EthernetFragmentPacket fragmentHeader (EthernetFragmentPacket::createAfter(senf::EthOUIExtensionPacket::createAfter(vlanPresent ? eth.next() : eth) ));

        size_type fragmentPayloadSize (std::min( size_type(std::distance(payloadIter, payloadEnd)),
                fragmentationThreshold_ - senf::EthOUIExtensionPacketParser::fixed_bytes - EthernetFragmentPacketParser::fixed_bytes));

        int fragmentSize (
                senf::EthernetPacketParser::fixed_bytes +
                (vlanPresent ? senf::EthVLanPacketParser::fixed_bytes : 0u) +
                senf::EthOUIExtensionPacketParser::fixed_bytes +
                EthernetFragmentPacketParser::fixed_bytes +
                fragmentPayloadSize );

        size_type padding (std::max(0, int(senf::EthernetPacketType::minPayloadSize) - fragmentSize));

        senf::DataPacket fragmentPayload (senf::DataPacket::createAfter(fragmentHeader, fragmentPayloadSize+padding, senf::noinit));

        std::copy( payloadIter, std::next(payloadIter, fragmentPayloadSize), fragmentPayload.data().begin());
        std::advance( payloadIter, fragmentPayloadSize);

        fragmentHeader->moreFragment() << (payloadIter != payloadEnd);
        fragmentHeader->size() << (fragmentNr == 1 ? payloadSize : fragmentPayloadSize);
        fragmentHeader->fragmentNr() << fragmentNr++;
        fragmentHeader->type_length() << (vlanPresent ? pkt.next<senf::EthVLanPacket>()->type_length() : pkt->type_length());

        eth.finalizeTo(fragmentHeader);

        output(eth);

    } while (payloadIter != payloadEnd);
}

prefix_ void senf::emu::EthernetFragmenter::onRequest()
{
    fragmentFrame( input());
}

prefix_ void senf::emu::EthernetFragmenter::fragmentationThreshold(unsigned _fragmentationThreshold)
{
    fragmentationThreshold_ = std::max(576u, _fragmentationThreshold);
}

prefix_ unsigned senf::emu::EthernetFragmenter::fragmentationThreshold()
    const
{
    return fragmentationThreshold_;
}

prefix_ unsigned senf::emu::EthernetFragmenter::fragmentationCount()
{
    unsigned tmp (fragmentationCount_);
    fragmentationCount_ = 0;
    return tmp;
}


///////////////////////////////////////////////////////////////////////////
// senf::emu::EthernetReassembler

prefix_ senf::emu::EthernetReassembler::EthernetReassembler()
    : nextFragmentNr_(1)
{
    route( input, output).autoThrottling(false);
    input.onRequest( &EthernetReassembler::onRequest);
    input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
}

prefix_ bool senf::emu::EthernetReassembler::isFragmentedPacket(senf::EthernetPacket const & eth)
{
    return ( eth->type_length() == senf::EthOUIExtensionPacketType::etherType ||
               (eth->type_length() == senf::EthVLanPacketType::etherType &&
                eth.next<senf::EthVLanPacket>()->type_length() == senf::EthOUIExtensionPacketType::etherType) ) &&
                eth.find<EthernetFragmentPacket>(senf::nothrow);
}

namespace {
    std::stringstream && ethHeader(senf::EthernetPacket const & eth) {
        std::stringstream ss;
        ss << "src: " << eth->source() << " dst: " << eth->destination();
        if (eth->type_length() == senf::EthVLanPacketType::etherType)
            ss << " vlan: " << eth.next<senf::EthVLanPacket>()->vlanId();
        ss << " type: 0x" << std::hex << eth.find<senf::emu::EthernetFragmentPacket>()->type_length();
        return std::move(ss);
    }
}

prefix_ void senf::emu::EthernetReassembler::onRequest()
{
    senf::EthernetPacket const & eth (input());

    if (eth->type_length() != senf::EthOUIExtensionPacketType::etherType &&
        (eth->type_length() != senf::EthVLanPacketType::etherType || eth.next<senf::EthVLanPacket>()->type_length() != senf::EthOUIExtensionPacketType::etherType)) {
        output(eth);
        return;
    }

    EthernetFragmentPacket fragment (eth.next().find<EthernetFragmentPacket>(senf::nothrow));
    if (!fragment) {
        output(eth);
        return;
    }

    std::uint8_t fragmentNr (fragment->fragmentNr());
    if (fragmentNr != nextFragmentNr_) {
        SENF_LOG( (senf::log::MESSAGE) ("invalid EthernetPacket fragment received. (expected "
                << unsigned(nextFragmentNr_) << ", got " << unsigned(fragmentNr) << ")" << ethHeader(eth)) );
        if (fragmentNr != 1)
            return;
    }
    if (fragmentNr == 1) {
        bool vlanPresent (eth->type_length() == senf::EthVLanPacketType::etherType);
        fragmentedPacket_ = eth.clone();
        senf::Packet p (fragmentedPacket_);
        if (vlanPresent) {
            p = p.next();
            fragmentedPacket_.next<senf::EthVLanPacket>()->type_length() << fragment->type_length();
        } else {
            fragmentedPacket_->type_length() << fragment->type_length();
        }
        payloadIter_ = (senf::DataPacket::createAfter(p, fragment->size(), senf::noinit)).data().begin();
    }
    payloadIter_ = std::copy(
            fragment.next().data().begin(),
            std::next( fragment.next().data().begin(),
                    std::min( senf::Packet::size_type(fragment->size()), fragment.next().data().size())),
            payloadIter_);
    nextFragmentNr_ = fragmentNr + 1;
    if (! fragment->moreFragment()) {
        nextFragmentNr_ = 1;
        fragmentedPacket_.reparse();
        output( fragmentedPacket_);
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
