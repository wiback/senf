// $Id$
//
// Copyright (C) 2007
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

/** \file
    \brief IpChecksum public header */

#ifndef HH_SENF_Utils_IpChecksum_
#define HH_SENF_Utils_IpChecksum_ 1

// Custom includes
#include <boost/cstdint.hpp>

//#include "IpChecksum.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Calculate TCP/IP 16bit checksum value

        This class allows calculation of the 16bit checksum value as defined in the TCP/IP v4
        RFC's. This checksum is \e not a CRC.

        To calculate the checksum, you create an instance of IpChecksum. You then repeatedly call
        the feed() members to pass data in. After all data has been processed, the sum() member will
        return the calculated checksum.

        The mathematical properties of the checksum allow to skip any \e even number of zero bytes
        without changing the checksum value.
      */
    class IpChecksum
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        IpChecksum();

        //\}
        //-////////////////////////////////////////////////////////////////////////

        void feed(boost::uint8_t byte); ///< Feed single byte

        template <class InputIterator>
        void feed(InputIterator b, InputIterator e);
                                        ///< Feed range of bytes

        boost::uint16_t sum() const;    ///< Calculate checksum

    protected:

    private:
        boost::uint32_t sum_;
        bool odd_;
    };


}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "IpChecksum.cci"
//#include "IpChecksum.ct"
#include "IpChecksum.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
