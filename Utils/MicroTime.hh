// $Id$
//
// Copyright (C) 2006 Stefan Bund <g0dil@senf.berlios.de>
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
    \brief MicroTime public header */

/** \defgroup time Microseconds Time

    MicroTime provides extremely rudimentary support for a time data
    type precise to 10<sup>-6</sup> seconds. Up to now, the only
    supported members are the datatype (which just uses a 64 bit
    integer) and the now() function to get the current UTC time in
    microsecods since the Epoch.

    \idea This thing only exists as a quick hack. We can probably make
        use of Boost.Time or some such thing so it probably does not
        make sense to extend this further. We should however check the
        performance of Boost.Time since this is used heavily in the
        Scheduler.
 */

#ifndef HH_MicroTime_
#define HH_MicroTime_ 1

// Custom includes
#include <boost/cstdint.hpp>

//#include "MicroTime.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup time
    /// @{

    typedef boost::uint64_t MicroTime; ///< Microsecond accurent time datatype

    MicroTime now(); ///< Get current UTC time with microsecond accuracy

    /// @}

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "MicroTime.cci"
//#include "MicroTime.ct"
//#include "MicroTime.cti"
//#include "MicroTime.mpp"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
