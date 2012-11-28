// $Id$
//
// Copyright (C) 2010
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
    \brief pimpl_ptr public header */

#ifndef HH_SENF_Utils_pimpl_ptr_
#define HH_SENF_Utils_pimpl_ptr_ 1

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
