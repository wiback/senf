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
        bool bypass() const;

    private:
        void v_outputFragment(senf::EthernetPacket const & eth)  override;

        void onRequest();
        void onRequestBypass();

        boost::unordered_map<senf::MACAddress,std::uint16_t> fragThreshMap_;
        std::uint16_t  defaultFragThresh_;
        bool bypass_;
     };
}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "EthernetFragmenter.cci"
//#include "EthernetFragmenter.ct"
//#include "EthernetFragmenter.cti"
#endif
