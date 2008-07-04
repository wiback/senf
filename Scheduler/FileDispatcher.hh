// $Id$
//
// Copyright (C) 2008 
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
    \brief FileDispatcher public header */

#ifndef HH_FileDispatcher_
#define HH_FileDispatcher_ 1

// Custom includes
#include <map>
#include "FdManager.hh"
#include "FIFORunner.hh"
#include "FdDispatcher.hh"

//#include "FileDispatcher.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace scheduler {

    /** \brief
      */
    class FileDispatcher
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void (int)> Callback;

        enum Events { 
            EV_READ = FdManager::EV_READ, EV_WRITE = FdManager::EV_WRITE,
            EV_HUP = FdManager::EV_HUP, EV_ERR = FdManager::EV_ERR,
            EV_ALL = FdManager::EV_READ | FdManager::EV_WRITE
        };

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        FileDispatcher(FdManager & manager, FIFORunner & runner);
        ~FileDispatcher();

        ///@}
        ///////////////////////////////////////////////////////////////////////////
        
        void add(int fd, Callback const & cb, int events = EV_ALL);
        void remove(int fd, int events = EV_ALL);

        void prepareRun();

        void timeout(int t);
        int timeout() const;

    protected:

    private:
        struct FileEvent
            : public detail::FdTask<0, FileEvent>,
              public detail::FdTask<1, FileEvent>
        {
            typedef detail::FdTask<0, FileEvent> ReadTask;
            typedef detail::FdTask<1, FileEvent> WriteTask;

            int activeEvents() const;
            int events;
        };

        typedef std::map<int, FileEvent> FileMap;

        FileMap files_;
        FdManager & manager_;
        FIFORunner & runner_;
        int managerTimeout_;
    };


}}

///////////////////////////////hh.e////////////////////////////////////////
#include "FileDispatcher.cci"
//#include "FileDispatcher.ct"
//#include "FileDispatcher.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
