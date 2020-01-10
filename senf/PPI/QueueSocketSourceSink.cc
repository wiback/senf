//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief QueueSocketSourceSink non-inline non-template implementation */

#include "QueueSocketSourceSink.hh"
#include "QueueSocketSourceSink.ih"

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::ppi::module::QueueEthVLanFilter::QueueEthVLanFilter()
{
    route(input, output).autoThrottling(false);
    input.onRequest(&QueueEthVLanFilter::request);
    input.throttlingDisc( senf::ppi::ThrottlingDiscipline::NONE);
}

prefix_ void senf::ppi::module::QueueEthVLanFilter::request()
{
    EthernetPacket eth (input());
    QueueBufferAnnotation const & buffer (eth.annotation<QueueBufferAnnotation>());
    boost::optional<unsigned> vlanId (buffer->vlan());
    unsigned tpid (buffer->tpid());
    if (vlanId) {
        if (tpid == EthVLanSPacketType::etherType) {
            EthVLanSPacket vlan (EthVLanSPacket::createInsertBefore(eth.next()));
            vlan->vlanId() << *vlanId;
            vlan->type_length() << eth->type_length();
            eth.finalizeTo(vlan);
            vlan.reparse();
        } else {
            EthVLanCPacket vlan (EthVLanCPacket::createInsertBefore(eth.next()));
            vlan->vlanId() << *vlanId;
            vlan->type_length() << eth->type_length();
            eth.finalizeTo(vlan);
            vlan.reparse();
        }
    }
    output(eth);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
