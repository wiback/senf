// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

#ifndef HH_Exception_
#define HH_Exception_ 1

// Custom includes
#include <exception>
#include <string>

//#include "Exception.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {


    struct SystemException : public std::exception
    {
        explicit SystemException(int err_) : where(0), err(err_) { init(); }
        SystemException(char const * where_, int err_) : where(where_), err(err_) { init(); }

        virtual char const * what() const throw();

        char const * where;
        int err;

        virtual ~SystemException() throw() {}
    private:
        void init();
	std::string buffer_;
    };
    
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Exception.cci"
//#include "Exception.ct"
//#include "Exception.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
