// $Id:$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//       Stefan Bund <g0dil@berlios.de>

/** \file
    \brief Config public header */

#ifndef HH_Carmen_Config_
#define HH_Carmen_Config_ 1

// Custom includes
#include <boost/range.hpp>
#include <boost/utility.hpp>
#include <string>
#include <senf/Scheduler/ClockService.hh>
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
#include <senf/Socket/Protocols/INet/INetAddressing.hh>
#include <senf/Utils/Console.hh>

//#include "Config.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace wiback {

    /** \brief Carmen emulator configuration

        This class manages all needed configuration information.
    */
    class Config
        : boost::noncopyable
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Structors and default members

        Config();

        //-////////////////////////////////////////////////////////////////////////

        void probeInterval(senf::ClockService::clock_type interval);
        senf::ClockService::clock_type probeInterval() const;

        void quiet(bool q);
        bool quiet() const;

        void silent(bool s);
        bool silent() const;

        void maxPacketDelay(senf::ClockService::clock_type delay);
        senf::ClockService::clock_type maxPacketDelay() const;

        void maxSchedulingDelay(senf::ClockService::clock_type delay);
        senf::ClockService::clock_type maxSchedulingDelay() const;

        void burnCycles(unsigned cycles);
        unsigned burnCycles() const;

        void numPackets(unsigned n);
        unsigned numPackets() const;

        void consolePort(unsigned n);
        unsigned consolePort() const;

        void nodeCoordinates(unsigned id, double latitude, double longitude);
        std::pair<double,double> const & nodeCoordinates(unsigned id) const;

        struct InvalidNodeIdException : public senf::Exception
        { InvalidNodeIdException() : senf::Exception("invalid node id") {} };

        senf::console::ScopedDirectory<> dir;

    private:
        void showHelp(std::ostream & os, std::string const & command);

        senf::ClockService::clock_type probeInterval_;
        bool quiet_;
        bool silent_;
        senf::ClockService::clock_type maxPacketDelay_;
        senf::ClockService::clock_type maxSchedulingDelay_;
        unsigned burnCycles_;
        unsigned numPackets_;
        unsigned consolePort_;

        typedef std::map<unsigned, std::pair<double,double> > Coordinates;
        Coordinates coordinates_;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Config.cci"
//#include "Config.ct"
//#include "Config.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -U"
// End:
