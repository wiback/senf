// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// http://senf.berlios.de/license.html
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief Daemon non-inline non-template implementation */

#include "Daemon.hh"
#include "Daemon.ih"

// Custom includes
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#ifdef SENF_BACKTRACE
   #include <execinfo.h>
#endif
#include <sstream>
#include <fstream>
#include <algorithm>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <senf/Utils/Exception.hh>
#include <senf/Utils/membind.hh>
#include <senf/Utils/Backtrace.hh>
#include <senf/Utils/signalnames.hh>

// #define __USE_GNU
#include <ucontext.h>

//#include "Daemon.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#define LIBC_CALL(fn, args) if (fn args < 0) \
    SENF_THROW_SYSTEM_EXCEPTION(#fn "()")

#define LIBC_CALL_RV(var, fn, args) \
    int var (fn args); if (var < 0) SENF_THROW_SYSTEM_EXCEPTION(#fn "()")

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::Daemon

prefix_ senf::Daemon::~Daemon()
{
    if (pidfileCreated_) {
        try {
            LIBC_CALL( ::unlink, (pidfile_.c_str()) );
        } catch (Exception & e) {
            // e << "; could not unlink " << pidfile_.c_str();
            // throw;
        }
    }
}

prefix_ void senf::Daemon::daemonize(bool v)
{
    daemonize_ = v;
}

prefix_ void senf::Daemon::catchExceptions(bool flag)
{
    catchExceptions_ = flag;
}

prefix_ bool senf::Daemon::daemon()
{
    return daemonize_;
}

prefix_ int senf::Daemon::argc()
{
    return argc_;
}

prefix_ char const ** senf::Daemon::argv()
{
    return argv_;
}

namespace {

    struct IsDaemonOpt {
        bool operator()(std::string const & str) const {
            return str == "--no-daemon"
                || boost::starts_with(str, std::string("--pid-file="))
                || boost::starts_with(str, std::string("--console-log="));
        }
    };
}

prefix_ void senf::Daemon::removeDaemonArgs()
{
    char const ** last (std::remove_if(argv_+1, argv_+argc_, IsDaemonOpt()));
    *last = 0;
    argc_ = last - argv_;
}

prefix_ void senf::Daemon::consoleLog(std::string const & path, StdStream which)
{
    switch (which) {
    case StdOut : stdoutLog_ = path; break;
    case StdErr : stderrLog_ = path; break;
    case Both : stdoutLog_ = path; stderrLog_ = path; break;
    }
}


prefix_ void senf::Daemon::openLog()
{
    int fd (-1);
    if (! stdoutLog_.empty()) {
        fd = ::open(stdoutLog_.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0666);
        if (fd < 0)
            SENF_THROW_SYSTEM_EXCEPTION(
                  " Could not open \"" + stdoutLog_ + "\" for redirecting stdout.");
        stdout_ = fd;
    }
    if (! stderrLog_.empty()) {
        if (stderrLog_ == stdoutLog_) {
            stderr_ = ::dup(fd);
            if (stderr_ < 0)
                SENF_THROW_SYSTEM_EXCEPTION("::dup()");
        }
        else {
            fd = ::open(stdoutLog_.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0666);
            if (fd < 0)
                SENF_THROW_SYSTEM_EXCEPTION(
                    " Could not open \"" + stderrLog_ + "\" for redirecting stderr.");
            stderr_ = fd;
        }
    }
}

prefix_ void senf::Daemon::logReopen()
{
    if (! stdoutLog_.empty()) {
        int fd (::open(stdoutLog_.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0666));
        if (fd < 0)
            goto error;
        if (::dup2(fd, 1) < 0)
            goto error;
        if (stderrLog_ == stdoutLog_) {
            if (::dup2(fd, 2) < 0)
                goto error;
            return;
        }
    }
    if (! stderrLog_.empty()) {
        int fd (::open(stderrLog_.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0666));
        if (fd < 0)
            goto error;
        if (::dup2(fd, 2) < 0)
            goto error;
    }
    return;

 error:
    SENF_LOG(
        (senf::log::CRITICAL)
        ("log-file reopen failed: (" << errno << ") " << ::strerror(errno)) );
}

prefix_ void senf::Daemon::pidFile(std::string const & f)
{
    pidfile_ = f;
}

namespace {
    bool signaled (false);
    void waitusr(int) {
        signaled = true;
    }
}

prefix_ void senf::Daemon::detach()
{
    if (daemonize_ && ! detached_) {
        // Wow .. ouch ..
        // To ensure all data is written to the console log file in the correct order, we suspend
        // execution here until the parent process tells us to continue via SIGUSR1: We block
        // SIGUSR1 and install our own signal handler saving the old handler and signal mask. Then
        // we close stdin/stderr which will send a HUP condition to the parent process. We wait for
        // SIGUSR1 and reinstall the old signal mask and action.
        ::sigset_t oldsig;
        ::sigset_t usrsig;
        ::sigemptyset(&usrsig);
        LIBC_CALL( ::sigaddset, (&usrsig, SIGUSR1) );
        LIBC_CALL( ::sigprocmask, (SIG_BLOCK, &usrsig, &oldsig) );
        struct ::sigaction oldact;
        struct ::sigaction usract;
        ::memset(&usract, 0, sizeof(usract));
        usract.sa_handler = &waitusr;
        LIBC_CALL( ::sigaction, (SIGUSR1, &usract, &oldact) );
        ::sigset_t waitsig (oldsig);
        LIBC_CALL( ::sigdelset, (&waitsig, SIGUSR1) );

        signaled = false;

        LIBC_CALL_RV( nul, ::open, ("/dev/null", O_WRONLY) );
        LIBC_CALL( ::dup2, (stdout_ == -1 ? nul : stdout_, 1) );
        LIBC_CALL( ::dup2, (stderr_ == -1 ? nul : stderr_, 2) );
        LIBC_CALL( ::close, (nul) );

        while (! signaled) {
            ::sigsuspend(&waitsig);
            if (errno != EINTR)
                SENF_THROW_SYSTEM_EXCEPTION("::sigsuspend()");
        }

        LIBC_CALL( ::sigaction, (SIGUSR1, &oldact, 0) );
        LIBC_CALL( ::sigprocmask, (SIG_SETMASK, &oldsig, 0) );

        detached_ = true;
    }
}

namespace {
    /* Purposely *not* derived from std::exception */
    struct DaemonExitException {
        DaemonExitException(unsigned c) : code(c) {}
        unsigned code;
    };
}

prefix_ void senf::Daemon::exit(unsigned code)
{
    throw DaemonExitException(code);
}

prefix_ int senf::Daemon::start(int argc, char const ** argv)
{
    argc_ = argc;
    argv_ = argv;

    try {
        configure();

        if (daemonize_) {
            openLog();
            fork();
        }
        installSighandlers();
        if (! pidfile_.empty()) {
            if (pidfileCreate())
                pidfileCreated_ = true;
            else {
                std::cerr << "PID file '" << pidfile_
                          << "' creation failed. Daemon running ?" << std::endl;
                return 1;
            }
        }

        main();
    }
    catch (DaemonExitException & e) {
        return e.code;
    }
    catch (std::exception & e) {
        if (catchExceptions_) {
            std::cerr << "\n*** Fatal exception: " << e.what() << "\n" << std::endl;
            return 1;
        } else {
            throw;
        }
    }
    catch (...) {
        if (catchExceptions_) {
            std::cerr << "\n*** Fatal exception: (unknown)" << "\n" << std::endl;
            return 1;
        } else {
            throw;
        }
    }

    return 0;
}

prefix_ senf::Daemon & senf::Daemon::instance()
{
    BOOST_ASSERT( instance_ );
    return *instance_;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// protected members

prefix_ senf::Daemon::Daemon()
    : argc_(0), argv_(0), daemonize_(true), stdout_(-1), stderr_(-1), pidfile_(""),
      pidfileCreated_(false), detached_(false)
{
    BOOST_ASSERT( ! instance_ );
    instance_ = this;
#ifdef SENF_DEBUG
    catchExceptions_ = false;
#else
    catchExceptions_ = true;
#endif
}

senf::Daemon * senf::Daemon::instance_ (0);

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// private members

prefix_ void senf::Daemon::configure()
{
    // int i (not unsigned) since argc_ is int ...
    for (int i (1); i<argc_; ++i) {
        std::string argv (argv_[i]);
        if (argv == "--no-daemon")
            daemonize(false);
        else if (boost::starts_with(argv, "--console-log=")) {
            std::string arg (argv.substr(14u));
            std::string::size_type komma (arg.find(','));
            if (komma == std::string::npos) {
                boost::trim(arg);
                if (arg == std::string("none")) consoleLog("");
                else if (!arg.empty())          consoleLog(arg);
            } else {
                std::string arg1 (arg,0,komma);
                std::string arg2 (arg,komma+1);
                boost::trim(arg1);
                boost::trim(arg2);
                if (arg1 == std::string("none")) consoleLog("",StdOut);
                else if (! arg1.empty() )        consoleLog(arg1, StdOut);
                if (arg2 == std::string("none")) consoleLog("",StdErr);
                else if (! arg2.empty() )        consoleLog(arg2, StdErr);
            }
        }
        else if (boost::starts_with(argv, "--pid-file="))
            pidFile(std::string(std::string(argv_[i]), 11u));
    }
}

prefix_ void senf::Daemon::main()
{
    init();
    detach();
    run();
}

prefix_ void senf::Daemon::init()
{}

prefix_ void senf::Daemon::run()
{}

prefix_ void senf::Daemon::fork()
{
    int coutpipe[2];
    int cerrpipe[2];

    LIBC_CALL_RV( nul, ::open, ("/dev/null", O_RDONLY) );
    LIBC_CALL( ::dup2, (nul, 0) );
    LIBC_CALL( ::close, (nul) );
    LIBC_CALL( ::pipe, (coutpipe) );
    LIBC_CALL( ::pipe, (cerrpipe) );

    // We need to block the SIGCHLD signal here so we don't miss it, if the child
    // dies immediately
    ::sigset_t oldsig;
    ::sigset_t cldsig;
    ::sigemptyset(&cldsig);
    LIBC_CALL( ::sigaddset, (&cldsig, SIGCHLD) );
    LIBC_CALL( ::sigprocmask, (SIG_BLOCK, &cldsig, &oldsig) );

    if (! senf::scheduler::empty() ) {
        std::cerr <<
            "\n"
            "*** WARNING ***\n"
            "Scheduler not empty before fork(). THIS MUST NOT HAPPEN.\n"
            "The scheduler will be reinitialized by the fork() and lose all registrations.\n\n";
        senf::scheduler::detail::EventManager::instance().listEvents(std::cerr);
        std::cerr <<
            "\n*** WARNING ***\n"
            "\n";
    }

    LIBC_CALL_RV( pid, ::fork, () );

    if (pid == 0) {
        // Daemon process

        LIBC_CALL( ::dup2, (coutpipe[1],1) );
        LIBC_CALL( ::dup2, (cerrpipe[1],2) );
        LIBC_CALL( ::close, (coutpipe[0]) );
        LIBC_CALL( ::close, (coutpipe[1]) );
        LIBC_CALL( ::close, (cerrpipe[0]) );
        LIBC_CALL( ::close, (cerrpipe[1]) );
        LIBC_CALL( ::setsid, () );
        LIBC_CALL( ::sigprocmask, (SIG_SETMASK, &oldsig, 0) );

        senf::scheduler::restart();
        return;
    }

    // Ouch ... ensure, the daemon watcher does not remove the pidfile ...
    pidfile_ = "";

    LIBC_CALL( ::close, (coutpipe[1]) );
    LIBC_CALL( ::close, (cerrpipe[1]) );

    senf::scheduler::restart();

    detail::DaemonWatcher watcher (pid, coutpipe[0], cerrpipe[0], stdout_, stderr_);
    watcher.run();

    ::_exit(0);
}

prefix_ bool senf::Daemon::pidfileCreate()
{
    // Create temporary file pidfile_.hostname.pid and hard-link it to pidfile_ If the hardlink
    // fails, the pidfile exists. If the link count of the temporary file is not 2 after this, there
    // was some race condition, probably over NFS.

    std::string tempname;
    boost::format linkErrorFormat(" Could not link \"%1%\" to \"%2%\".");

    {
        char hostname[HOST_NAME_MAX+1];
        LIBC_CALL( ::gethostname, (hostname, HOST_NAME_MAX+1) );
        hostname[HOST_NAME_MAX] = 0;
        std::stringstream tempname_s;
        tempname_s << pidfile_ << "." << hostname << "." << ::getpid();
        tempname = tempname_s.str();
    }

    while (1) {
        {
            std::ofstream pidf (tempname.c_str());
            if (! pidf)
                SENF_THROW_SYSTEM_EXCEPTION(" Could not open pidfile \"" + tempname + "\" for output.");
            pidf << ::getpid() << std::endl;
            if (! pidf)
                SENF_THROW_SYSTEM_EXCEPTION(" Could not write to pidfile \"" + tempname + "\".");
        }

        if (::link(tempname.c_str(), pidfile_.c_str()) < 0) {
            if (errno != EEXIST)
                SENF_THROW_SYSTEM_EXCEPTION((linkErrorFormat % pidfile_ % tempname).str());
        }
        else {
            struct ::stat s;
            LIBC_CALL( ::stat, (tempname.c_str(), &s) );
            LIBC_CALL( ::unlink, (tempname.c_str()) );
            return s.st_nlink == 2;
        }

        // pidfile exists. Check, whether the pid in the pidfile still exists.
        {
            int old_pid (-1);
            std::ifstream pidf (pidfile_.c_str());
            if ( ! (pidf >> old_pid)
                 || old_pid < 0
                 || ::kill(old_pid, 0) >= 0
                 || errno == EPERM ) {
                LIBC_CALL( ::unlink, (tempname.c_str()) );
                return false;
            }
        }

        // If we reach this point, the pid file exists but the process mentioned within the
        // pid file does *not* exists. We assume, the pid file to be stale.

        // I hope, the following procedure is without race condition: We remove our generated
        // temporary pid file and recreate it as hard-link to the old pid file. Now we check, that
        // the hard-link count of this file is 2. If it is not, we terminate, since someone else
        // must have already created his hardlink. We then truncate the file and write our pid.

        LIBC_CALL( ::unlink, (tempname.c_str() ));
        if (::link(pidfile_.c_str(), tempname.c_str()) < 0) {
            if (errno != ENOENT)
                SENF_THROW_SYSTEM_EXCEPTION( (linkErrorFormat % tempname % pidfile_).str());
            // Hmm ... the pidfile mysteriously disappeared ... try again.
            continue;
        }

        {
            struct ::stat s;
            LIBC_CALL( ::stat, (tempname.c_str(), &s) );
            if (s.st_nlink != 2) {
                LIBC_CALL( ::unlink, (tempname.c_str()) );
                return false;
            }
        }

        {
            std::ofstream pidf (tempname.c_str());
            pidf << ::getpid() << std::endl;
        }

        LIBC_CALL( ::unlink, (tempname.c_str()) );
        break;
    }
    return true;
}


#ifdef SENF_DEBUG

namespace {
    void fatalSignalsHandler(int sig, ::siginfo_t * info, void * arg)
    {
        // ::ucontext_t * ucontext = static_cast<ucontext_t*>(arg);
        std::cerr << "\n" << "Signal " << senf::signalName(sig) << '(' << sig << ')'
                  << " received\n";

        if (sig == SIGSEGV)
            std::cerr << "Invalid memory access at " << info->si_addr << "\n";
#ifdef SENF_BACKTRACE
        static void * entries[SENF_DEBUG_BACKTRACE_NUMCALLERS];
        int nEntries( ::backtrace(entries, SENF_DEBUG_BACKTRACE_NUMCALLERS) );

        // Hack the callers address into the backtrace
        // entries[1] = reinterpret_cast<void *>(ucontext->uc_mcontext.gregs[REG_EIP]);

        std::cerr << "Backtrace:\n";
        senf::formatBacktrace(std::cerr, entries, nEntries);
        std::cerr << "-- \n";
#endif //SENF_BACKTRACE
        if (sig != SIGUSR2) {
            ::signal(sig, SIG_DFL);
            ::kill(::getpid(), sig);
        }
    }

}

#endif // SENF_DEBUG

namespace {
    void sighupHandler(int sig)
    {
        senf::Daemon::instance().logReopen();
    }
}

prefix_ void senf::Daemon::installSighandlers()
{
    struct ::sigaction sa;

    ::sigemptyset(&sa.sa_mask);
    sa.sa_handler = &sighupHandler;
    sa.sa_flags = SA_RESTART;

    ::sigaction(SIGHUP,   &sa, NULL);

    sa.sa_handler = SIG_IGN;
    ::sigaction(SIGPIPE, &sa, NULL);

#ifdef SENF_DEBUG
    sa.sa_sigaction = &fatalSignalsHandler;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;

    ::sigaction(SIGILL,    &sa, NULL);
    ::sigaction(SIGTRAP,   &sa, NULL);
    ::sigaction(SIGABRT,   &sa, NULL);
    ::sigaction(SIGFPE,    &sa, NULL);
    ::sigaction(SIGBUS,    &sa, NULL);
    ::sigaction(SIGSEGV,   &sa, NULL);
#ifdef SIGSTKFLT //SIGSTKFLT is used for stack faults on coprocessors. That condition doesn't exist on MIPS
    ::sigaction(SIGSTKFLT, &sa, NULL);
#endif
    ::sigaction(SIGSYS,    &sa, NULL);
    ::sigaction(SIGUSR2,   &sa, NULL);
#endif
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::detail::DaemonWatcher

prefix_ senf::detail::DaemonWatcher::DaemonWatcher(int pid, int coutpipe, int cerrpipe,
                                                   int stdout, int stderr)
    : childPid_(pid), coutpipe_(coutpipe), cerrpipe_(cerrpipe), stdout_(stdout),
      stderr_(stderr), sigChld_(false),
      cldSignal_ (SIGCHLD, senf::membind(&DaemonWatcher::sigChld, this)),
      timer_ ("senf::detail::DaemonWatcher::childOk", senf::membind(&DaemonWatcher::childOk, this)),
      coutForwarder_(coutpipe_, boost::bind(&DaemonWatcher::pipeClosed, this, 1)),
      cerrForwarder_(cerrpipe_, boost::bind(&DaemonWatcher::pipeClosed, this, 2))
{
    coutForwarder_.addTarget(1);
    if (stdout_ >= 0)
        coutForwarder_.addTarget(stdout_);
    cerrForwarder_.addTarget(2);
    if (stderr_ >= 0)
        cerrForwarder_.addTarget(stderr_);
}

prefix_ void senf::detail::DaemonWatcher::run()
{
    scheduler::process();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// private members

prefix_ void senf::detail::DaemonWatcher::pipeClosed(int id)
{
    switch (id) {
    case 1 : coutpipe_ = -1; break;
    case 2 : cerrpipe_ = -1; break;
    }

    if (coutpipe_ == -1 && cerrpipe_ == -1) {
        if (sigChld_)
            childDied(); // does not return
        if (::kill(childPid_, SIGUSR1) < 0 && errno != ESRCH)
            SENF_THROW_SYSTEM_EXCEPTION("::kill()");
        timer_.timeout(scheduler::eventTime() + ClockService::seconds(1));
    }
}

prefix_ void senf::detail::DaemonWatcher::sigChld(siginfo_t const &)
{
    sigChld_ = true;
    if (coutpipe_ == -1 && cerrpipe_ == -1)
        childDied(); // does not return
}

prefix_ void senf::detail::DaemonWatcher::childDied()
{
    int status (0);
    if (::waitpid(childPid_,&status,0) < 0) SENF_THROW_SYSTEM_EXCEPTION("::waitpid()");
    if (WIFSIGNALED(status)) {
        ::signal(WTERMSIG(status),SIG_DFL);
        ::kill(::getpid(), WTERMSIG(status));
        // should not be reached
        ::_exit(126);
    }
    if (WEXITSTATUS(status) == 0)
        ::_exit(127);
    ::_exit(WEXITSTATUS(status));
}

prefix_ void senf::detail::DaemonWatcher::childOk()
{
    scheduler::terminate();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::detail::DaemonWatcher::Forwarder

prefix_ senf::detail::DaemonWatcher::Forwarder::Target::Target(Forwarder & fwd, int fd_)
    : fd (fd_), offset (0),
      writeevent ("senf::detail::DaemonWatcher::Forwarder::Target::writeevent",
                  boost::bind(&Forwarder::writeData, &fwd, _1, this),
                  fd, scheduler::FdEvent::EV_WRITE, false)
{}

prefix_ senf::detail::DaemonWatcher::Forwarder::Forwarder(int src, Callback cb)
    : src_(src), cb_(cb),
      readevent_("senf::detail::DaemonWatcher::Forwarder::readevent", senf::membind(&Forwarder::readData, this),
                 src_, scheduler::FdEvent::EV_READ)
{}

prefix_ senf::detail::DaemonWatcher::Forwarder::~Forwarder()
{
    targets_.clear_and_dispose(DestroyDelete());
}

prefix_ void senf::detail::DaemonWatcher::Forwarder::addTarget(int fd)
{
    targets_.push_back(*(new Target(*this, fd)));
}

prefix_ void senf::detail::DaemonWatcher::Forwarder::readData(int event)
{
    char buf[1024];
    int n (0);

    while (1) {
        n = ::read(src_,buf,1024);
        if (n<0) {
            if (errno != EINTR)
                SENF_THROW_SYSTEM_EXCEPTION("::read()");
        }
        else
            break;
    }

    if (n == 0) {
        if (buffer_.empty())
            cb_();
        src_ = -1;
        readevent_.disable();
        return;
    }

    if (targets_.empty())
        return;

    for (Targets::iterator i (targets_.begin()); i != targets_.end(); ++i)
        if (i->offset >= buffer_.size())
            i->writeevent.enable();

    buffer_.insert(buffer_.end(), buf, buf+n);
}

prefix_ void senf::detail::DaemonWatcher::Forwarder::writeData(int event, Target * target)
{
    if (event != scheduler::FdEvent::EV_WRITE) {
        // Broken pipe while writing data ? Not much, we can do here, we just drop the data
        targets_.erase_and_dispose(Targets::s_iterator_to(*target),DestroyDelete());
        if (targets_.empty() && src_ == -1)
            cb_();
        return;
    }

    char buf[1024];
    int n (buffer_.size() - target->offset > 1024 ? 1024 : buffer_.size() - target->offset);
    std::copy(buffer_.begin() + target->offset, buffer_.begin() + target->offset + n, buf);

    int w (::write(target->fd, buf, n));
    if (w < 0) {
        if (errno != EINTR) SENF_THROW_SYSTEM_EXCEPTION("::write()");
        return;
    }
    target->offset += w;

    n = std::min_element(
        targets_.begin(), targets_.end(),
        boost::bind(&Target::offset, _1) < boost::bind(&Target::offset, _2))->offset;

    buffer_.erase(buffer_.begin(), buffer_.begin()+n);

    for (Targets::iterator i (targets_.begin()); i != targets_.end(); ++i)
        i->offset -= n;

    if (target->offset >= buffer_.size())
        target->writeevent.disable();
    if (src_ == -1 && (buffer_.empty() || targets_.empty()))
        cb_();
}

#undef LIBC_CALL
#undef LIBC_CALL_RV

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "Daemon.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
