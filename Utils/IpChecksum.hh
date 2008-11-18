// $Id$
//
// Copyright (C) 2007
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

/** \file
    \brief IpChecksum public header */

#ifndef HH_SENF_Utils_IpChecksum_
#define HH_SENF_Utils_IpChecksum_ 1

// Custom includes
#include <boost/cstdint.hpp>

//#include "IpChecksum.mpp"
///////////////////////////////hh.p////////////////////////////////////////

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
        ///////////////////////////////////////////////////////////////////////////
        // Types

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        IpChecksum();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

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

///////////////////////////////hh.e////////////////////////////////////////
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
