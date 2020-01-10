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
