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
    \brief PacketDropper public header */

#ifndef HH_SENF_Ext_NetEmu_Modules_PacketDropper_
#define HH_SENF_Ext_NetEmu_Modules_PacketDropper_ 1

// Custom includes
#include <senf/PPI/Module.hh>
#include <senf/PPI/Connectors.hh>
#include <boost/random.hpp>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    class PacketDropper
        : public ppi::module::Module
    {
        SENF_PPI_MODULE(PacketDropper);

    public:
        ppi::connector::PassiveInput<> input;
        ppi::connector::ActiveOutput<> output;

        PacketDropper(double rate);
        double lossRate() const;
        void lossRate(double lossRate);

    private:
        typedef boost::mt19937 RandomEngine; // Mersenne twister
        typedef boost::bernoulli_distribution<> RandomDistribution;
        typedef boost::variate_generator<RandomEngine, RandomDistribution> RandomGenerator;

        RandomEngine randomEngine_;
        boost::scoped_ptr<RandomDistribution> randomDistribution_;
        boost::scoped_ptr<RandomGenerator> randomGenerator_;
        double lossRate_;

        void onRequest_forwardAll();
        void onRequest_discardAll();
        void onRequest_random();
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "PacketDropper.cci"
//#include "PacketDropper.ct"
//#include "PacketDropper.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
