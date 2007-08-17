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

    template <class Source>
    connector::PassiveInput & connect(Source & source, module::PassiveJoin & target);
    
    template <class Source>
    connector::ActiveInput & connect(Source & source, module::PriorityJoin & target);

namespace module {

    class PassiveJoin
        : public Module
    {
        SENF_PPI_MODULE(PassiveJoin);
    public:
        connector::ActiveOutput output;

        PassiveJoin();

        template <class Source>
        connector::PassiveInput & connect(Source & source);

    private:
        connector::PassiveInput & newInput();

        void request(connector::PassiveInput & input);
        void onThrottle();
        void onUnthrottle();

        typedef boost::ptr_vector<connector::PassiveInput> Inputs;
        Inputs inputs_;
    };

    class PriorityJoin
        : public Module
    {
        SENF_PPI_MODULE(PriorityJoin);
    public:
        connector::PassiveOutput output;

        PriorityJoin();

        template <class Source>
        connector::ActiveInput & connect(Source & source);

    private:
        connector::ActiveInput & newInput();

        void request();
        void onThrottle();
        void onUnthrottle();

        typedef boost::ptr_vector<connector::ActiveInput> Inputs;
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
