//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief RateAnalyzer public header */

#ifndef HH_SENF_PPI_RateAnalyzer_
#define HH_SENF_PPI_RateAnalyzer_ 1

// Custom includes
#include <boost/function.hpp>
#include <boost/signals2.hpp>
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
            boost::signals2::signal<void (float)> packetsPerSecond;
            boost::signals2::signal<void (float)> bytesPerSecond;
            boost::signals2::signal<void (unsigned,float,unsigned)> bytesPerPacket;
        } signals;

        void startStatistics(senf::ClockService::clock_type interval);
                                        ///< Start generating statistics at given interval

    private:
        void v_handlePacket(Packet const & p) override;
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
