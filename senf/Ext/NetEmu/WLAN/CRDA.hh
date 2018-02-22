//
// Copyright (C) 2014
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
//

#ifndef HH_SENF_Ext_NetEmu_WLAN_CRDA_
#define HH_SENF_Ext_NetEmu_WLAN_CRDA_ 1

// Custom includes
#include <boost/functional/hash.hpp>
#include <senf/Utils/Console/ScopedDirectory.hh>
#include <senf/Utils/Logger/SyslogTarget.hh>
#include <senf/Ext/NetEmu/WLAN/Regulatory.hh>
#include <senf/Ext/NetEmu/WLAN/WirelessNLController.hh>

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {
namespace emu {

#define DEFAULT_CRDA_SYNC_FILE       "/dev/shm/NetEMU-CRDA.sync"
#define CRDA_SLAVE_NAME              "wiback-crda"

    class CRDA
    {
    public:
        static CRDA & instance();

        senf::console::ScopedDirectory<> dir;

        bool init(bool MasterMode = false, std::string const & filename = DEFAULT_CRDA_SYNC_FILE);
        int  run(int argc, char const ** argv);
        
        std::string slaveName() const;

        bool regDomain(senf::emu::RegulatoryDomain regDomain);
        senf::emu::RegulatoryDomain const & regDomain() const;
        senf::emu::RegulatoryDomain const & worldRegDomain() const;

        bool equalsKernel();

        unsigned dfsRegionFlag() const;

    private:
        CRDA();

        // Master only
        void cachedRegDomains(std::ostream & os) const;

        // Common
        bool setRegCountry(std::string alpha2Country);
        void kernelRegDomain(std::ostream & os);

        // Slave CRDA only
        void setRegulatory();
        int setRegulatoryDirect();

        void help(int exit_status);
        void debugEnable();
        
        boost::scoped_ptr<senf::log::Target> logDebugTarget_;
        senf::log::SyslogTarget logTarget_;
        std::string dummyCountry_;
        senf::emu::RegulatoryDomain worldRegDomain_;
        senf::emu::RegulatoryDomain currentRegDomain_;
        unsigned dfsRegionFlag_;
        std::string syncFilename_;
        std::string logTag_;
        
        // here we keep a cache of known regDomains which we have already pushed into the kernel
        std::set<senf::emu::RegulatoryDomain> cachedRegDomains_;
    };    
}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "CRDA.cci"
//#include "CRDA.ct"
//#include "CRDA.cti"
#endif
