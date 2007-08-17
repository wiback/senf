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
    \brief Setup public header */

#ifndef HH_Setup_
#define HH_Setup_ 1

// Custom includes
#include "predecl.hh"

//#include "Setup.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {
    
    void connect(connector::ActiveOutput & source, connector::PassiveInput & target);
    void connect(connector::PassiveOutput & source, connector::ActiveInput & target);
    
    template <class M, class C>
    void connect(M & source, C & target,
                 typename boost::enable_if< boost::is_base_of<module::Module, M> >::type * = 0,
                 typename boost::enable_if< boost::is_base_of<connector::Connector, C> >::type * = 0);

    template <class C, class M>
    void connect(C & source, M & target,
                 typename boost::enable_if< boost::is_base_of<connector::Connector, C> >::type * = 0,
                 typename boost::enable_if< boost::is_base_of<module::Module, M> >::type * = 0);

    template <class M1, class M2>
    void connect(M1 & source, M2 & target,
                 typename boost::enable_if< boost::is_base_of<module::Module, M1> >::type * = 0,
                 typename boost::enable_if< boost::is_base_of<module::Module, M2> >::type * = 0);
    
    void run();
    void init();

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Setup.cci"
//#include "Setup.ct"
#include "Setup.cti"
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
