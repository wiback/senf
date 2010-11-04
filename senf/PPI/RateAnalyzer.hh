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
    \brief RateAnalyzer public header */

#ifndef HH_SENF_PPI_RateAnalyzer_
#define HH_SENF_PPI_RateAnalyzer_ 1

// Custom includes
#include <boost/function.hpp>
#include <boost/signal.hpp>
#include "MonitorModule.hh"
#include "IntervalTimer.hh"

//#include "RateAnalyzer.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {

    /** \brief Generate periodic packet statistics

        This module will periodically generate statistics concerning the traversing packets. The
        statistics are emitted as Boost.Signals signals:

        \li \c signals.packetsPerSecond: number of packets in the last interval scaled to 1 second
        \li \c signals.bytesPerSecond; number of bytes in the last interval scaled to 1 second
        \li \c signals.bytesPerPacket: minimal, average and maximal packet size in the last interval

        These signals are normally connected as needed to senf::Statistics instances.

        \code
        senf::RateAnalyzer analyzer;
        senf::Statistics packets;
        senf::Statistics packetSize;

        analyzer.signals.packetsPerSecond.connect(packets);
        analyzer.signals.bytesPerPacket.connect(packetSize);

        analyzer.startStatistics(senf::ClockService::milliseconds(100u));
        \endcode

        Statistics output is only generated after a call to startStatistics()

        \ingroup routing_modules
     */
    class RateAnalyzer
        : public MonitorModule<>
    {
        SENF_PPI_MODULE(RateAnalyzer);
    public:
        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        RateAnalyzer();

        //\}
        //-////////////////////////////////////////////////////////////////////////
        // Statistics signals

        struct Statistics {
            boost::signal<void (float)> packetsPerSecond;
            boost::signal<void (float)> bytesPerSecond;
            boost::signal<void (unsigned,float,unsigned)> bytesPerPacket;
        } signals;

        void startStatistics(senf::ClockService::clock_type interval);
                                        ///< Start generating statistics at given interval

    private:
        void v_handlePacket(Packet const & p);
        void tick();

        senf::ppi::IntervalTimer timer_;
        unsigned packets_;
        unsigned bytes_;
        unsigned minSize_;
        unsigned maxSize_;
        double factor_;
    };

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "RateAnalyzer.cci"
//#include "RateAnalyzer.ct"
//#include "RateAnalyzer.cti"
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
