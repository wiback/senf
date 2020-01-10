//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
