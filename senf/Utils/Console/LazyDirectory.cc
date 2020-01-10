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
    \brief LazyDirectory non-inline non-template implementation */

#include "LazyDirectory.hh"
//#include "LazyDirectory.ih"

// Custom includes
#include "ScopedDirectory.hh"

//#include "LazyDirectory.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::console::LazyDirectory::LazyDirectory()
    : p_( new ScopedDirectory<>() )
{}

prefix_ senf::console::LazyDirectory::~LazyDirectory()
{}

prefix_ senf::console::ScopedDirectory<> & senf::console::LazyDirectory::operator()()
{
    return *p_;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "LazyDirectory.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
