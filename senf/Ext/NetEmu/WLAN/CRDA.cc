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

prefix_ unsigned senf::emu::CRDA::dfsRegionFlag()
    const
{
    return dfsRegionFlag_;
}

prefix_ senf::emu::CRDA::CRDA()
    : dummyCountry_(INITIAL_REG_ALPHA),
      dfsRegionFlag_(unsigned(RegulatoryDomain::DFSRegion::Unset)),
      logTag_("[senf::emu::CRDA " + senf::str(getpid()) + "] ")
{
    namespace fty = senf::console::factory;
    dir.add("kernelRegDomain",  fty::Command(&CRDA::kernelRegDomain, this));
    dir.add("worldRegDomain",   fty::Variable(boost::cref(worldRegDomain_)));
    dir.add("regDomain",        fty::Variable(boost::cref(currentRegDomain_)));
    dir.add("dfsRegionFlag",    fty::Variable(boost::cref(dfsRegionFlag_)));
    dir.add("syncFilename",     fty::Variable(boost::cref(syncFilename_)));
    dir.add("dummyCountry",     fty::Variable(boost::cref(dummyCountry_)));
    dir.add("cachedRegDomains", fty::Command(&CRDA::cachedRegDomains, this));

    try {
        std::fstream fs;
        fs.open("/sys/module/ath/parameters/dfsRegion", std::fstream::in);
        if (fs.is_open()) {
            fs >> dfsRegionFlag_;
            fs.close();
        }
    }
    catch (...) {};

    if (dfsRegionFlag() == unsigned(RegulatoryDomain::DFSRegion::Unset)) {
        logTag_ = "[senf::emu::CRDA_DEBUG " + senf::str(getpid()) + "] ";

        SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "DFS-Unset mode enabled.") );

        // Regd debug world regulatory domain
        worldRegDomain_.alpha2Country = "";  // leave this blank here !!!
        worldRegDomain_.dfsRegion = RegulatoryDomain::DFSRegion(dfsRegionFlag());
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
        worldRegDomain_.dfsRegion = RegulatoryDomain::DFSRegion(dfsRegionFlag());
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
                                     .maxEIRP(3000) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(2474000, 2494000)
                                     .maxBandwidth(20000)
                                     .maxEIRP(3000) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(3000000, 4000000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(4900000, 5170000)
                                     .maxBandwidth(80000)
                                     .maxEIRP(3000) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(5170000, 5250000)
                                     .maxBandwidth(80000)
                                     .maxEIRP(3000) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(5250000, 5330000)
                                     .maxBandwidth(80000)
                                     .maxEIRP(3000)
                                     .cacTime(60000)
                                     .dfsRequired(true) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(5470000, 5730000)
                                     .maxBandwidth(160000)
                                     .maxEIRP(3000)
                                     .cacTime(60000)
                                     .dfsRequired(true) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(5735000, 5835000)
                                     .maxBandwidth(80000)
                                     .maxEIRP(3000) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(5835000, 6100000)
                                     .maxBandwidth(80000)
                                     .maxEIRP(3000) );
        worldRegDomain_.rules.insert(RegulatoryRule()
                                     .frequencyRange(57240000, 63720000)
                                     .maxBandwidth(2160000)
                                     .maxEIRP(1000) );
    }
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
            currentRegDomain_ = worldRegDomain_;
            currentRegDomain_.alpha2Country = WORLD_REG_ALPHA;
            SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "No Wireless Subsystem found. This node might be a non-wireless box. Defaulting to build-in worldRegDomain " << currentRegDomain_) );
        }
    } else {
        logTag_ += "(SLV) ";
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

prefix_ senf::emu::RegulatoryDomain const & senf::emu::CRDA::worldRegDomain()
    const
{
    return worldRegDomain_;
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
        alpha2Country = std::string(a2);
    }

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

    if (a2 == "US" and !currentRegDomain_.alpha2Country.empty()) {
        SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "RegDomain US should not be requested from userspace. Ignoring request.") );
        return;
    }
    
    auto regDomain ((currentRegDomain_ && a2.compare("00") != 0 && a2.compare("US") != 0) ? currentRegDomain_ : worldRegDomain_);

    regDomain.alpha2Country = a2;

    try {
        WirelessNLController wnlc (false);  // do not disable seqNoChecks
        wnlc.set_regulatory(regDomain);
    } catch (senf::ExceptionMixin & e) {
        SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "Setting regulatory domain failed: " << e.message() << ", " << regDomain) );
        return;
    }
    
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

    char *a2 = getenv("COUNTRY");
    char *action = getenv("ACTION");

    if (!a2 and strcmp(action,"change") == 0)
        return -EINVAL;
        
    CRDA & crda (instance());

    crda.logTarget_.route<senf::log::MESSAGE>();

    namespace fty = senf::console::factory;
    senf::console::root().add("help",          fty::Command( &CRDA::help, &crda).arg(senf::console::kw::default_value = EXIT_SUCCESS) );
    senf::console::root().add("setRegCountry", fty::Command( &CRDA::setRegCountry, &crda).arg(senf::console::kw::default_value = "") );
    senf::console::root().add("setRegulatory", fty::Command( &CRDA::setRegulatory, &crda) );
    senf::console::root().add("getRegulatory", fty::Command( &CRDA::kernelRegDomain, &crda) );
    senf::console::root().add("regDomain",     fty::Variable(currentRegDomain_));
    senf::console::root().add("debug",         fty::Command( &CRDA::debugEnable, &crda) );

    SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "Action " << action << ", COUNTRY " << a2 << ", regDomain.alpha " << (currentRegDomain_ ? currentRegDomain_.alpha2Country : "(unset)")) );

    if (a2) {
        if ((strlen(a2) != 2) or ((strcmp(a2, "00") != 0) and (!isalpha(a2[0]) or !isalpha(a2[1])))) {
            SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "Illegal COUNTRY alpha: '" << a2 << "'. Ignoring request.") );
        return -EINVAL;
        }
    }

    if (strcmp(action,"change") == 0) {
        try {
            // Try to read and parse the redDBFile written by the main process
            // If present and valid, this will call setRegDomain
            senf::console::ConfigFile regDb (syncFilename_);
            regDb.ignoreMissing();
            regDb.parse(senf::console::root());
            ::unlink(syncFilename_.c_str());
        }
        catch(...) {};
    }

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


//
// The below has been taken from crda-3.13
//

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>
#include "nl80211-new.h"

struct nl80211_state {
        struct nl_sock *nl_sock;
        struct nl_cache *nl_cache;
        struct genl_family *nl80211;
};

static int nl80211_init(struct nl80211_state *state)
{
        int err;

        state->nl_sock = nl_socket_alloc();
        if (!state->nl_sock) {
                fprintf(stderr, "Failed to allocate netlink sock.\n");
                return -ENOMEM;
        }

        if (genl_connect(state->nl_sock)) {
                fprintf(stderr, "Failed to connect to generic netlink.\n");
                err = -ENOLINK;
                goto out_sock_destroy;
        }

        if (genl_ctrl_alloc_cache(state->nl_sock, &state->nl_cache)) {
                fprintf(stderr, "Failed to allocate generic netlink cache.\n");
                err = -ENOMEM;
                goto out_sock_destroy;
        }

        state->nl80211 = genl_ctrl_search_by_name(state->nl_cache, "nl80211");
        if (!state->nl80211) {
                fprintf(stderr, "nl80211 not found.\n");
                err = -ENOENT;
                goto out_cache_free;
        }

        return 0;

 out_cache_free:
        nl_cache_free(state->nl_cache);
 out_sock_destroy:
        nl_socket_free(state->nl_sock);
        return err;
}

static void nl80211_cleanup(struct nl80211_state *state)
{
    genl_family_put(state->nl80211);
    nl_cache_free(state->nl_cache);
    nl_socket_free(state->nl_sock);
}

static int reg_handler(struct nl_msg __attribute__((unused)) *msg,
                        void __attribute__((unused)) *arg)
{
    return NL_SKIP;
}

static int wait_handler(struct nl_msg __attribute__((unused)) *msg, void *arg)
{
    int *finished = (int*) arg;
    *finished = 1;
    return NL_STOP;
}

static int error_handler(struct sockaddr_nl __attribute__((unused)) *nla,
                            struct nlmsgerr *err,
                            void __attribute__((unused)) *arg)
{
    fprintf(stderr, "nl80211 error %d\n", err->error);
    exit(err->error);
}

static int put_reg_rule(senf::emu::RegulatoryRule const & rule, struct nl_msg *msg)
{
    NLA_PUT_U32( msg, NL80211_ATTR_REG_RULE_FLAGS,      rule.flags());
    NLA_PUT_U32( msg, NL80211_ATTR_FREQ_RANGE_START,    rule.frequencyRangeBegin());
    NLA_PUT_U32( msg, NL80211_ATTR_FREQ_RANGE_END,      rule.frequencyRangeEnd());
    NLA_PUT_U32( msg, NL80211_ATTR_FREQ_RANGE_MAX_BW,   rule.maxBandwidth());
    NLA_PUT_U32( msg, NL80211_ATTR_POWER_RULE_MAX_ANT_GAIN, rule.maxAntennaGain());
    NLA_PUT_U32( msg, NL80211_ATTR_POWER_RULE_MAX_EIRP, rule.maxEIRP());
    
    return 0;
    
 nla_put_failure:
    return -1;
}

prefix_ int senf::emu::CRDA::setRegulatoryDirect()
{
    std::string a2 (getenv("COUNTRY") ? getenv("COUNTRY") : "");

    if(a2.empty()) {
        SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "COUNTRY not set. Ignoring request.") );
        exit(-EINVAL); 
    }

    if (a2 == "US" and !currentRegDomain_.alpha2Country.empty()) {
        SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "RegDomain US should not be requested from userspace. Ignoring request.") );
        exit(-EINVAL);
    }
    
    auto regDomain ((currentRegDomain_ && a2.compare("00") != 0 && a2.compare("US") != 0) ? currentRegDomain_ : worldRegDomain_);

    regDomain.alpha2Country = a2;

    //
    // From here, we exec the original CRDA code using our regDomain
    //

    SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "Executing CRDA code to setRegulatory for " << regDomain) );
    
    int i = 0, r;
    struct nl80211_state nlstate;
    struct nl_cb *cb = NULL;
    struct nl_msg *msg;
    int finished = 0;
    struct nlattr *nl_reg_rules;

        
    r = nl80211_init(&nlstate);
    if (r) {
        exit(-EIO);
    }
    
    msg = nlmsg_alloc();
    if (!msg) {
        r = -1;
        goto out;
    }
    
    genlmsg_put(msg, 0, 0, genl_family_get_id(nlstate.nl80211), 0,
                0, NL80211_CMD_SET_REG, 0);
    
    NLA_PUT_STRING(msg, NL80211_ATTR_REG_ALPHA2, regDomain.alpha2Country.c_str());
    NLA_PUT_U8(msg, NL80211_ATTR_DFS_REGION, std::uint8_t(regDomain.dfsRegion));
    
    nl_reg_rules = nla_nest_start(msg, NL80211_ATTR_REG_RULES);
    if (!nl_reg_rules) {
        goto nla_put_failure;
    }

    for (RegulatoryRule const & rule : regDomain.rules) {
        struct nlattr *nl_reg_rule;
        nl_reg_rule = nla_nest_start(msg, i);
        if (!nl_reg_rule)
            goto nla_put_failure;
        
        r = put_reg_rule(rule, msg);
        if (r)
            goto nla_put_failure;
        
        nla_nest_end(msg, nl_reg_rule);
    }
    
    nla_nest_end(msg, nl_reg_rules);
    
    cb = nl_cb_alloc(NL_CB_CUSTOM);
    if (!cb)
        goto cb_out;

    r = nl_send_auto_complete(nlstate.nl_sock, msg);
    
    if (r < 0) {
        goto cb_out;
    }
    
    nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, reg_handler, NULL);
    nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, wait_handler, &finished);
    nl_cb_err(cb, NL_CB_CUSTOM, error_handler, NULL);
    
    if (!finished) {
        r = nl_wait_for_ack(nlstate.nl_sock);
        if (r < 0) {
            goto cb_out;
        }
    }
    
 cb_out:
    nl_cb_put(cb);
 nla_put_failure:
    nlmsg_free(msg);
 out:
    nl80211_cleanup(&nlstate);
    
    if (r) {
        SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "Error (" << r << ") pushing regDomain to kernel as " << regDomain) );
    } else {
        SENF_LOG( (senf::log::IMPORTANT) (logTag_ << "Regulatory rules pushed to kernel as " << regDomain) );
    }

    exit(r);
}



///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
