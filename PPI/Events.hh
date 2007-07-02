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
    \brief Events public header */

#ifndef HH_Events_
#define HH_Events_ 1

// Custom includes

//#include "Events.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief Generic event interface baseclass

        The EventDescriptor baseclass provides an interface to manuplate events in a generic
        way. This allows to register events or to temporarily disable event processing.
     */ 
    class EventDescriptor
    {
    public:
        bool enabled(); ///< Check, whether the event is currently enabled
        void enabled(bool); ///< Enable or disable the event

    protected:
        typedef unspecified CallbackType; ///< Fixed type of the (internal) event handler.

        void register(CallbackType handler); ///< Register the event
        void unregister(); ///< Unregister the event

    private:
        virtual void v_register(CallbackType handler) = 0; ///< Called to register the event
        virtual void v_unregister() = 0; ///< Called to unregister the event
        virtual void v_enable() = 0;    ///< Called to enable the event delivery
        virtual void v_disable() = 0;   ///< Called to disable the event delilvery
        virtual void v_process() = 0;   ///< Called whenever the event is signaled
                                        /**< This virtual method is called \e after every call to
                                             the event handler to provide a hook for further
                                             processing (example: calculate the next time, an
                                             interval timer expires) */

        bool enabled_;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Events.cci"
//#include "Events.ct"
//#include "Events.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
