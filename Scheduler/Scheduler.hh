// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

#ifndef HH_Scheduler_
#define HH_Scheduler_ 1

// Custom includes
#include <map>
#include <boost/function.hpp>
#include <boost/utility.hpp>

//#include "scheduler.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    /** \brief Singleton class to manage the event loop

        This class manages a single select() type event loop. A
        customer of this class may register any number of file
        descriptiors with this class and pass callback functions to be
        called on input, output or error. This functions are specified
        using boost::function objects
      */
    class Scheduler
        : boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        enum EventId { EV_NONE=0, 
                       EV_READ=1, EV_WRITE=2, EV_HUP=4, EV_ERR=8, 
                       EV_ALL=15 };
        typedef boost::function<void (int fd, EventId event)> Callback;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // private default constructor
        // no copy constructor
        // no copy assignment
        // default destructor
        // no conversion constructors

        static Scheduler & instance();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void set(int fd, Callback const & cb, EventId eventMask = EV_ALL);
        void unset(int fd, EventId eventMask = EV_ALL);

    protected:

    private:
        Scheduler();

        struct EventSpec 
        {
            Callback cb_read;
            Callback cb_write;
            Callback cb_hup;
            Callback cb_err;

            int epollMask() const;
        };
        
        typedef std::map<int,EventSpec> FdTable;

        FdTable fdTable_;
        int epollFd_;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Scheduler.cci"
//#include "Scheduler.ct"
//#include "Scheduler.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
