// $Id$
//
// Copyright (C) 2007 
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
    \brief Joins public header */

#ifndef HH_Joins_
#define HH_Joins_ 1

// Custom includes
#include <boost/ptr_container/ptr_vector.hpp>
#include "predecl.hh"
#include "Connectors.hh"
#include "Module.hh"

//#include "Joins.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

#ifndef DOXYGEN

    template <class Source>
    connector::GenericPassiveInput & connect(Source & source, module::PassiveJoin & target);
    
    template <class Source>
    connector::GenericActiveInput & connect(Source & source, module::PriorityJoin & target);

#endif

namespace module {

    /** \brief Join multiple packet streams with passive inputs

        The PassiveJoin will combine any number of packet streams. You may connect any number of
        ActiveOutput<>'s  to the PassiveJoin instance. The combined stream is then provided on the
        ActiveOutput<> \a output.

        Since PassiveJoin allows any number of incoming packet streams, the input connectors are
        dynamically managed. A special senf::ppi::connect() overload is used to dynamically create
        the needed input connectors. This hides this extra functionality from the user.
        \code
        senf::ppi::module::PassiveJoin join;

        ppi::connect(module1,join);             // Connect first module to join's input
        ppi::connect(module2.some_output,join); // Connect another module to join's input
        ppi::connect(join,module3);             // Forward combined stream to module3
        \endcode

        \ingroup routing_modules
     */
    class PassiveJoin
        : public Module
    {
        SENF_PPI_MODULE(PassiveJoin);
    public:
        connector::ActiveOutput<> output;

        PassiveJoin();

    private:
        connector::PassiveInput<> & newInput();

#ifndef DOXYGEN
        // I didn't get template friend functions to work ...
    public:
#endif
        template <class Source>
        connector::GenericPassiveInput & connect(Source & source);

    private:
        void request(connector::GenericPassiveInput & input);
        void onThrottle();
        void onUnthrottle();

        typedef boost::ptr_vector<connector::PassiveInput<> > Inputs;
        Inputs inputs_;
    };

    /** \brief Join multiple packet streams with active inputs

        The PriorityJoin will combine any number of packet streams. You may connect any number of
        PassiveInput<>'s  to the PassiveJoin instance. The combined stream is then provided on the
        PassiveOutput<> \a output.

        When a packet request is received on Priorityjoin's \a output, The request will be serviced
        from the first unthrottled input. The order, in which connectors are connected to the
        PriorityJoin's input is important: The earlier connected peer has the higher priority and
        will be serviced first.

        Since PriorityJoin allows any number of incoming packet streams, the input connectors are
        dynamically managed. A special senf::ppi::connect() overload is used to dynamically create
        the needed input connectors. This hides this extra functionality from the user.
        \code
        senf::ppi::module::PriorityJoin join;

        ppi::connect(module1,join);             // Connect first module to join's input
        ppi::connect(module2.some_output,join); // Connect another module to join's input
        ppi::connect(join,module3);             // Forward combined stream to module3
        \endcode
        Here, \a module1 has higher priority than \a module2 which will only be queried if \a
        module1 is throttled.
        
        \ingroup routing_modules
     */
    class PriorityJoin
        : public Module
    {
        SENF_PPI_MODULE(PriorityJoin);
    public:
        connector::PassiveOutput<> output;

        PriorityJoin();

    private:
        connector::ActiveInput<> & newInput();

#ifndef DOXYGEN
    public:
#endif
        template <class Source>
        connector::GenericActiveInput & connect(Source & source);

    private:
        void request();
        void onThrottle();
        void onUnthrottle();

        typedef boost::ptr_vector<connector::ActiveInput<> > Inputs;
        Inputs inputs_;
    };

}}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Joins.cci"
//#include "Joins.ct"
#include "Joins.cti"
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
