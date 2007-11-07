// $Id$
//
// Copyright (C) 2006 Stefan Bund <g0dil@senf.berlios.de>
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
    \brief DaemonTools  non-inline non-template implementation */

#include "DaemonTools.hh"
//#include "DaemonTools.ih"

// Custom includes
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "Exception.hh"

//#include "DaemonTools.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::Daemon

prefix_ senf::Daemon::~Daemon()
{}

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
            throwErrno("open()");
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

prefix_ void senf::Daemon::pidFile(std::string f, bool unique)
{
    pidfile_ = f;
    unique_ = unique;
}

prefix_ void senf::Daemon::detach()
{}

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
    if (! pidfile_.empty())
        pidfileCreate();
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
    : argc_(0), argv_(0), daemonize_(true), stdout_(-1), stderr_(-1), pidfile_(""), unique_(true),
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
    
}

prefix_ void senf::Daemon::pidfileCreate()
{}

///////////////////////////////////////////////////////////////////////////

prefix_ void senf::daemonize()
{
    int pid = fork();
    if (pid < 0)
        throw senf::SystemException("fork",errno);
    if (pid > 0)
        ::_exit(0);
    if (::setsid() < 0)
        throw senf::SystemException("setsid",errno);
}

prefix_ void senf::redirect_stdio(std::string const & path)
{
    int fd = ::open(path.c_str(),O_RDWR);
    if (fd < 0) throw senf::SystemException("open",errno);
    if (dup2(fd,0) < 0) throw senf::SystemException("dup2",errno);
    if (dup2(fd,1) < 0) throw senf::SystemException("dup2",errno);
    if (dup2(fd,2) < 0) throw senf::SystemException("dup2",errno);
    if (::close(fd) < 0) throw senf::SystemException("close",errno);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "DaemonTools.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
