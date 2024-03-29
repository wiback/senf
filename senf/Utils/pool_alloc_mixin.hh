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
    \brief pool_alloc_mixin public header */

#ifndef HH_SENF_Utils_pool_alloc_mixin_
#define HH_SENF_Utils_pool_alloc_mixin_ 1

// Custom includes
#include <boost/pool/singleton_pool.hpp>

//#include "pool_alloc_mixin.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    struct pool_alloc_mixin_tag;

    /** \brief Mixin to assign pool allocator to a class

        This mixin will overload a classes <tt>operator new</tt> and <tt>operator delete</tt> so as
        to make the class use the <a
        href="http://www.boost.org/doc/libs/release/libs/pool/doc/index.html">Boost.Pool</a> memory allocator by
        default. Using this allocator does however introduce a few restrictions:

        \li The operator is defined for a fixed size. Therefore if you derive from the class <b>you
            must not change it's size</b>.
        \li If you change the size of the class in a derived class you have to derive from
            pool_alloc_mixin again.

        Usage:
        \code
          class SomeClass
              : public senf::pool_alloc_mixin<SomeClass>
          {
              // ...
          };
        \endcode

        \note pool_alloc_mixin uses the <a
            href="http://www.boost.org/doc/libs/release/libs/pool/doc/index.html">Boost.Pool</a> <i>singleton
            pool</i> interface with the tag <tt>pool_alloc_mixin_tag</tt>. This class is accessible
            via the <tt>pool</tt> member. Using this member, it is simple to call relevant pool
            functions, e.g. <tt>SomeClass::pool<>::release_memory()</tt>.
     */
    template <class Self>
    class pool_alloc_mixin
    {
    public:
        /** \brief Templated typedef for the pool used

            Since the instantiation of the typedef must be delayed until Self is completely defined,
            the simple typedef is replaced with a nested struct.
         */
        template <class T=void>
        struct pool
            : public boost::singleton_pool< pool_alloc_mixin_tag, sizeof(Self) >
        {
            typedef boost::singleton_pool< pool_alloc_mixin_tag, sizeof(Self) > type;
        };

        static void * operator new (size_t size);
                                        ///< Operator new utilizing pool allocation
        static void operator delete (void *p, size_t size);
                                        ///< Operator delete utilizing pool allocation

#ifdef SENF_DEBUG
        static unsigned long allocCounter();
    private:
        static unsigned long allocCounter(long delta);
#endif
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "pool_alloc_mixin.cci"
//#include "pool_alloc_mixin.ct"
#include "pool_alloc_mixin.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
