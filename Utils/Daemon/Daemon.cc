// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief Daemon non-inline non-template implementation */

#include "Daemon.hh"
#include "Daemon.ih"

// Custom includes
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sstream>
#include <algorithm>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "../Exception.hh"
#include "../membind.hh"

//#include "Daemon.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

#define LIBC_CALL(fn, args) if (fn args < 0) throwErrno(#fn "()")
#define LIBC_CALL_RV(var, fn, args) int var (fn args); if (var < 0) throwErrno(#fn "()")

///////////////////////////////////////////////////////////////////////////
// senf::Daemon

prefix_ senf::Daemon::~Daemon()
{
    if (! pidfile_.empty())
        LIBC_CALL( ::unlink, (pidfile_.c_str()) );
}

prefix_ void senf::Daemon::daemonize(bool v)
{
    daemonize_ = v;
}

prefix_ bool senf::Daemon::daemon()
{
    return daemonize_;
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
            throwErrno("::open()");
        stdout_ = fd;
    }
    if (stderrLog_ == stdoutLog_)
        stderr_ = fd;
    else if (! stderrLog_.empty()) {
        fd = ::open(stdoutLog_.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0666);
        if (fd < 0)
            throwErrno("::open()");
        stderr_ = fd;
    }
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

        LIBC_CALL_RV( nul, ::open, ("/dev/null", O_WRONLY) );
        LIBC_CALL( ::dup2, (stdout_ == -1 ? nul : stdout_, 1) );
        LIBC_CALL( ::dup2, (stderr_ == -1 ? nul : stderr_, 2) );
        LIBC_CALL( ::close, (nul) );

        signaled = false;
        while (! signaled) {
            ::sigsuspend(&waitsig);
            if (errno != EINTR)
                throwErrno("::sigsuspend()");
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
        if (! pidfile_.empty() && ! pidfileCreate()) {
            std::cerr << "\n*** PID file '" << pidfile_ << "' creation failed. Daemon running ?" 
                      << std::endl;
            return 1;
        }

        main();
    }
    catch (DaemonExitException & e) {
        return e.code;
    }

#ifdef SENF_NO_DEBUG

    catch (std::exception & e) {
        std::cerr << "\n*** Fatal exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "\n*** Fatal exception: (unknown)" << std::endl;
        return 1;
    }

#   endif

    return 0;
}

////////////////////////////////////////
// protected members

prefix_ senf::Daemon::Daemon()
    : argc_(0), argv_(0), daemonize_(true), stdout_(-1), stderr_(-1), pidfile_(""),
      detached_(false)
{}

////////////////////////////////////////
// private members

prefix_ void senf::Daemon::configure()
{
    for (int i (1); i<argc_; ++i) {
        if (argv_[i] == std::string("--no-daemon"))
            daemonize(false);
        else if (boost::starts_with(argv_[i], std::string("--console-log="))) {
            std::string arg (std::string(argv_[i]), 14u);
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
        else if (boost::starts_with(argv_[i], std::string("--pid-file="))) 
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
        return;
    }

    // Ouch ... ensure, the daemon watcher does not remove the pidfile ...
    pidfile_ = "";
    
    LIBC_CALL( ::close, (coutpipe[1]) );
    LIBC_CALL( ::close, (cerrpipe[1]) );

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
            pidf << ::getpid() << std::endl;
        }

        if (::link(tempname.c_str(), pidfile_.c_str()) < 0) {
            if (errno != EEXIST) 
                throwErrno("::link()");
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
                 || errno == EPERM )
                return false;
        }

        // If we reach this point, the pid file exists but the process mentioned within the
        // pid file does *not* exists. We assume, the pid file to be stale.

        // I hope, the following procedure is without race condition: We remove our generated
        // temporary pid file and recreate it as hard-link to the old pid file. Now we check, that
        // the hard-link count of this file is 2. If it is not, we terminate, since someone else
        // must have already created his hardlink. We then truncate the file and write our pid.

        LIBC_CALL( ::unlink, (tempname.c_str() ));
        if (::link(pidfile_.c_str(), tempname.c_str()) < 0) {
            if (errno != ENOENT) throwErrno("::link()");
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

///////////////////////////////////////////////////////////////////////////
// senf::detail::DaemonWatcher

prefix_ senf::detail::DaemonWatcher::DaemonWatcher(int pid, int coutpipe, int cerrpipe,
                                                   int stdout, int stderr)
    : childPid_(pid), coutpipe_(coutpipe), cerrpipe_(cerrpipe), stdout_(stdout),
      stderr_(stderr), sigChld_(false),
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
    Scheduler::instance().registerSignal(SIGCHLD, senf::membind(&DaemonWatcher::sigChld, this));
    Scheduler::instance().process();
}

////////////////////////////////////////
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
        if (::kill(childPid_, SIGUSR1) < 0)
            if (errno != ESRCH) throwErrno("::kill()");
        Scheduler::instance().timeout(
            Scheduler::instance().eventTime() + ClockService::seconds(1),
            senf::membind(&DaemonWatcher::childOk, this));
    }
}

prefix_ void senf::detail::DaemonWatcher::sigChld()
{
    sigChld_ = true;
    if (coutpipe_ == -1 && cerrpipe_ == -1)
        childDied(); // does not return
}

prefix_ void senf::detail::DaemonWatcher::childDied()
{
    int status (0);
    if (::waitpid(childPid_,&status,0) < 0) throwErrno("::waitpid()");
    if (WIFSIGNALED(status)) {
        ::signal(WTERMSIG(status),SIG_DFL);
        ::kill(::getpid(), WTERMSIG(status));
        // should not be reached
        ::_exit(1);
    }
    if (WEXITSTATUS(status) == 0)
        ::_exit(1);
    ::_exit(WEXITSTATUS(status));
}

prefix_ void senf::detail::DaemonWatcher::childOk()
{
    Scheduler::instance().terminate();
}

///////////////////////////////////////////////////////////////////////////
// senf::detail::DaemonWatcher::Forwarder

prefix_ senf::detail::DaemonWatcher::Forwarder::Forwarder(int src, Callback cb)
    : src_(src), cb_(cb)
{
    Scheduler::instance().add(src_, senf::membind(&Forwarder::readData, this),
                              Scheduler::EV_READ);
}

prefix_ senf::detail::DaemonWatcher::Forwarder::~Forwarder()
{
    if (src_ != -1)
        Scheduler::instance().remove(src_);
    
    for (Targets::iterator i (targets_.begin()); i != targets_.end(); ++i)
        if (i->offset >= buffer_.size())
            Scheduler::instance().remove(i->fd);
}

prefix_ void senf::detail::DaemonWatcher::Forwarder::addTarget(int fd)
{
    Target target = { fd, 0 };
    targets_.push_back(target);
}

prefix_ void senf::detail::DaemonWatcher::Forwarder::readData(Scheduler::EventId event)
{
    char buf[1024];
    int n (0);

    while (1) {
        n = ::read(src_,buf,1024);
        if (n<0) {
            if (errno != EINTR) throwErrno("::read()");
        } else 
            break;
    }

    if (n == 0) {
        // Hangup
        Scheduler::instance().remove(src_);
        if (buffer_.empty())
            cb_(); 
        src_ = -1;
        return;
    }

    if (targets_.empty())
        return;

    for (Targets::iterator i (targets_.begin()); i != targets_.end(); ++i)
        if (i->offset >= buffer_.size())
            Scheduler::instance().add( i->fd, 
                                       boost::bind(&Forwarder::writeData, this, _1, i),
                                       Scheduler::EV_WRITE );

    buffer_.insert(buffer_.end(), buf, buf+n);
}

prefix_ void senf::detail::DaemonWatcher::Forwarder::writeData(Scheduler::EventId event,
                                                               Targets::iterator target)
{    
    if (event != Scheduler::EV_WRITE) {
        // Broken pipe while writing data ? Not much, we can do here, we just drop the data
        Scheduler::instance().remove(target->fd);
        targets_.erase(target);
        if (targets_.empty() && src_ == -1)
            cb_();
        return;
    }

    char buf[1024];
    int n (buffer_.size() - target->offset > 1024 ? 1024 : buffer_.size() - target->offset);
    std::copy(buffer_.begin() + target->offset, buffer_.begin() + target->offset + n, buf);

    int w (::write(target->fd, buf, n));
    if (w < 0) {
        if (errno != EINTR) throwErrno("::write()");
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
        Scheduler::instance().remove(target->fd);
    if (src_ == -1 && (buffer_.empty() || targets_.empty()))
        cb_();
}

#undef LIBC_CALL
#undef LIBC_CALL_RV

///////////////////////////////cc.e////////////////////////////////////////
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
