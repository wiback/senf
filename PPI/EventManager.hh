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
    \brief EventManager public header */

#ifndef HH_EventManager_
#define HH_EventManager_ 1

// Custom includes
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include "predecl.hh"
#include "detail/Callback.hh"
#include "detail/EventBinding.hh"

//#include "EventManager.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief
      */
    class EventManager
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        template <class Descriptor>
#ifndef DOXYGEN
        struct Callback
#else
        // This is SO stupid but doxygen must have some scoping problems if the 
        // struct is called 'Callback' and will hang in an endless loop somewhere
        struct Callback_
#endif
            : public detail::Callback<typename Descriptor::EventArg>
        {};

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        static EventManager & instance();

        // default default constructor
        // default copy constructor
        // default copy assignment
        // default destructor

        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        template <class Descriptor>
        void registerEvent(module::Module & module,
                           typename Callback<Descriptor>::type callback,
                           Descriptor & descriptor);

        boost::posix_time::ptime eventTime();

    protected:

    private:
        void destroyModule(module::Module & module);

        typedef boost::ptr_vector<detail::EventBindingBase> EventRegistrations;
        EventRegistrations registrations_;

        void eventTime(boost::posix_time::ptime time);

        boost::posix_time::ptime eventTime_;

        friend class detail::EventBindingBase;
        friend class module::Module;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "EventManager.cci"
#include "EventManager.ct"
//#include "EventManager.cti"
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
