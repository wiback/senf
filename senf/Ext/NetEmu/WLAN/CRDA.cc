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

#include <senf/Utils/Console.hh>
#include <senf/Ext/NetEmu/WLAN/WirelessNLController.hh>
//#include "MIHMessaging/Utils/Enumerations.hh"
//#include "Utils/autoconf.hh"

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
    dir.add("setRegulatory",    fty::Command(&CRDA::setCurrentRegulatory, this));
    dir.add("dumpRegulatory",   fty::Command(&CRDA::dumpRegulatory, this));
    dir.add("worldRegDomain",   fty::Variable(worldRegDomain_));
    dir.add("currentRegDomain", fty::Variable(currentRegDomain_));
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
        worldRegDomain_.alpha2Country = "00";
        worldRegDomain_.dfsRegion = senf::emu::RegulatoryDomain::DFSRegion::Unset;
        worldRegDomain_.rules.push_back(senf::emu::RegulatoryRule()
                                          .frequencyRange(2402000, 2472000)
                                          .maxBandwidth(40000)
                                          .maxEIRP(3000) );
        worldRegDomain_.rules.push_back(senf::emu::RegulatoryRule()
                                          .frequencyRange(2457000, 2482000)
                                          .maxBandwidth(20000)
                                          .maxEIRP(3000)
                                          .noIR(true) );
        worldRegDomain_.rules.push_back(senf::emu::RegulatoryRule()
                                          .frequencyRange(5170000, 5250000)
                                          .maxBandwidth(40000)
                                          .maxEIRP(3000)
                                          .noIR(true) );
        worldRegDomain_.rules.push_back(senf::emu::RegulatoryRule()
                                          .frequencyRange(5250000, 5330000)
                                          .maxBandwidth(40000)
                                          .maxEIRP(3000)
                                          .noIR(true)
                                          .dfsRequired(true) );
        worldRegDomain_.rules.push_back(senf::emu::RegulatoryRule()
                                          .frequencyRange(5490000, 5730000)
                                          .maxBandwidth(40000)
                                          .maxEIRP(3000)
                                          .noIR(true)
                                          .dfsRequired(true) );
        worldRegDomain_.rules.push_back(senf::emu::RegulatoryRule()
                                          .frequencyRange(5735000, 5835000)
                                          .maxBandwidth(40000)
                                          .maxEIRP(3000)
                                          .noIR(true) );
    } else {
        // DFS-disabled world regulatory domain (00)
        worldRegDomain_.alpha2Country = "00";
        worldRegDomain_.dfsRegion = senf::emu::RegulatoryDomain::DFSRegion::Unset;
        worldRegDomain_.rules.push_back(senf::emu::RegulatoryRule()
                                          .frequencyRange(2402000, 2482000)
                                          .maxBandwidth(40000)
                                          .maxEIRP(3000) );
        worldRegDomain_.rules.push_back(senf::emu::RegulatoryRule()
                                          .frequencyRange(4900000, 6000000)
                                          .maxBandwidth(80000)
                                          .maxEIRP(3000) );
    }

    if (setRegDomainFromKernel()) {
        std::stringstream ss;
        senf::console::format(currentRegDomain_, ss);
        SENF_LOG( ("currentRegDomain initialized from kernel to " << ss.str()) );
    }
}

prefix_ bool senf::emu::CRDA::setRegDomainFromKernel()
{
    bool rtn (false);

    try {
        senf::emu::WirelessNLController wnlc;
        currentRegDomain_ = wnlc.get_regulatory();
        rtn = true;
    } catch (senf::ExceptionMixin & e) {
        SENF_LOG( ("Failed to retrieve regDomain from kernel due to: " << e.message()) );
    }

    return rtn;
}

prefix_ senf::emu::RegulatoryDomain const & senf::emu::CRDA::regulatoryDomain()
    const
{
    return currentRegDomain_;
}

prefix_ void senf::emu::CRDA::regulatoryDomain(senf::emu::RegulatoryDomain const & regDomain)
{
    currentRegDomain_ = regDomain;

    // remove DFS related flags
    if (!dfsMode_) {
        for (auto & rule : currentRegDomain_.rules) {
            rule.dfsRequired(false);
            rule.noIR(false);
        }
    }

}

prefix_ bool senf::emu::CRDA::setRegCountry(std::string alpha2Country)
{
    try {
        senf::emu::WirelessNLController wnlc;
        wnlc.set_regulatory_request(alpha2Country);
    } catch (senf::ExceptionMixin & e) {
        SENF_LOG( ("Setting Regulatory Country to " << alpha2Country << " failed: " << e.message()) );
        return false;
    }
    return true;
}

prefix_ bool senf::emu::CRDA::setNextDummyRegCountry()
{
    if (dummyCountry_[1]++ == 'Z') {
        dummyCountry_[1] = 'A';
        if (dummyCountry_[0]++ == 'Z') {
            dummyCountry_[0] = 'A';
        }
    }
    return setRegCountry(dummyCountry_);
}

prefix_ void senf::emu::CRDA::setWorldRegulatory(std::string const & alpha2Country)
{
    auto regDomain = worldRegDomain_;
    regDomain.alpha2Country = alpha2Country;
    setRegulatory(regDomain);
}

prefix_ void senf::emu::CRDA::setCurrentRegulatory(std::string const & alpha2Country)
{
    auto regDomain = currentRegDomain_;
    if (regDomain.alpha2Country.empty())
        regDomain = worldRegDomain_;
    regDomain.alpha2Country = alpha2Country;
    setRegulatory(regDomain);
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

prefix_ void senf::emu::CRDA::dumpRegulatory(std::ostream & os)
{
    senf::emu::RegulatoryDomain regDomain;
    try {
        senf::emu::WirelessNLController wnlc;
        regDomain = wnlc.get_regulatory();
    } catch (senf::ExceptionMixin & e) {
        os << "No Wireless Subsystem found. This node is a non-wireless box." << std::endl;
        return;
    }
    
    senf::console::format(regDomain, os);
    os << std::endl;
}


//
///////////////////////////////////////////////////////////////////////////
//

prefix_ void senf::emu::CRDA::parseConfigFile(std::string const & filename)
{
    senf::console::ConfigFile cfg (filename);
    cfg.ignoreMissing();
    cfg.parse(dir);
}

prefix_ void senf::emu::CRDA::help()
{
    print_help(EXIT_SUCCESS);
}

prefix_ void senf::emu::CRDA::print_help(int exit_status)
{
    std::cout << "Usage: crda [options]"
        "\n  options:"
        "\n     --help                       show this help"
        "\n     --setRegCountry=\"<alpha2>\"   notify the kernel about the current regulatory country."
        "\n     --setRegulatory=\"<alpha2>\"   push world regulatory rules to the kernel."
        "\n     --getRegulatory              dump current regulatory domain."
        "\n";
    exit(exit_status);
}

prefix_ int senf::emu::CRDA::run(int argc, char const ** argv)
{
    if (argc == 1)
        print_help(EXIT_SUCCESS);

    CRDA & crda (instance());

    namespace fty = senf::console::factory;
    senf::console::DirectoryNode & cmdDir (senf::console::root().add("init", fty::Directory()));
    cmdDir.add("help",          fty::Command( &CRDA::help, &crda) );
    cmdDir.add("setRegCountry", fty::Command( &CRDA::setRegCountry, &crda) );
    cmdDir.add("setRegulatory", fty::Command( &CRDA::setWorldRegulatory, &crda) );
    cmdDir.add("getRegulatory", fty::Command( &CRDA::dumpRegulatory, &crda) );
    cmdDir.add("config",        fty::Command( &CRDA::parseConfigFile, &crda) );
    senf::console::DirectoryNode & nodeDir (senf::console::root().add("node", fty::Directory()));
    nodeDir.add("crda", crda.dir);

    crda.logTarget_.route<senf::log::MESSAGE>();

    std::vector<std::string> nonOptions;
    senf::console::ProgramOptions cmdlineOptions (argc, argv, cmdDir);
    cmdlineOptions.nonOptions(nonOptions);
    try {
        cmdlineOptions.parse();
    } catch (senf::console::SyntaxErrorException const & e) {
        std::cerr << e.message() << std::endl;
        print_help(EXIT_FAILURE);
    }
    if (not nonOptions.empty())
        print_help(EXIT_FAILURE);

    return EXIT_SUCCESS;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
