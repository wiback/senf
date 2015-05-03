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

        int  run(int argc, char const ** argv);

        bool setNextDummyRegCountry();
        bool regulatoryDomain(senf::emu::RegulatoryDomain const & regDomain);
        senf::emu::RegulatoryDomain const & regulatoryDomain() const;
        bool setRegDomainFromKernel();

        bool createTmpFile(std::string const & filename);

    private:
        CRDA();

        bool setRegCountry(std::string alpha2Country);
        void setWorldRegulatory(std::string const & alpha2Country);
        void setCurrentRegulatory(std::string const & alpha2Country);
        void setRegulatory(senf::emu::RegulatoryDomain const & regDomain);
        void dumpRegulatory(std::ostream & os);

        void parseConfigFile(std::string const & filename);
        void help();
        void print_help(int exit_status);

        senf::log::SyslogTarget logTarget_;
        std::string dummyCountry_;
        senf::emu::RegulatoryDomain worldRegDomain_;
        senf::emu::RegulatoryDomain currentRegDomain_;
        bool dfsMode_;
        unsigned regDomainFd_;
    };
}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "CRDA.cci"
//#include "CRDA.ct"
//#include "CRDA.cti"
#endif
