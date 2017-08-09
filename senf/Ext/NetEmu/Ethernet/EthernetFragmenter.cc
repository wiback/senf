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

#include "EthernetFragmenter.hh"

// Custom includes

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::emu::EthernetFragmenter

prefix_ senf::emu::EthernetFragmenterBase::EthernetFragmenterBase()
    : fragmentationCount_(0)
{
}

prefix_ bool senf::emu::EthernetFragmenterBase::fragmentationRequired(senf::EthernetPacket const & eth, unsigned threshold)
{
    unsigned payloadSize (eth.size() - senf::EthernetPacketParser::fixed_bytes -
             ((eth->type_length() == senf::EthVLanCPacketType::etherType) or (eth->type_length() == senf::EthVLanSPacketType::etherType)) * senf::EthVLanPacketParser::fixed_bytes);
    
    return payloadSize > threshold;
}

prefix_ unsigned senf::emu::EthernetFragmenterBase::fragmentationCount()
{
    unsigned tmp (fragmentationCount_);
    fragmentationCount_ = 0;
    return tmp;
}

prefix_ void senf::emu::EthernetFragmenterBase::do_fragmentFrame(senf::EthernetPacket const & pkt, unsigned threshold)
{
    typedef senf::Packet::size_type size_type;
    unsigned fragmentNr (1);
    bool vlanCPresent (pkt->type_length() == senf::EthVLanCPacketType::etherType);
    bool vlanSPresent (pkt->type_length() == senf::EthVLanSPacketType::etherType);
    size_type payloadSize (pkt.size() - senf::EthernetPacketParser::fixed_bytes);
    senf::PacketData::iterator payloadIter (std::next( pkt.data().begin(), senf::EthernetPacketParser::fixed_bytes +
                                                       ((vlanCPresent||vlanSPresent) * senf::EthVLanPacketParser::fixed_bytes)));
    senf::PacketData::iterator payloadEnd (pkt.data().end());

    fragmentationCount_++;

    do {
        senf::EthernetPacket eth (pkt.clone());
        EthernetFragmentPacket fragmentHeader (EthernetFragmentPacket::createAfter(senf::EthOUIExtensionPacket::createAfter((vlanCPresent||vlanSPresent) ? eth.next() : eth) ));

        size_type fragmentPayloadSize (std::min( size_type(std::distance(payloadIter, payloadEnd)),
                threshold - senf::EthOUIExtensionPacketParser::fixed_bytes - EthernetFragmentPacketParser::fixed_bytes));

        int fragmentSize (
                senf::EthernetPacketParser::fixed_bytes +
                ((vlanCPresent||vlanSPresent) ? senf::EthVLanPacketParser::fixed_bytes : 0u) +
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
        if (vlanSPresent)
            fragmentHeader->type_length() << pkt.next<senf::EthVLanSPacket>()->type_length();
        else if (vlanCPresent)
            fragmentHeader->type_length() << pkt.next<senf::EthVLanCPacket>()->type_length();
        else
            fragmentHeader->type_length() << pkt->type_length();

        eth.finalizeTo(fragmentHeader);

        v_outputFragment(eth);

    } while (payloadIter != payloadEnd);
}

prefix_ senf::emu::EthernetFragmenter::EthernetFragmenter()
{
}

prefix_ std::vector<senf::EthernetPacket> & senf::emu::EthernetFragmenter::fragments()
{
    return fragments_;
}

prefix_ void senf::emu::EthernetFragmenter::v_outputFragment(senf::EthernetPacket const & eth)
{
    fragments_.emplace_back(eth);
}

prefix_ void senf::emu::EthernetFragmenter::fragmentFrame(senf::EthernetPacket const & eth, unsigned threshold)
{
    fragments_.clear();
    do_fragmentFrame(eth, threshold);
}

prefix_ senf::emu::EthernetFragmenterModule::EthernetFragmenterModule(std::uint16_t defaultFragThresh)
    : defaultFragThresh_(std::max(std::uint16_t(576),defaultFragThresh))
{
    route( input, output).autoThrottling(false);
    input.onRequest( &EthernetFragmenterModule::onRequest);
    input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
}

prefix_ void senf::emu::EthernetFragmenterModule::fragmentationThreshold(std::uint16_t threshold, senf::MACAddress const & dst)
{
    if (!dst) {
        if (threshold > 0)
            defaultFragThresh_ = std::max(std::uint16_t(576), threshold);
        fragThreshMap_.clear();
    } else {
        fragThreshMap_.erase(dst);
        if (threshold > 0) {
            fragThreshMap_.insert(std::make_pair(dst, std::max(std::uint16_t(576), threshold)));
        }
    }
}

prefix_ std::uint16_t senf::emu::EthernetFragmenterModule::fragmentationThreshold(senf::MACAddress const & dst)
    const
{
    auto const it (fragThreshMap_.find(dst));
    if (SENF_LIKELY(it != fragThreshMap_.end())) {
        return it->second;
    }
    
    return defaultFragThresh_;
}

prefix_ boost::unordered_map<senf::MACAddress,std::uint16_t> const & senf::emu::EthernetFragmenterModule::fragThreshMap()
    const
{
    return fragThreshMap_;
}

prefix_ void senf::emu::EthernetFragmenterModule::reset()
{
    fragThreshMap_.clear();
    bypass(false);
}

prefix_ void senf::emu::EthernetFragmenterModule::bypass(bool on)
{
    input.onRequest( on ? &EthernetFragmenterModule::onRequestBypass : &EthernetFragmenterModule::onRequest); 
}


prefix_ void senf::emu::EthernetFragmenterModule::v_outputFragment(senf::EthernetPacket const & eth)
{
    output(eth);
}

prefix_ void senf::emu::EthernetFragmenterModule::onRequest()
{
    senf::EthernetPacket const & eth (input());
    if (fragmentationRequired(eth, fragmentationThreshold(eth->destination())))
        do_fragmentFrame(eth, fragmentationThreshold(eth->destination()));
    else
        output(eth);
}

prefix_ void senf::emu::EthernetFragmenterModule::onRequestBypass()
{
    output(input());
}


///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
