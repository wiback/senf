// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer NETwork research (NET)
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
#include "../Utils/Exception.hh"
#include "../Utils/membind.hh"

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

prefix_ void senf::Daemon::consoleLog(std::string path, StdStream which)
{
    int fd (-1);
    if (! path.empty()) {
        int fd (::open(path.c_str(), O_WRONLY | O_APPEND));
        if (fd < 0)
            throwErrno("::open()");
    }
    switch (which) {
    case StdOut:
        stdout_ = fd;
        break;
    case StdErr:
        stderr_ = fd;
        break;
    case Both:
        stdout_ = fd;
        stderr_ = fd;
        break;
    }
}

prefix_ void senf::Daemon::pidFile(std::string f)
{
    pidfile_ = f;
}

prefix_ void senf::Daemon::detach()
{
    if (daemonize_) {
        LIBC_CALL_RV( nul, ::open, ("/dev/null", O_WRONLY) );
        LIBC_CALL( ::dup2, (nul, 1) );
        LIBC_CALL( ::dup2, (nul, 2) );
        LIBC_CALL( ::close, (nul) );
    }
}

prefix_ int senf::Daemon::start(int argc, char const ** argv)
{
    argc_ = argc;
    argv_ = argv;

#   ifdef NDEBUG
    try {
#   endif

        configure();

        if (daemonize_)
            fork();
        if (! pidfile_.empty() && ! pidfileCreate()) {
            std::cerr << "\n*** PID file '" << pidfile_ << "' creation failed. Daemon running ?" 
                      << std::endl;
            return 1;
        }

        main();

#   ifdef NDEBUG
    }
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
{}

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

    detail::DaemonWatcher watcher (pid, coutpipe[0], cerrpipe[0]);
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

prefix_ senf::detail::DaemonWatcher::DaemonWatcher(int pid, int coutpipe, int cerrpipe)
    : childPid_(pid), coutpipe_(coutpipe), cerrpipe_(cerrpipe), sigChld_(false),
      coutForwarder_(coutpipe_, 1, boost::bind(&DaemonWatcher::pipeClosed, this, 1)),
      cerrForwarder_(cerrpipe_, 2, boost::bind(&DaemonWatcher::pipeClosed, this, 2))
{}

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

prefix_ senf::detail::DaemonWatcher::Forwarder::Forwarder(int src, int dst, Callback cb)
    : src_(src), dst_(dst), cb_(cb)
{
    Scheduler::instance().add(src_, senf::membind(&Forwarder::readData, this),
                              Scheduler::EV_READ);
}

prefix_ senf::detail::DaemonWatcher::Forwarder::~Forwarder()
{
    if (src_ != -1)
        Scheduler::instance().remove(src_);
    if (dst_ != -1 && ! buffer_.empty())
        Scheduler::instance().remove(dst_);
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
    if (dst_ == -1)
        // There was an error writing data -> drop it
        return;
    if (buffer_.empty())
        Scheduler::instance().add(dst_, senf::membind(&Forwarder::writeData, this),
                                  Scheduler::EV_WRITE);
    buffer_.insert(buffer_.end(), buf, buf+n);
}

prefix_ void senf::detail::DaemonWatcher::Forwarder::writeData(Scheduler::EventId event)
{    
    if (event != Scheduler::EV_WRITE) {
        // Broken pipe while writing data ? Not much, we can do here, we just drop the data
        Scheduler::instance().remove(dst_);
        dst_ = -1;
        if (src_ == -1) cb_();
        return;
    }
    char buf[1024];
    int n (buffer_.size() > 1024 ? 1024 : buffer_.size());
    std::copy(buffer_.begin(), buffer_.begin() + n, buf);
    int w (::write(dst_, buf, n));
    if (w < 0) {
        if (errno != EINTR) throwErrno("::write()");
        return;
    }
    buffer_.erase(buffer_.begin(), buffer_.begin()+w);
    if (buffer_.empty()) {
        Scheduler::instance().remove(dst_);
        if (src_ == -1)
            cb_();
    }
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
