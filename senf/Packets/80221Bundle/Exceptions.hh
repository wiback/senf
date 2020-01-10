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


#ifndef HH_SENF_Packets_80221Bundle_Exceptions_
#define HH_SENF_Packets_80221Bundle_Exceptions_ 1

// Custom includes

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    struct MIHLengthException : public senf::Exception
    {
        MIHLengthException(std::string const & description)
          : senf::Exception("MIHLengthException: ") { append(description); }
    };

    struct InvalidMIHPacketException : public senf::Exception
    {
        InvalidMIHPacketException(std::string const & description)
            : senf::Exception("Invalid MIH message: ") { append(description); }
    };
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Exceptions.cci"
//#include "Exceptions.ct"
//#include "Exceptions.cti"
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
