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
    \brief Target public header */

#ifndef HH_Target_
#define HH_Target_ 1

// Custom includes
#include <set>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/utility.hpp>
#include "../singleton.hh"
#include "../mpl.hh"
#include "StreamRegistry.hh"
#include "AreaRegistry.hh"

//#include "Target.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace log {

    class TargetRegistry;

    /** \brief Logging target base class

        All enabled log messages are eventually routed to one or more logging targets. It is the
        responsibility of the logging target to write the log messages somewhere: onto the console,
        to a file, to mail them to the administrator or whatever. To this end, the logging target is
        passed the log message and a complete set of logging parameters (\e stream, \e area and \e
        level).

        \fixme optionally Integrate with Scheduler / ClockService to reduce number of gettimeofday()
            calls.
      */
    class Target : private boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        enum action_t { ACCEPT, REJECT };

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        Target();
        virtual ~Target();

        ///@}

        template <class Stream>
        void route(action_t action=ACCEPT);

        template <class Stream, class Arg>
        void route(action_t action=ACCEPT);

        template <class Stream, class Area, class Level>
        void route(action_t action=ACCEPT);

        void route(std::string const & stream, action_t action=ACCEPT);
        void route(std::string const & stream, std::string const & area, action_t action=ACCEPT);
        void route(std::string const & stream, unsigned level, action_t action=ACCEPT);
        void route(std::string const & stream, std::string const & area, unsigned level, 
                   action_t action=ACCEPT);

        struct InvalidStreamException : public std::exception
        { virtual char const * what() const throw() 
                { return "senf::log::Target::InvalidStreamException"; } };

        struct InvalidAreaException : public std::exception
        { virtual char const * what() const throw() 
                { return "senf::log::Target::InvalidAreaException"; } };
        
    protected:

        std::string timestamp();

    private:

        void route(detail::StreamBase const * stream, detail::AreaBase const * area, 
                   unsigned level, action_t action);
        void unroute(detail::StreamBase const * stream, detail::AreaBase const * area, 
                     unsigned level, action_t action);

        template <class Area>
        void route(detail::StreamBase const * stream, detail::AreaBase const *, action_t action);

        template <class Level>
        void route(detail::StreamBase const * stream, detail::LevelBase const *, action_t action);
        
        void updateRoutingCache(detail::StreamBase const * stream, detail::AreaBase const * area);

        void write(boost::posix_time::ptime timestamp, detail::StreamBase const & stream,
                   detail::AreaBase const & area, unsigned level, std::string const & message);

#   ifdef DOXYGEN
    protected:
#   endif

        virtual void v_write(boost::posix_time::ptime, std::string const & stream, 
                             std::string const & area, unsigned level, 
                             std::string const & message) = 0;

#   ifdef DOXYGEN
    private:
#   endif

        struct RoutingEntry 
        {
            RoutingEntry(detail::StreamBase const * stream_, detail::AreaBase const * area_, 
                         unsigned level_, action_t action_);
            RoutingEntry();

            bool operator==(RoutingEntry const & other);

            detail::StreamBase const * stream;
            detail::AreaBase const * area;
            unsigned level;            action_t action;
        };

        typedef std::vector<RoutingEntry> RIB;

        RIB rib_;
        
        friend class detail::AreaBase;
    };

    /** \brief Target registry

        The TargetRegistry keeps a record of all existing targets. 
      */
    class TargetRegistry
        : public senf::singleton<TargetRegistry>
    {
    public:
        using senf::singleton<TargetRegistry>::instance;

        void write(detail::StreamBase const & stream, detail::AreaBase const & area,
                   unsigned level, std::string msg);

    private:
        void registerTarget(Target * target);
        void unregisterTarget(Target * target);

        typedef std::set<Target *> Targets;
        Targets targets_;
        
        friend class Target;
    };


    template <class Stream, class Area, class Level>
    void write(std::string msg);

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Target.cci"
//#include "Target.ct"
#include "Target.cti"
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
