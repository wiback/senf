//
// Copyright (C) 2014
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
//

#ifndef HH_SENF_Ext_NetEmu_WLAN_CRDA_
#define HH_SENF_Ext_NetEmu_WLAN_CRDA_ 1

// Custom includes
#include <senf/Utils/Console/ScopedDirectory.hh>
#include <senf/Utils/Logger/SyslogTarget.hh>
#include <senf/Ext/NetEmu/WLAN/Regulatory.hh>

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {
namespace emu {

    class CRDA
    {
    public:
        static CRDA & instance();

        senf::console::ScopedDirectory<> dir;

        bool init(std::string const & filename, bool create = false);
        int  run(int argc, char const ** argv);

        bool regDomain(senf::emu::RegulatoryDomain const & regDomain);
        senf::emu::RegulatoryDomain const & regDomain() const;

        bool equalsKernel() const;

    private:
        CRDA();

        bool setRegCountry(std::string alpha2Country);
        void kernelRegDomain(std::ostream & os);
        void setRegulatory(senf::emu::RegulatoryDomain const & regDomain);

        void help(int exit_status);
        void setRegulatory_();

        senf::log::SyslogTarget logTarget_;
        std::string dummyCountry_;
        senf::emu::RegulatoryDomain worldRegDomain_;
        senf::emu::RegulatoryDomain currentRegDomain_;
        bool dfsMode_;
        std::string regDbFile;
    };
}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "CRDA.cci"
//#include "CRDA.ct"
//#include "CRDA.cti"
#endif
