// $Id:$
//
// Copyright (C) 2009
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
//       Thorsten Horstmann <tho@berlios.de>

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
