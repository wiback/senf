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

    This collection of utilities provides help in managing daemon processes.

    \idea Add communication between parent and child process to daemonize() and add things like
        init_done(), failure() etc which allow the daemon process to tell the frontend of successful
        startup or failure. This probably means moving all the methods into a DaemonTools class (as
        statics or via a singleton). This would also allow for automatic pid file creation and
        removal (remove in global destructor).

    \idea Add a DaemonProcess baseclass with init() and main() abstract members which wraps the
        startup process. DaeminProcess::run() would fork, call init(), create a pid file and then
        call main(). Exceptions during init()'s execution would be passed to the parent
        process. This is based on the above API.

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
// End:
