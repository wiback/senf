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

#include "EthernetReassembler.hh"

// Custom includes

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::emu::EthernetReassembler

prefix_ senf::emu::EthernetReassemblerBase::EthernetReassemblerBase()
    : nextFragmentNr_(1), packetsUnfragmented_(0), packetsReassembled_(0), fragmentsInvalid_(0), fragmentsProcessed_(0)
{
}

prefix_ bool senf::emu::EthernetReassemblerBase::isFragmentedPacket(senf::EthernetPacket const & eth)
{
    return ( eth->type_length() == senf::EthOUIExtensionPacketType::etherType ||
               (eth->type_length() == senf::EthVLanPacketType::etherType &&
                eth.next<senf::EthVLanPacket>()->type_length() == senf::EthOUIExtensionPacketType::etherType) ) &&
                eth.find<EthernetFragmentPacket>(senf::nothrow);
}

prefix_ senf::EthernetPacket & senf::emu::EthernetReassemblerBase::reassembledPacket()
{
    return reassembledPacket_;
}


prefix_ bool senf::emu::EthernetReassemblerBase::processFrame(senf::EthernetPacket const & eth)
{
    EthernetFragmentPacket fragment (eth.next().find<EthernetFragmentPacket>(senf::nothrow));
    if (SENF_UNLIKELY(!fragment)) {
        // clone here to be on the safe side - this should not happen anyway
        reassembledPacket_ = eth.clone();
        packetsUnfragmented_++;
        return true;
    }

    std::uint8_t fragmentNr (fragment->fragmentNr());
    if (SENF_UNLIKELY(fragmentNr != nextFragmentNr_)){
        fragmentsInvalid_++;
        if (fragmentNr != 1)
            return false;
    }

    fragmentsProcessed_++;
    
    if (fragmentNr == 1) {
        bool vlanPresent (eth->type_length() == senf::EthVLanPacketType::etherType);
        reassembledPacket_ = eth.clone();
        senf::Packet p (reassembledPacket_);
        if (vlanPresent) {
            p = p.next();
            reassembledPacket_.next<senf::EthVLanPacket>()->type_length() << fragment->type_length();
        } else {
            reassembledPacket_->type_length() << fragment->type_length();
        }
        payloadIter_ = (senf::DataPacket::createAfter(p, fragment->size(), senf::noinit)).data().begin();
    }
    payloadIter_ = std::copy(
            fragment.next().data().begin(),
            std::next( fragment.next().data().begin(), std::min( senf::Packet::size_type(fragment->size()), fragment.next().data().size())),
            payloadIter_);
    nextFragmentNr_ = fragmentNr + 1;

    if (! fragment->moreFragment()) {
        nextFragmentNr_ = 1;
        reassembledPacket_.reparse();
        packetsReassembled_++;
        return true;
    }

    return false;
}

prefix_ unsigned senf::emu::EthernetReassemblerBase::fragmentsProcessed()
{
    unsigned tmp (fragmentsProcessed_);
    fragmentsProcessed_ = 0;
    return tmp;
}

prefix_ unsigned senf::emu::EthernetReassemblerBase::fragmentsInvalid()
{
    unsigned tmp (fragmentsInvalid_);
    fragmentsInvalid_ = 0;
    return tmp;
}

prefix_ unsigned senf::emu::EthernetReassemblerBase::packetsReassembled()
{
    unsigned tmp (packetsReassembled_);
    packetsReassembled_ = 0;
    return tmp;
}

prefix_ unsigned senf::emu::EthernetReassemblerBase::packetsUnfragmented()
{
    unsigned tmp (packetsUnfragmented_);
    packetsUnfragmented_ = 0;
    return tmp;
}

prefix_ senf::emu::EthernetReassembler::EthernetReassembler()
{
}

prefix_ senf::emu::EthernetReassemblerModule::EthernetReassemblerModule()
{
    route( input, output).autoThrottling(false);
    input.onRequest( &EthernetReassemblerModule::onRequest);
    input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
}

prefix_ void senf::emu::EthernetReassemblerModule::onRequest()
{
    senf::EthernetPacket const & eth (input());
    
    if (isFragmentedPacket(eth)) {
        if (processFrame(eth))
            output(reassembledPacket());
    } else {
        output(eth);
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
