// $Id: AddressingPolicy.hh 296 2007-07-10 20:39:34Z g0dil $
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
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
    \brief NetdeviceController public header
 */

#ifndef HH_NetdeviceController_
#define HH_NetdeviceController_ 1

// Custom includes
#include <string>
#include "Protocols/Raw/MACAddress.hh"

//#include "NetdeviceController.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief NetdeviceController
     
        \todo document me
     */
    class NetdeviceController
    {
    public:
        NetdeviceController(std::string const interface_name);
        
        virtual ~NetdeviceController();
        
        MACAddress hardwareAddress();
    
    private:
        int sockfd_;
        std::string interfacename_;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "NetdeviceController.cci"
//#include "NetdeviceController.ct"
//#include "NetdeviceController.cti"
//#include "NetdeviceController.mpp"
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
