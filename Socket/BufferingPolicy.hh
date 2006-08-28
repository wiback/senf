// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

#ifndef HH_BufferingPolicy_
#define HH_BufferingPolicy_ 1

// Custom includes
#include "FileHandle.hh"
#include "SocketPolicy.hh"

//#include "BufferingPolicy.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    // TODO: Should this be dependent on Read / WritePolicy ?
    struct SocketBufferingPolicy : public BufferingPolicyBase
    {
        static unsigned rcvbuf(FileHandle handle);
        static void rcvbuf(FileHandle handle, unsigned size);

        static unsigned sndbuf(FileHandle handle);
        static void sndbuf(FileHandle handle, unsigned size);
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "BufferingPolicy.cci"
//#include "BufferingPolicy.ct"
//#include "BufferingPolicy.cti"
//#include "BufferingPolicy.mpp"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
