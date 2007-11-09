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
    \brief DaemonTools public header */

/** \defgroup process Process Management

    Future features:

    \li Daemon manager watching the real daemon. Auto-restart, when the daemon fails

    \li Provide access to the config console indirectly via the daemon manager. This allows to
        connect to the daemon manager console even if the app is not running
    
    \li For this to be efficient, the daemon manager must be it's own executable (e.g. senf-launch)

    \li auto-detect whether called from senf-launch or not

    \li when daemon is running, the console is transparently forwarded to the daemon. The daemon
        however can still access the daemon manager as a subgroup
    
    \li No idea, whether this is sensible: Make the daemon manager completely self-contained (not
        dependent on any external OS support) by providing our own log-file rotation support.

    This collection of utilities provides help in managing daemon processes.

    \idea A closeall()/closemost() function which is useful when starting child processes. We'll use
        getrlimit to now the biggest filehandle and close all of em. closemost() takes a number of
        file handles as arg and will keep those open.

    \idea We might want to add other oft used utitlities: chroot(), setreuid(), pipes() / IPC ...
 */

#ifndef HH_DaemonTools_
#define HH_DaemonTools_ 1

// Custom includes
#include <string>

//#include "DaemonTools.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup process
    /// @{

    void daemonize();                   ///< Make the current process a daemon process
                                        /**< daemonize() will fork, detach from the controlling
                                             terminal and start a new process group. */
    void redirect_stdio(std::string const & path = "/dev/null"); ///< Redirect STDIN, STDOUT and STDERR
                                        /**< All standard file-descriptors will be redirected to the
                                             given path defaulting to <tt>/dev/null</tt>
                                             \param[in] path path to redirect to */

    /// @}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "DaemonTools.cci"
//#include "DaemonTools.ct"
//#include "DaemonTools.cti"
//#include "DaemonTools.mpp"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
