// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief INet4Address public header */

#ifndef HH_INet4Address_
#define HH_INet4Address_ 1

// Custom includes
#include <iostream>
#include <string>
#include <boost/cstdint.hpp>
#include "Utils/SafeBool.hh"

//#include "INet4Address.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
    
    /** \brief
      */
    class INet4Address
        : public ComparableSafeBool<INet4Address>
    {
    public:
        typedef uint32_t address_type;

        static INet4Address const None;
        static INet4Address const Loopback;
        static INet4Address const Broadcast;

        INet4Address();
        explicit INet4Address(address_type value);

        static INet4Address from_string(std::string const & s);
        template <class InputIterator> 
        static INet4Address from_data(InputIterator i);
        static INet4Address from_inaddr(unsigned long v);

        bool local() const;
        bool loopback() const;
        bool multicast() const;
        bool broadcast() const;
        bool boolean_test() const;

        unsigned long raw() const;

        struct SyntaxException : public std::exception
        { virtual char const * what() const throw() { return "invalid INet4 address syntax"; } };

    private:
        // Hmm ... address_type or unsigned long? I'd use address_type but the man pages for in_addr
        // have sa_addr as unsigned long
        unsigned long addr_;
    };

    std::ostream & operator<<(std::ostream & os, INet4Address const & addr);

}

///////////////////////////////hh.e////////////////////////////////////////
#include "INet4Address.cci"
//#include "INet4Address.ct"
#include "INet4Address.cti"
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
