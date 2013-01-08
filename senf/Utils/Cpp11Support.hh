// $Id$
//
// Copyright (C) 2013
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
//   Thorsten Horstmann <tho@berlios.de>

#ifndef HH_SENF_Utils_Cpp11Support_
#define HH_SENF_Utils_Cpp11Support_ 1

// Custom includes
#include <boost/config.hpp>
#include <senf/autoconf.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __clang__
#  if __has_feature(cxx_decltype)
#    define SENF_DECLTYPE decltype
#  else
#    define SENF_DECLTYPE typeof
#  endif
#else
#  ifdef BOOST_HAS_DECLTYPE
#    define SENF_DECLTYPE decltype
#  else
#    define SENF_DECLTYPE typeof
#  endif
#endif

#ifdef HAVE_STD__UNIQUE_PTR_VOID_
#  define unique_or_auto_ptr std::unique_ptr
#  define move_or_noop(t)    std::move(t)
#else
#  define unique_or_auto_ptr std::auto_ptr
#  define move_or_noop(t)    t
#endif

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Cpp11Support.cci"
//#include "Cpp11Support.ct"
//#include "Cpp11Support.cti"
#endif
