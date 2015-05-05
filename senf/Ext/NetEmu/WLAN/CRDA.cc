//
// Copyright (C) 2014
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
//

#include "CRDA.hh"

// Custom includes
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef __O_TMPFILE
#define __O_TMPFILE     020000000
#endif
/* a horrid kludge trying to make sure that this will fail on old kernels */
#ifndef O_TMPFILE
#define O_TMPFILE (__O_TMPFILE | O_DIRECTORY)
#define O_TMPFILE_MASK (__O_TMPFILE | O_DIRECTORY | O_CREAT)      
#endif

#include <senf/Utils/Console.hh>
#include <senf/Ext/NetEmu/WLAN/WirelessNLController.hh>


#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::emu::CRDA & senf::emu::CRDA::instance()
{
    static CRDA instance_;
    return instance_;
}

prefix_ senf::emu::CRDA::CRDA()
    : dummyCountry_("AA"),
      dfsMode_(true)
{
    namespace fty = senf::console::factory;
    dir.add("kernelRegDomain",  fty::Command(&CRDA::kernelRegDomain, this));
    dir.add("worldRegDomain",   fty::Variable(worldRegDomain_));
    dir.add("regDomain",        fty::Variable(boost::cref(currentRegDomain_)));
    dir.add("dfsMode",          fty::Variable(boost::cref(dfsMode_)));

    // sync our CRDA behavior with the kernel setting (boot time argument, for now)
    int fd (::open("/proc/cmdline", O_RDONLY));
    if (fd != -1) {
        char buf[8192];
        size_t rd;
        if ((rd = ::read(fd, buf, sizeof(buf)-1)) > 0) {
            buf[rd] = '\0';
            dfsMode_ = strstr(buf, "WiBACK-CRDA") == NULL;
            if (!dfsMode_) {
                SENF_LOG( ("WiBACK-CRDA mode enabled.") );
            }
        }
        ::close(fd);
    }

    if (dfsMode_) {
        // DFS-aware world regulatory domain (00)
        worldRegDomain_.alpha2Country = dummyCountry_;
        worldRegDomain_.dfsRegion = senf::emu::RegulatoryDomain::DFSRegion::Unset;
        worldRegDomain_.rules.insert(senf::emu::RegulatoryRule()
                                     .frequencyRange(2402000, 2472000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000) );
        worldRegDomain_.rules.insert(senf::emu::RegulatoryRule()
                                     .frequencyRange(2457000, 2482000)
                                     .maxBandwidth(20000)
                                     .maxEIRP(3000)
                                     .noIR(true) );
        worldRegDomain_.rules.insert(senf::emu::RegulatoryRule()
                                     .frequencyRange(5170000, 5250000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000)
                                     .noIR(true) );
        worldRegDomain_.rules.insert(senf::emu::RegulatoryRule()
                                     .frequencyRange(5250000, 5330000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000)
                                     .noIR(true)
                                     .dfsRequired(true) );
        worldRegDomain_.rules.insert(senf::emu::RegulatoryRule()
                                     .frequencyRange(5490000, 5730000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000)
                                     .noIR(true)
                                     .dfsRequired(true) );
        worldRegDomain_.rules.insert(senf::emu::RegulatoryRule()
                                     .frequencyRange(5735000, 5835000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000)
                                     .noIR(true) );
    } else {
        // DFS-disabled world regulatory domain (00)
        worldRegDomain_.alpha2Country = dummyCountry_;
        worldRegDomain_.dfsRegion = senf::emu::RegulatoryDomain::DFSRegion::Unset;
        worldRegDomain_.rules.insert(senf::emu::RegulatoryRule()
                                     .frequencyRange(2402000, 2482000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000) );
        worldRegDomain_.rules.insert(senf::emu::RegulatoryRule()
                                     .frequencyRange(4900000, 6000000)
                                     .maxBandwidth(80000)
                                     .maxEIRP(3000) );
    }
}

prefix_ bool senf::emu::CRDA::init(std::string const & filename, bool masterMode)
{
    if (filename.empty())
        return false;
    regDbFile = filename;

    if (masterMode) {
        try {
            std::fstream fs;
            fs.open(regDbFile, std::fstream::out | std::fstream::trunc);
            fs.close();
        }
        catch(...) {
            return false;
        }
    }

    if (masterMode) {
        try {
            senf::emu::WirelessNLController wnlc;
            currentRegDomain_ = wnlc.get_regulatory();
            SENF_LOG( ("currentRegDomain initialized from kernel to " << currentRegDomain_) );
        } catch (senf::ExceptionMixin & e) {
            currentRegDomain_ = worldRegDomain_;
            SENF_LOG( ("No Wireless Subsystem found. This node is a non-wireless box. Defaulting to build-in worldRegDomain " << currentRegDomain_) );
        }
    }
    
    return true;
}

prefix_ senf::emu::RegulatoryDomain const & senf::emu::CRDA::regDomain()
    const
{
    return currentRegDomain_;
}

prefix_ bool senf::emu::CRDA::equalsKernel()
    const
{
    try {
        senf::emu::WirelessNLController wnlc;
        return currentRegDomain_.isEqual(wnlc.get_regulatory());
    }
    catch(...) {
    }

    return true;
}

prefix_ bool senf::emu::CRDA::regDomain(senf::emu::RegulatoryDomain regDomain)
{
    if (!regDomain)
        regDomain = worldRegDomain_;

    // we might need to revert, if the below fails
    senf::emu::RegulatoryDomain old (currentRegDomain_);
    
    // set the new regDomain first, to avoid possible race conditions with the kernel CRDA upcall
    currentRegDomain_ = regDomain;
    currentRegDomain_.rules.clear();
    for (auto & rule : regDomain.rules) {
        if (dfsMode_) {
            currentRegDomain_.rules.insert(rule);
        } else {
            // remove DFS related flags
            auto r (rule);
            r.dfsRequired(false);
            r.noIR(false);
            currentRegDomain_.rules.insert(r);
        }
    }
    
    if (dummyCountry_[1]++ == 'Z') {
        dummyCountry_[1] = 'A';
        if (dummyCountry_[0]++ == 'Z') {
            dummyCountry_[0] = 'A';
        }
    }

    if (setRegCountry(dummyCountry_)) {
        return true;
    }
    
    currentRegDomain_ = old;
    return false;
}

prefix_ bool senf::emu::CRDA::setRegCountry(std::string alpha2Country)
{
    if (alpha2Country.empty())
        alpha2Country = dummyCountry_;

    try {
        std::fstream fs;
        fs.open(regDbFile, std::fstream::out | std::fstream::trunc);
        fs << "init/regDomain " << currentRegDomain_ << ";" << std::endl;
        fs.close();
    }
    catch(...) {
        return false;
    }
    
    try {
        senf::emu::WirelessNLController wnlc;
        wnlc.set_regulatory_request(alpha2Country);
    } catch (senf::ExceptionMixin & e) {
        SENF_LOG( ("Setting Regulatory Country to " << alpha2Country << " failed: " << e.message()) );
        return false;
    }
    return true;
}

prefix_ void senf::emu::CRDA::setRegulatory(senf::emu::RegulatoryDomain const & regDomain)
{
    try {
        senf::emu::WirelessNLController wnlc;
        wnlc.set_regulatory(regDomain);
    } catch (senf::ExceptionMixin & e) {
        SENF_LOG( ("Setting Regulatory Domain failed: " << e.message()) );
        return;
    }
    SENF_LOG( ("Regulatory rules pushed to kernel") );
}

prefix_ void senf::emu::CRDA::kernelRegDomain(std::ostream & os)
{
    senf::emu::RegulatoryDomain regDomain;
    try {
        senf::emu::WirelessNLController wnlc;
        regDomain = wnlc.get_regulatory();
        os << regDomain << std::endl;
    } catch (senf::ExceptionMixin & e) {
        os << "No Wireless Subsystem found. This node is a non-wireless box." << std::endl;
    }
}

//
///////////////////////////////////////////////////////////////////////////
//

prefix_ void senf::emu::CRDA::setRegulatory_()
{
    auto regDomain (currentRegDomain_ ? currentRegDomain_ : worldRegDomain_);
    char *a2 (getenv("COUNTRY"));
    regDomain.alpha2Country = !!a2 ? a2 : dummyCountry_;
    setRegulatory(regDomain);
}

prefix_ void senf::emu::CRDA::help(int exit_status)
{
    std::cout << "Usage: crda [options]"
        "\n  options:"
        "\n     --help                       show this help"
        "\n     --regDomain=\"<regDomain>\"    specifies the regDomain to be pushed with setRegCountry."
        "\n     --setRegCountry=\"<alpha2>\"   trigger the kernel to load the specified regulatory for the given dummy country."
        "\n     --setRegulatory              push current/world regulatory rules to the kernel - called via udev"
        "\n     --getRegulatory              dump current kernel regulatory domain."
        "\n";
    exit(exit_status);
}

prefix_ int senf::emu::CRDA::run(int argc, char const ** argv)
{
    if (argc == 1)
        help(EXIT_SUCCESS);

    CRDA & crda (instance());

    namespace fty = senf::console::factory;
    senf::console::DirectoryNode & cmdDir (senf::console::root().add("init", fty::Directory()));
    cmdDir.add("help",          fty::Command( &CRDA::help, &crda).arg(senf::console::kw::default_value=EXIT_SUCCESS) );
    cmdDir.add("regDomain",     fty::Variable(currentRegDomain_));
    cmdDir.add("setRegCountry", fty::Command( &CRDA::setRegCountry, &crda).arg(senf::console::kw::default_value ="") );
    cmdDir.add("setRegulatory", fty::Command( &CRDA::setRegulatory_, &crda) );
    cmdDir.add("getRegulatory", fty::Command( &CRDA::kernelRegDomain, &crda) );

    crda.logTarget_.route<senf::log::MESSAGE>();

    try {
        // Try to read and parse the redDBFile written by the main main process
        // If present and valid, this will call setRegDomain
        if (!regDbFile.empty()) {
            senf::console::ConfigFile regDb (regDbFile);
            regDb.ignoreMissing();
            regDb.parse(cmdDir);
        }
    }
    catch(...) {};

    std::vector<std::string> nonOptions;
    senf::console::ProgramOptions cmdlineOptions (argc, argv, cmdDir);
    cmdlineOptions.nonOptions(nonOptions);
    try {
        cmdlineOptions.parse();
    } catch (senf::console::SyntaxErrorException const & e) {
        std::cerr << e.message() << std::endl;
        help(EXIT_FAILURE);
    }
    if (not nonOptions.empty())
        help(EXIT_FAILURE);

    return EXIT_SUCCESS;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
