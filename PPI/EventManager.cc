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
    \brief EventManager non-inline non-template implementation */

#include "EventManager.hh"
//#include "EventManager.ih"

// Custom includes
#include <boost/lambda/lambda.hpp>

//#include "EventManager.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::ppi::EventManager

////////////////////////////////////////
// private members

prefix_ void senf::ppi::EventManager::destroyModule(module::Module & module)
{
    using boost::lambda::_1;

    // boost::ptr_vector::erase(f,l) asserts !empty() .. why ??
    if (!registrations_.empty())
        registrations_.erase(
            std::remove_if(registrations_.begin(), registrations_.end(),
                           ((&_1) ->* & detail::EventBindingBase::module_) == & module),
            registrations_.end());
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "EventManager.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
