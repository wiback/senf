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

#ifndef HH_docstub_
#define HH_docstub_ 1

// Custom includes

//#include "docstub.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace boost {

    struct noncopyable {};
    template <class Derived> struct totally_ordered {};
    template <class Derived, class Value, class Traversal, class Reference> struct iterator_facade {};
    template <class T> struct intrusive_ptr { T * ptr; };
    template <class T> struct shared_ptr { T * ptr; };

}

namespace std {

    struct exception {};
    template <class T> struct vector { T * elements; };
    template <class T> struct list { T * elements; };

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "docstub.cci"
//#include "docstub.ct"
//#include "docstub.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
