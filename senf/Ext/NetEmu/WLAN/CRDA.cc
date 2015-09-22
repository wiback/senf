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


#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

#define WORLD_REG_ALPHA "00"
#define INITIAL_REG_ALPHA "AA"

prefix_ senf::emu::CRDA & senf::emu::CRDA::instance()
{
    static CRDA instance_;
    return instance_;
}

prefix_ std::string senf::emu::CRDA::slaveName()
    const
{
    return CRDA_SLAVE_NAME;
}

prefix_ senf::emu::CRDA::CRDA()
    : dummyCountry_(INITIAL_REG_ALPHA),
      dfsMode_(true),
      nonWirelessBox_(false)
{
    namespace fty = senf::console::factory;
    dir.add("kernelRegDomain",  fty::Command(&CRDA::kernelRegDomain, this));
    dir.add("worldRegDomain",   fty::Variable(boost::cref(worldRegDomain_)));
    dir.add("regDomain",        fty::Variable(boost::cref(currentRegDomain_)));
    dir.add("dfsMode",          fty::Variable(boost::cref(dfsMode_)));
    dir.add("nonWirelessBox",   fty::Variable(boost::cref(nonWirelessBox_)));
    dir.add("syncFilename",     fty::Variable(boost::cref(syncFilename_)));
    dir.add("dummyCountry",     fty::Variable(boost::cref(dummyCountry_)));
    dir.add("cachedRegDomains", fty::Command(&CRDA::cachedRegDomains, this));

    // sync our CRDA behavior with the kernel setting (boot time argument, for now)
    int fd (::open("/proc/cmdline", O_RDONLY));
    if (fd != -1) {
        char buf[8192];
        size_t rd;
        if ((rd = ::read(fd, buf, sizeof(buf)-1)) > 0) {
            buf[rd] = '\0';
            dfsMode_ = strstr(buf, "WiBACK-CRDA") == NULL;
            if (!dfsMode_) {
                SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] WiBACK-CRDA mode enabled.") );
            }
        }
        ::close(fd);
    }

    if (dfsMode_) {
        // DFS-aware world regulatory domain
        worldRegDomain_.alpha2Country = "";
        worldRegDomain_.dfsRegion = RegulatoryDomain::DFSRegion::Unset;
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(700000, 800000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(2402000, 2472000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(2457000, 2482000)
                                     .maxBandwidth(20000)
                                     .maxEIRP(3000)
                                     .noIR(true) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(3000000, 4000000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(5170000, 5250000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000)
                                     .noIR(true) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(5250000, 5330000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000)
                                     .noIR(true)
                                     .dfsRequired(true) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(5490000, 5730000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000)
                                     .noIR(true)
                                     .dfsRequired(true) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(5735000, 5835000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000)
                                     .noIR(true) );
    } else {
        // DFS-disabled world regulatory domain
        worldRegDomain_.alpha2Country = "";
        worldRegDomain_.dfsRegion = RegulatoryDomain::DFSRegion::Unset;
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(700000, 800000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(2402000, 2494000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(3000000, 4000000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(4900000, 6100000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000) );
    }
}

prefix_ bool  senf::emu::CRDA::pushRegulatory( senf::emu::RegulatoryDomain & reg)
{
    try {
        WirelessNLController wnlc;
        wnlc.set_regulatory( reg);
    } catch (senf::ExceptionMixin & e) {
        SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] Setting regulatory domain failed: " << e.message() << ", " << reg) );
        return false;
    }
    return true;
}

prefix_ bool senf::emu::CRDA::init(bool masterMode, std::string const & filename)
{
    if (filename.empty())
        return false;
    syncFilename_ = filename;

    if (masterMode) {
        try {
            std::fstream fs;
            fs.open(syncFilename_, std::fstream::out | std::fstream::trunc);
            fs.close();
        }
        catch(...) {
            return false;
        }
    }

    if (masterMode) {
        try {
            WirelessNLController wnlc;
            currentRegDomain_ = wnlc.get_regulatory();
            // initialize our country counter
            dummyCountry_ = currentRegDomain_.alpha2Country.compare( WORLD_REG_ALPHA) == 0 ? INITIAL_REG_ALPHA : currentRegDomain_.alpha2Country;
            SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] currentRegDomain initialized from kernel to " << currentRegDomain_ << ". 'DummyCountry' initialized to " << dummyCountry_ << ".") );
            // store the new mapping (if the mapping already exists, this does nothing)
            cachedRegDomains_.insert(currentRegDomain_);

//            auto domainAA( worldRegDomain_);
//            domainAA.alpha2Country = WORLD_REG_ALPHA;
//            cachedRegDomains_.insert( domainAA);
//            regDomain( domainAA);
//
//            ::usleep(500000);  // 0.5s
//
//            auto domainUS( worldRegDomain_);
//            domainUS.alpha2Country = "US";
//            cachedRegDomains_.insert( domainUS);
//            regDomain( domainUS);

        } catch (...) {
            nonWirelessBox_ = true;
            currentRegDomain_ = worldRegDomain_;
            currentRegDomain_.alpha2Country = WORLD_REG_ALPHA;
            SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] No Wireless Subsystem found. This node might be a non-wireless box. Defaulting to build-in worldRegDomain " << currentRegDomain_) );
        }
    } else {
        try {
            WirelessNLController wnlc;
            wnlc.get_regulatory();
        } catch (...) {
            nonWirelessBox_ = true;
            SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] No Wireless Subsystem found.") );
        }
    }
    
    return true;
}

prefix_ void senf::emu::CRDA::cachedRegDomains(std::ostream & os)
    const
{
    os << "Number of cached regDomains: " << cachedRegDomains_.size() << std::endl;

    for (auto const & rd : cachedRegDomains_){
        os << rd << std::endl;
    }
}

prefix_ senf::emu::RegulatoryDomain const & senf::emu::CRDA::regDomain()
    const
{
    return currentRegDomain_;
}

prefix_ bool senf::emu::CRDA::equalsKernel()
{
    try {
        WirelessNLController wnlc;
        if (!currentRegDomain_.isEqualKernel(wnlc.get_regulatory())) {
            SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] Kernel regDomain " << wnlc.get_regulatory() << " does not match CRDA regDoamin " << currentRegDomain_) );
            return false;
        }
        return true;
    }
    catch(...) {
        SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] No Wireless Subsystem found. This node might be a non-wireless box. Hence equalsKernel() is always true") );
    }

    return true;
}

prefix_ bool senf::emu::CRDA::regDomain(senf::emu::RegulatoryDomain regDomain)
{
    if (!regDomain) {
        regDomain = worldRegDomain_;
        regDomain.alpha2Country = WORLD_REG_ALPHA;
    }  else {
        SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] regDomain cache lookup for " << regDomain) );
        // check if we already have a mapping for this regDomain
        auto const it (cachedRegDomains_.find(regDomain));
        if (it != cachedRegDomains_.end()) {
            // yes: re-use the alpha2Country
            regDomain.alpha2Country = it->alpha2Country;
            SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] cached regDomain found as " << *it) );
        } else {
            // no: generate a new alpha2country
            if (dummyCountry_[1]++ == 'Z') {
                dummyCountry_[1] = 'A';
                if (dummyCountry_[0]++ == 'Z') {
                    dummyCountry_[0] = 'A';
                }
            }
            // US seem to be special countries, so we avoid using it
            if( dummyCountry_.compare("US") == 0)
                dummyCountry_ = "UT";
            regDomain.alpha2Country = dummyCountry_;
            SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] regDomain not found in cache. Creating new regDomain as " << regDomain) );
        }
    }
    
    // store the new mapping (if the mapping already exists, this does nothing)
    auto res (cachedRegDomains_.insert(regDomain));
    if (!res.second) {
        SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] not adding regDomain " << regDomain << " to cache, as entry " << *(res.first) << " is already present.") );
    }

    // we might need to revert, if the below fails
    RegulatoryDomain old (currentRegDomain_);
    
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
    
    if (setRegCountry(currentRegDomain_.alpha2Country)) {
        return true;
    }
    
    currentRegDomain_ = old;
    return false;
}

prefix_ bool senf::emu::CRDA::setRegCountry(std::string alpha2Country)
{
    if (alpha2Country.empty())
        alpha2Country = WORLD_REG_ALPHA;

    if (nonWirelessBox_)
        return true;

    try {
        SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] writing regDomain tmpfile " << currentRegDomain_) );
        std::fstream fs;
        fs.open(syncFilename_, std::fstream::out | std::fstream::trunc);
        fs << "regDomain " << currentRegDomain_ << ";" << std::endl;
        fs.close();
    }
    catch(...) {
        SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] error creating regDomain tmpfile at " << syncFilename_ << ". Reason given: " << strerror(errno)) );
        return false;
    }
    
    currentRegDomain_.alpha2Country = alpha2Country;

    try {
        WirelessNLController wnlc;
        wnlc.set_regulatory_request(alpha2Country);
    } catch (senf::ExceptionMixin & e) {
        SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] setting Regulatory Country to " << alpha2Country << " failed: " << e.message()) );
        return false;
    }
    SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] requested Regulatory Country " << alpha2Country ) );

    return true;
}

prefix_ void senf::emu::CRDA::kernelRegDomain(std::ostream & os)
{
    try {
        WirelessNLController wnlc;
        os        << wnlc.get_regulatory() << std::endl;
        std::cout << wnlc.get_regulatory() << std::endl;
    } catch (...) {
        os        << "No Wireless Subsystem found. This node might be a non-wireless box." << std::endl;
        std::cout << "No Wireless Subsystem found. This node might be a non-wireless box." << std::endl;
    }
}

//
///////////////////////////////////////////////////////////////////////////
//

prefix_ void senf::emu::CRDA::setRegulatory()
{
    char *a2 = getenv("COUNTRY");

    if( !a2) {
        SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] No a2 country code provided. Ignore request") );
        return;
    }

    std::string c2( a2 ? a2 : WORLD_REG_ALPHA);

    auto regDomain (currentRegDomain_ && c2.compare("00") != 0 && c2.compare("US") != 0 ? currentRegDomain_ : worldRegDomain_);

    if( regDomain.alpha2Country.empty())
            regDomain.alpha2Country = c2;

//    if( c2.length() != 2 or c2.compare( regDomain.alpha2Country) != 0) {
//        SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] Requested country is not the same: "<< c2 << " expected "<<  regDomain.alpha2Country << ".") );
//    }

    if( not pushRegulatory( regDomain))
        return;

    SENF_LOG( (senf::log::IMPORTANT) ("[senf::emu::CRDA] Regulatory rules pushed to kernel as " << regDomain) );
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

    crda.logTarget_.route<senf::log::MESSAGE>();

    namespace fty = senf::console::factory;
    senf::console::root().add("help",          fty::Command( &CRDA::help, &crda).arg(senf::console::kw::default_value = EXIT_SUCCESS) );
    senf::console::root().add("setRegCountry", fty::Command( &CRDA::setRegCountry, &crda).arg(senf::console::kw::default_value = "") );
    senf::console::root().add("setRegulatory", fty::Command( &CRDA::setRegulatory, &crda) );
    senf::console::root().add("getRegulatory", fty::Command( &CRDA::kernelRegDomain, &crda) );
    senf::console::root().add("regDomain",     fty::Variable(currentRegDomain_));

    try {
        // Try to read and parse the redDBFile written by the main main process
        // If present and valid, this will call setRegDomain
        senf::console::ConfigFile regDb (syncFilename_);
        regDb.ignoreMissing();
        regDb.parse(senf::console::root());
    }
    catch(...) {};
    
    // udev rule file will add '--setRegulatory' arg to command line
    //    declare -x ACTION="change"
    //    declare -x COUNTRY="AA"
    //    declare -x DEVPATH="/devices/platform/regulatory.0"
    //    declare -x MODALIAS="platform:regulatory"
    //    declare -x OLDPWD
    //    declare -x PWD="/"
    //    declare -x SEQNUM="1699"
    //    declare -x SHLVL="1"
    //    declare -x SUBSYSTEM="platform"
    //    declare -x UDEV_LOG="3"
    //    declare -x USEC_INITIALIZED="62694593"

    std::vector<std::string> nonOptions;
    senf::console::ProgramOptions cmdlineOptions (argc, argv, senf::console::root());
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
