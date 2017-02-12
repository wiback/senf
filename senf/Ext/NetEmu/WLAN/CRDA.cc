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

prefix_ bool senf::emu::CRDA::debugRegd()
    const
{
    return debugRegd_;
}

prefix_ senf::emu::CRDA::CRDA()
    : dummyCountry_(INITIAL_REG_ALPHA),
      debugRegd_(false),
      nonWirelessBox_(false),
      logTag_("[senf::emu::CRDA " + senf::str(getpid()) + "/"  + senf::str(senf::ClockService::in_milliseconds(senf::ClockService::now()) % 100000) + "] ")
{
    namespace fty = senf::console::factory;
    dir.add("kernelRegDomain",  fty::Command(&CRDA::kernelRegDomain, this));
    dir.add("worldRegDomain",   fty::Variable(boost::cref(worldRegDomain_)));
    dir.add("regDomain",        fty::Variable(boost::cref(currentRegDomain_)));
    dir.add("debugRegd",        fty::Variable(boost::cref(debugRegd_)));
    dir.add("nonWirelessBox",   fty::Variable(boost::cref(nonWirelessBox_)));
    dir.add("syncFilename",     fty::Variable(boost::cref(syncFilename_)));
    dir.add("dummyCountry",     fty::Variable(boost::cref(dummyCountry_)));
    dir.add("cachedRegDomains", fty::Command(&CRDA::cachedRegDomains, this));

    // sync our CRDA behavior with the ath.ko module's debugRegd setting
    int fd (::open("/sys/module/ath/parameters/debugRegd", O_RDONLY));
    if (fd != -1) {
        char buf[1];
        if ((::read(fd, buf, sizeof(buf))) > 0) {
            debugRegd_ = (buf[0] == '1');
        }
        ::close(fd);
    }
    else {
        debugRegd_ = getenv("DEBUGREGD");
    }

    if (debugRegd()) {
        logTag_ = "[senf::emu::CRDA_DEBUG " + senf::str(getpid()) + "/"  + senf::str(senf::ClockService::in_milliseconds(senf::ClockService::now()) % 100000) + "] ";

        SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "debugRegd mode enabled.") );

        // Regd debug world regulatory domain
        worldRegDomain_.alpha2Country = "";  // leave this blank here !!!
        worldRegDomain_.dfsRegion = RegulatoryDomain::DFSRegion::Unset;
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(700000, 800000)
                                     .maxBandwidth(20000)
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
                                     .maxBandwidth(80000)
                                     .maxEIRP(3000) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(57240000, 63720000)
                                     .maxBandwidth(2160000)
                                     .maxEIRP(1000) );
    } else {
        // regular world regulatory domain
        worldRegDomain_.alpha2Country = "";  // leave this blank here !!!
        worldRegDomain_.dfsRegion = RegulatoryDomain::DFSRegion::Unset;
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(700000, 800000)
                                     .maxBandwidth(20000)
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
                                     .maxBandwidth(80000)
                                     .maxEIRP(3000)
                                     .noIR(true) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(5250000, 5330000)
                                     .maxBandwidth(80000)
                                     .maxEIRP(3000)
                                     .cacTime(0)
                                     .noIR(true)
                                     .dfsRequired(true) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(5490000, 5730000)
                                     .maxBandwidth(80000)
                                     .maxEIRP(3000)
                                     .cacTime(0)
                                     .noIR(true)
                                     .dfsRequired(true) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(5735000, 5835000)
                                     .maxBandwidth(80000)
                                     .maxEIRP(3000)
                                     .noIR(true) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(57240000, 63720000)
                                     .maxBandwidth(2160000)
                                     .maxEIRP(1000) );
    }
}

prefix_ bool  senf::emu::CRDA::pushRegulatory( senf::emu::RegulatoryDomain & reg)
{
    try {
        WirelessNLController wnlc;
        wnlc.set_regulatory( reg);
    } catch (senf::ExceptionMixin & e) {
        SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "Setting regulatory domain failed: " << e.message() << ", " << reg) );
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
            if (currentRegDomain_.alpha2Country.compare(WORLD_REG_ALPHA) == 0 or currentRegDomain_.alpha2Country.compare("98") == 0) 
                dummyCountry_ = INITIAL_REG_ALPHA;
            else
                dummyCountry_ = currentRegDomain_.alpha2Country;
            SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "currentRegDomain initialized from kernel to " << currentRegDomain_ << ". 'DummyCountry' initialized to " << dummyCountry_ << ".") );
            // store the new mapping (if the mapping already exists, this does nothing)
            cachedRegDomains_.insert(currentRegDomain_);
        } catch (...) {
            nonWirelessBox_ = true;
            currentRegDomain_ = worldRegDomain_;
            currentRegDomain_.alpha2Country = WORLD_REG_ALPHA;
            SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "No Wireless Subsystem found. This node might be a non-wireless box. Defaulting to build-in worldRegDomain " << currentRegDomain_) );
        }
    } else {
        try {
            WirelessNLController wnlc;
            wnlc.get_regulatory();
        } catch (...) {
            nonWirelessBox_ = true;
            SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "No Wireless Subsystem found.") );
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
            SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "Kernel regDomain " << wnlc.get_regulatory() << " does not match CRDA regDoamin " << currentRegDomain_) );
            return false;
        }
        return true;
    }
    catch(...) {
        SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "No Wireless Subsystem found. This node might be a non-wireless box. Hence equalsKernel() is always true") );
    }

    return true;
}

prefix_ bool senf::emu::CRDA::regDomain(senf::emu::RegulatoryDomain regDomain)
{
    if (!regDomain) {
        regDomain = worldRegDomain_;
        regDomain.alpha2Country = WORLD_REG_ALPHA;
    }  else {
        SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "regDomain cache (" << cachedRegDomains_.size() << ") lookup for " << regDomain) );
        // check if we already have a mapping for this regDomain
        auto const it (cachedRegDomains_.find(regDomain));
        if (it != cachedRegDomains_.end()) {
            // yes: re-use the alpha2Country
            regDomain.alpha2Country = it->alpha2Country;
            SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "Cached regDomain found as " << *it) );
        } else {
            // no: generate a new alpha2country
            if (dummyCountry_[1]++ == 'Z') {
                dummyCountry_[1] = 'A';
                if (dummyCountry_[0]++ == 'Z') {
                    dummyCountry_[0] = 'A';
                }
            }
            // 'US' seem to be a special country code, so we avoid using it
            if( dummyCountry_.compare("US") == 0)
                dummyCountry_ = "UT";
            regDomain.alpha2Country = dummyCountry_;
            SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "regDomain not found in cache. Creating new regDomain as " << regDomain) );
        }
    }
    
    // store the new mapping (if the mapping already exists, this does nothing)
    auto res (cachedRegDomains_.insert(regDomain));
    if (!res.second) {
        SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "Not adding regDomain " << regDomain << " to cache, as entry " << *(res.first) << " is already present.") );
    }

    // we might need to revert, if the below fails
    RegulatoryDomain old (currentRegDomain_);
    
    // set the new regDomain first, to avoid possible race conditions with the kernel CRDA upcall
    currentRegDomain_ = regDomain;
    currentRegDomain_.rules.clear();
    for (auto & rule : regDomain.rules) {
        if (debugRegd_) {
            auto r (rule);
            r.dfsRequired(false);
            r.noIR(false);
            currentRegDomain_.rules.insert(r);
        } else {
            currentRegDomain_.rules.insert(rule);
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
    if (alpha2Country.empty()) {
        char *a2 = getenv("COUNTRY");
        if (!a2 or strlen(a2) == 0) {
            SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "COUNTRY not set. Ignoring Request.") );
            return false;
        }
    }

    if (nonWirelessBox_)
        return true;

    try {
        SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "Writing regDomain tmpfile " << currentRegDomain_) );
        std::fstream fs;
        fs.open(syncFilename_, std::fstream::out | std::fstream::trunc);
        fs << "regDomain " << currentRegDomain_ << ";" << std::endl;
        fs.close();
    }
    catch(...) {
        SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "Error creating regDomain tmpfile at " << syncFilename_ << ". Reason given: " << strerror(errno)) );
        return false;
    }
    
    currentRegDomain_.alpha2Country = alpha2Country;

    try {
        WirelessNLController wnlc;
        wnlc.set_regulatory_request(alpha2Country);
    } catch (senf::ExceptionMixin & e) {
        SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "Setting Regulatory Country to " << alpha2Country << " failed: " << e.message()) );
        return false;
    }
    SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "Requested Regulatory Country " << alpha2Country ) );

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
    std::string a2 (getenv("COUNTRY") ? getenv("COUNTRY") : "");

    if(a2.empty()) {
        SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "COUNTRY not set. Ignoring request.") );
        return;
    }

    auto regDomain (currentRegDomain_ && a2.compare("00") != 0 && a2.compare("US") != 0 ? currentRegDomain_ : worldRegDomain_);

    if( regDomain.alpha2Country.empty())
            regDomain.alpha2Country = a2;

    if( not pushRegulatory( regDomain))
        return;

    SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "Regulatory rules pushed to kernel as " << regDomain) );
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
        "\n     --debug                      log debug info into /tmp/crda.log"
        "\n";
    exit(exit_status);
}

prefix_ void senf::emu::CRDA::debugEnable()
{
    logDebugTarget_.reset( new senf::log::FileTarget( "/tmp/crda.log"));
    logDebugTarget_->route<senf::log::NOTICE>();
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
    senf::console::root().add("debug",         fty::Command( &CRDA::debugEnable, &crda) );

    try {
        // Try to read and parse the redDBFile written by the main main process
        // If present and valid, this will call setRegDomain
        senf::console::ConfigFile regDb (syncFilename_);
        regDb.ignoreMissing();
        regDb.parse(senf::console::root());
    }
    catch(...) {};
    
    char *a2 = getenv("COUNTRY");
    char *action = getenv("ACTION");
    SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "Action " << action << ", COUNTRY " << (a2 ? a2 : "(unset)") << ", regDomain.alpha " << (currentRegDomain_ ? currentRegDomain_.alpha2Country : "(unset)")) );

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
