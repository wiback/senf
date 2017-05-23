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

#ifndef HH_WiBACK_Core_Interfaces_EthernetFragmenter_
#define HH_WiBACK_Core_Interfaces_EthernetFragmenter_ 1

// Custom includes
#include <boost/unordered/unordered_map.hpp>
#include <senf/PPI/Module.hh>
#include <senf/PPI/Connectors.hh>
#include "EthernetFragmentPacket.hh"

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {
namespace emu {

    class EthernetFragmenterBase
    {
    public:
        EthernetFragmenterBase();
        virtual ~EthernetFragmenterBase() {};

        unsigned fragmentationCount();

        static bool fragmentationRequired(senf::EthernetPacket const & pkt, unsigned threshold);

    protected:
        void do_fragmentFrame(senf::EthernetPacket const & eth, unsigned threshold);

    private:
        virtual void v_outputFragment(senf::EthernetPacket const & eth) = 0;

        unsigned fragmentationCount_;
    };

    class EthernetFragmenter
        : public EthernetFragmenterBase
    {
    public:
        EthernetFragmenter();

        void fragmentFrame(senf::EthernetPacket const & pkt, unsigned treshold);
        std::vector<senf::EthernetPacket> & fragments();
        
    private:
        std::vector<senf::EthernetPacket> fragments_;
        void v_outputFragment(senf::EthernetPacket const & eth)  override;
    };

    class EthernetFragmenterModule
        : public senf::ppi::module::Module,
          public EthernetFragmenterBase 
    {
        SENF_PPI_MODULE( EthernetFragmenterModule );

    public:
        senf::ppi::connector::PassiveInput<senf::EthernetPacket> input;
        senf::ppi::connector::ActiveOutput<senf::EthernetPacket> output;

        EthernetFragmenterModule(std::uint16_t defaultFragThresh = 1500u);

        void fragmentationThreshold(std::uint16_t _fragmentationThreshold, senf::MACAddress const & dst = senf::MACAddress::None);
        std::uint16_t fragmentationThreshold(senf::MACAddress const & dst = senf::MACAddress::None) const;
        boost::unordered_map<senf::MACAddress,std::uint16_t> const & fragThreshMap() const;
        void reset();
        void bypass(bool on);
        
    private:
        void v_outputFragment(senf::EthernetPacket const & eth)  override;

        void onRequest();
        void onRequestBypass();

        boost::unordered_map<senf::MACAddress,std::uint16_t> fragThreshMap_;
        std::uint16_t  defaultFragThresh_;
     };
}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "EthernetFragmenter.cci"
//#include "EthernetFragmenter.ct"
//#include "EthernetFragmenter.cti"
#endif
