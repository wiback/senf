// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief EventBinding public header */

#ifndef HH_EventBinding_
#define HH_EventBinding_ 1

// Custom includes
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "../predecl.hh"
#include "Callback.hh"

//#include "EventBinding.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {
namespace detail {

    class EventBindingBase
    {
    protected:
        EventBindingBase(EventManager & manager, module::Module & module, 
                         EventDescriptor & descriptor);

        void eventTime(boost::posix_time::ptime time);

    private:
        EventManager * manager_;
        module::Module * module_;
        EventDescriptor * descriptor_;
    };

    template <class EventType>
    class EventBinding
        : public EventBindingBase
    {
    public:
        typedef EventType Event;
        typedef typename detail::EventArgType<Event>::type EventArg;
        typedef typename detail::Callback<Event const &>::type Callback;

        EventBinding(EventManager & manager, module::Module & module, Callback callback,
                     EventDescriptor & descriptor);

        void callback(EventArg event, boost::posix_time::ptime time);
        void callback(EventArg event);

    private:
        Callback callback_;
    };

}}}

///////////////////////////////hh.e////////////////////////////////////////
#include "EventBinding.cci"
//#include "EventBinding.ct"
#include "EventBinding.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u ../test"
// End:
