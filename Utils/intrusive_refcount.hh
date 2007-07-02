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

/** \file
    \brief intrusive_refcount public header */

#ifndef HH_intrusive_refcount_
#define HH_intrusive_refcount_ 1

// Custom includes
#include <boost/utility.hpp>

//#include "intrusive_refcount.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Reference count mixin for intrusive_ptr

        This class provides a simple internally managed refcount and supplies the <a
        href="http://www.boost.org/libs/smart_ptr/intrusive_ptr.html">boost::intrusive_ptr</a>
        required interface. To make a class compatible with \c boost::intrusive_ptr, just derive
        publicly from intrusive_refcount.

        Two additional benefits of using intrusive_refcount are
        \li The object can access it's own refcount
        \li It is valid and safe to convert a plain object pointer to an intrusive_ptr at any time
            (not only after new)
     */
    class intrusive_refcount
        : public boost::noncopyable
    {
    public:
        typedef unsigned refcount_t;    ///< reference count type

        virtual ~intrusive_refcount();

        refcount_t refcount();          ///< current refcount
        bool is_shared();               ///< return \c true if refcount() > 1

    protected:
        intrusive_refcount();

    private:
        void add_ref();
        bool release();

        refcount_t refcount_;

        friend void senf::intrusive_ptr_add_ref(intrusive_refcount* p);
        friend void senf::intrusive_ptr_release(intrusive_refcount* p);
    };

    void intrusive_ptr_add_ref(intrusive_refcount* p);
    void intrusive_ptr_release(intrusive_refcount* p);
}

///////////////////////////////hh.e////////////////////////////////////////
#include "intrusive_refcount.cci"
//#include "intrusive_refcount.ct"
//#include "intrusive_refcount.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
