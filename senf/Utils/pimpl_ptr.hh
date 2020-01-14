//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
