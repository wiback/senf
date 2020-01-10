//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
