// $Id$
//
// Copyright (C) 2007 
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
    \brief UNAddress public header */

#ifndef HH_UNAddress_
#define HH_UNAddress_ 1

// Custom includes
#include <string>
#include <boost/operators.hpp>
#include <boost/filesystem/path.hpp>
#include "../../../Utils/SafeBool.hh"

//#include "UNAddress.mpp"
///////////////////////////////hh.p////////////////////////////////////////
namespace senf {
    /** \brief Unix domain address
        
        UNAddress represents a simple unix domain address which is given by a path to a socket. 
        It is modelled as a boost::filesystem::path.

        \ingroup addr_group
      */
    class UNAddress 
        : public boost::filesystem::path,
          public ComparableSafeBool<UNAddress>
    {   
    public: 
        UNAddress(); ///< Construct an empty address
        explicit UNAddress(boost::filesystem::path);///< Construct an address constant from given path
        static UNAddress fromString(std::string & s); ///< Convert string to address by interpreting the string as path
        static UNAddress fromPath(boost::filesystem::path & p);///< Convert path to address 
        UNAddress clone(); ///< Clone object 
        std::string pathString() const; ///< Return the path of the address as string

        /** \brief Base-class for UNAddress exceptions */
        struct AddressException : public std::exception {}; 

    private:
        boost::filesystem::path path;
    };

std::ostream & operator<<(std::ostream & os, UNAddress const & addr);
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "UNAddress.cci"
//#include "UNAddress.ct"
//#include "UNAddress.cti"
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
