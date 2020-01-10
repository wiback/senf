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
    \brief ActiveFeeder public header */

#ifndef HH_SENF_PPI_ActiveFeeder_
#define HH_SENF_PPI_ActiveFeeder_ 1

// Custom includes
#include "Module.hh"
#include "Connectors.hh"
#include "IdleEvent.hh"

//#include "ActiveFeeder.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {

    /** \brief Adapter to connect passive connectors

        ActiveFeeder is an adapter module which allows two connect to passive connectors. As long as
        none of the connectors is throttled, the ActiveFeeder will forward packets from it's input
        to it's output.

        \note For this Module to work correctly, it is very important for the connectors to be
            correctly throttled. Otherwise the system might well hang in an endless loop.

        \ingroup adapter_modules
     */
    class ActiveFeeder
        : public Module
    {
        SENF_PPI_MODULE(ActiveFeeder);
    public:
        connector::ActiveInput<> input;
        connector::ActiveOutput<> output;

        ActiveFeeder();

    private:
        void request();

        IdleEvent idle_;
    };

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "ActiveFeeder.cci"
//#include "ActiveFeeder.ct"
//#include "ActiveFeeder.cti"
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
