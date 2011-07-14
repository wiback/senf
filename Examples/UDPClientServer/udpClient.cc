// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>


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
            sock.writeto(senf::INet4SocketAddress(argv[1]),s.str());
            std::cout << i << std::endl;
        }
    }
    catch (std::exception const & ex)
    {
        std::cerr << senf::prettyName(typeid(ex)) << ": " << ex.what() << "\n";
    }

    return 0;
}
