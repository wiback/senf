//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/**file
   brief EnumParser public header */

#ifndef HH_SENF_Packets_EnumParser_
#define HH_SENF_Packets_EnumParser_ 1

// Custom includes
#include "PacketParser.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    template <typename EnumType, typename UnderlyingTypeParser>
    class EnumParser
        : public PacketParserBase
    {
    public:
        EnumParser(data_iterator i, state_type s);

        typedef EnumType value_type;
        static size_type const fixed_bytes = UnderlyingTypeParser::fixed_bytes;

        void value(EnumType v);
        EnumType value() const;

    private:
        UnderlyingTypeParser underlyingValue() const;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "EnumParser.cci"
//#include "EnumParser.ct"
#include "EnumParser.cti"
#endif
