// $Id$
//
// Copyright (C) 2008
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

#include <iostream>
#include <sstream>
#include <senf/Socket/Protocols/INet.hh>

int main(int argc, char const * argv[])
{
    try
    {
        for (int i=0; i<=10; i++)
        {
            senf::UDPv4ClientSocketHandle sock;
            std::stringstream s;
            s << i;
            sock.writeto(senf::INet4SocketAddress("127.0.0.1:4243"),s.str());
            std::cout << i << std::endl;
        }
    }
    catch (std::exception const & ex)
    {
        std::cerr << senf::prettyName(typeid(ex)) << ": " << ex.what() << "\n";
    }

    return 0;
}
