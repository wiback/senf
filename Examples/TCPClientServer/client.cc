// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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


// Custom includes
#include <iostream>
#include <sstream>
#include "Socket/TCPSocketHandle.hh"
#include "Socket/INetAddressing.hh"
#include "Packets/EthernetPacket.hh"


int main(int argc, char const * argv[])
{
    try {
        for (int i=0; i<=1000; i++) {
            senf::TCPv4ClientSocketHandle sock;
            sock.connect(senf::INet4Address("127.0.0.1", 4243));
            sock.protocol().linger(true);
            
            std::stringstream s;
            s << i++;
            sock.write(s.str());

            sock.close();
            
            std::cout << i << std::endl;
        }
    }
    catch (std::exception const & ex) {
        std::cerr << senf::prettyName(typeid(ex)) << ": " << ex.what() << "\n";
    }
    
    return 0;
}
