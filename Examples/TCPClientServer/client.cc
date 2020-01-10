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
    try {
        for (int i=0; i<=1000; i++) {
            senf::TCPv4ClientSocketHandle sock;
            sock.connect(senf::INet4SocketAddress("127.0.0.1:4243"));
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


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u"
// comment-column: 40
// End:
