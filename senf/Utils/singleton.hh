// $Id$
//
// Copyright (C) 2007
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
    \brief singleton public header */

#ifndef HH_SENF_Utils_singleton_
#define HH_SENF_Utils_singleton_ 1

// Custom includes
#include <boost/utility.hpp>

//#include "singleton.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Mark a class as singleton and provide singleton accessor

        This mixin class will mark a class as singleton and provide an accessor function to access
        this singleton instance. The following preconditions must be met for this class to work as
        advertised:
        \li There must be only a single thread executing before main() starts. (This should always
            be the case)
        \li There must be only a single thread executing after main() ends. (This is always
            important, otherwise global object destruction might fail)
        \li The singleton class must have a non throwing default constructor and destructor

        If these conditions are met, this mixin will ensure that the singleton is constructed \e
        before main even starts executing. If static construction code calls the instance() member,
        it is ensured, that the instance is constructed no later than the first call to instance().

        Usage example:
        \code
          class SomeClass
              : public senf::singleton<SomeClass>
          {
              // Must have default constructor
              SomeClass();

              // Give singleton access to the constructor
              friend class senf::singleton<SomeClass>;

        public:
              // By default 'instance()' is protected. If you want, you may make it public:
              using senf::singleton<SomeClass>::instance;

              // ...
          };

          int main(int argc, char ** argv)
          {
              // At this point, the instance has already been constructed

              SomeClass::instance().doSomething();
          }
        \endcode

        \warning The singleton class should \e not have any static data members since it cannot be
            guaranteed, that these members will be constructed before the singleton instance.

        \implementation This implementation is directly taken from
            <tt>boost/pool/detail/singleton.hpp</tt>. See that file for a description of the
            technique. The only difference is, that I prefer to advertise this class as a mixin
            (though it may be used the same way as the original too).
     */
    template <class Self>
    class singleton
        : boost::noncopyable
    {
    protected:
        singleton();
        ~singleton();

        static Self & instance();       ///< Return singleton instance
        static bool alive();            ///< Return \c true, if instance ok, \c false otherwise

    private:
        /** \brief Internal
            \internal
         */
        struct force_creation
        {
            force_creation();
            void nop() const;
        };

        static force_creation creator_;
        static bool alive_;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "singleton.cci"
//#include "singleton.ct"
#include "singleton.cti"
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
