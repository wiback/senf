// $Id$
//
// Copyright (C) 2010
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
    \brief pimpl_ptr public header */

#ifndef HH_SENF_senf_Utils_pimpl_ptr_
#define HH_SENF_senf_Utils_pimpl_ptr_ 1

// Custom includes
#include <algorithm>
#include <boost/ptr_container/clone_allocator.hpp>

//#include "pimpl_ptr.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf
{

    template<typename T, class CloneAllocator=boost::heap_clone_allocator>
    class pimpl_ptr
    {
        typedef void (*Deleter)(T* p);
        typedef T* (*Copier)(const T* p);

    public:
        explicit pimpl_ptr(T* pointee);
        pimpl_ptr(const pimpl_ptr & rhs);
        ~pimpl_ptr() throw();

        const T* get() const throw();
        T* get() throw();

        const T* operator->() const throw();
        T* operator->() throw();

        const T& operator*() const throw();
        T& operator*() throw();

        void swap(pimpl_ptr& with) throw();

        pimpl_ptr & operator=(const pimpl_ptr & rhs);

    private:
        static Copier doCopy_;
        static Deleter doDelete_;
        T* p;

        static void myDeleteFn(T* p);
        static T* myCopyFn(const T* p);
    };

}

namespace std
{
    template<class T, class CloneAllocator>
    void swap(senf::pimpl_ptr<T,CloneAllocator>& lhs, senf::pimpl_ptr<T,CloneAllocator>& rhs)
        throw();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "pimpl_ptr.cci"
//#include "pimpl_ptr.ct"
#include "pimpl_ptr.cti"
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
