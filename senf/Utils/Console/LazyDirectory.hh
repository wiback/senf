//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief LazyDirectory public header */

#ifndef HH_SENF_Utils_Console_LazyDirectory_
#define HH_SENF_Utils_Console_LazyDirectory_ 1

// Custom includes
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

//#include "LazyDirectory.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace console {

    template <class Owner=void> class ScopedDirectory;

    class LazyDirectory
        : boost::noncopyable
    {
    public:
        LazyDirectory();
        ~LazyDirectory();

        ScopedDirectory<> & operator()();

    private:
        boost::scoped_ptr< ScopedDirectory<> > p_;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "LazyDirectory.cci"
//#include "LazyDirectory.ct"
//#include "LazyDirectory.cti"
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
