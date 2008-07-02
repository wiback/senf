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
    \brief FIFORunner public header */

#ifndef HH_FIFORunner_
#define HH_FIFORunner_ 1

// Custom includes
#include <boost/utility.hpp>
#include "../boost/intrusive/ilist.hpp"
#include "../boost/intrusive/ilist_hook.hpp"

//#include "FIFORunner.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf { 
namespace scheduler {

    /** \brief
      */
    class FIFORunner
        : boost::noncopyable
    {
    public:
        struct TaskInfo;

    private:
        struct TaskListTag;
        typedef boost::intrusive::ilist_base_hook<TaskListTag> TaskListBase;
        typedef TaskListBase::value_traits<TaskInfo> TaskListType;
        typedef boost::intrusive::ilist<TaskListType, false> TaskList;

    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        struct TaskInfo 
            : public TaskListBase
        {
            TaskInfo();
            virtual ~TaskInfo();

            bool runnable;
            virtual void run() = 0;
        };

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        FIFORunner();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void enqueue(TaskInfo * task);
        void dequeue(TaskInfo * task);
        
        void run();

    protected:

    private:
        TaskList tasks_;
        TaskList::iterator next_;
    };


}}

///////////////////////////////hh.e////////////////////////////////////////
#include "FIFORunner.cci"
//#include "FIFORunner.ct"
//#include "FIFORunner.cti"
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
