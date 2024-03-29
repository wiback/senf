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
    \brief intrusive_refcount public header */

#ifndef HH_SENF_Utils_intrusive_refcount_
#define HH_SENF_Utils_intrusive_refcount_ 1

// Custom includes
#include <boost/noncopyable.hpp>

//#include "intrusive_refcount.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    template <class Self> class intrusive_refcount_t;
    
    template <class Self>
    void intrusive_ptr_add_ref(intrusive_refcount_t<Self> const * p);
    template <class Self>
    void intrusive_ptr_release(intrusive_refcount_t<Self> const * p);
    

    /** \brief Reference count mixin interface baseclass

        This class is the baseclass of all intrusive refcount mixins. See \ref intrusive_refcount_t
        and \ref intrusive_refcount for more information.
     */
    class intrusive_refcount_base
        : public boost::noncopyable
    {
    public:
        typedef unsigned refcount_t;    ///< reference count type

        virtual ~intrusive_refcount_base();

        refcount_t refcount() const;    ///< current refcount
        bool is_shared() const;         ///< return \c true if refcount() > 1

    protected:
        intrusive_refcount_base();

        void add_ref();
        bool release();

    private:
        refcount_t refcount_;

        template <class S> void intrusive_ptr_add_ref();
        template <class S> void intrusive_ptr_release();

        template <class S>
        friend void senf::intrusive_ptr_add_ref(intrusive_refcount_t<S> const * p);
        template <class S>
        friend void senf::intrusive_ptr_release(intrusive_refcount_t<S> const * p);
    };

    /** \brief Customizable reference count mixin for intrusive_ptr

        This class provides a simple internally managed refcount and supplies the <a
        href="http://www.boost.org/doc/libs/release/libs/smart_ptr/intrusive_ptr.html">boost::intrusive_ptr</a>
        required interface. To make a class compatible with \c boost::intrusive_ptr, just derive
        publicly from intrusive_refcount_t.

        \code
        class SomeClass
            : public intrusive_refcount_t<SomeClass>
        {
            // ...

        private:
            typedef intrusive_refcount_t<SomeClass> intrusive_base;

            void add_ref()
            {
                // somehow call intrusive_base::add_ref()
            }

            bool release()
            {
                // Call intrusive_base::release() to decrement the
                // refcount. This call will return 'true' when the refcount reaches 0.

                // Return 'true', if the instance shall be deleted
            }

            // Needed to give intrusive_base access to 'add_ref' and 'release'
            friend class intrusive_base;
        };
        \endcode

        Two additional benefits of using intrusive_refcount are
        \li The object can access it's own refcount
        \li It is valid and safe to convert a plain object pointer to an intrusive_ptr at any time
            (not only after new)

        This class allows to customize the reference counting strategy (e.g. additional refcounting
        within another object or checking some additional condition before deleting the object when
        the refcount reaches 0).

        The interface of this class is defined in \ref intrusive_refcount_base (which you should not
        use directly).
     */
    template <class Self>
    class intrusive_refcount_t
        : public intrusive_refcount_base
    {
    protected:
        intrusive_refcount_t();
    };

    /** \brief Reference count mixin for intrusive_ptr

        This class provides a simple internally managed refcount and supplies the <a
        href="http://www.boost.org/doc/libs/release/libs/smart_ptr/intrusive_ptr.html">boost::intrusive_ptr</a>
        required interface. To make a class compatible with \c boost::intrusive_ptr, just derive
        publicly from intrusive_refcount.

        \code
        class SomeClass
            : public intrusive_refcount
        {
            // ...
        };
        \endcode

        Two additional benefits of using intrusive_refcount are
        \li The object can access it's own refcount
        \li It is valid and safe to convert a plain object pointer to an intrusive_ptr at any time
            (not only after new)

        This mixin class directly uses the default allocation strategy. If you want to customize the
        reference couting, use \ref intrusive_refcount_t.

        The interface of this class is defined in \ref intrusive_refcount_t and \ref
        intrusive_refcount_base (the latter of which you should not use directly).
     */
    class intrusive_refcount
        : public intrusive_refcount_t<intrusive_refcount>
    {
    protected:
        intrusive_refcount();
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "intrusive_refcount.cci"
//#include "intrusive_refcount.ct"
#include "intrusive_refcount.cti"
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
