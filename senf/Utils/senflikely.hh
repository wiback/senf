// $Id: senflikely.hh 2056 2014-03-14 13:59:24Z tho $
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fit.fraunhofer.de/license.html
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
//   Stefan Bund <senf@g0dil.de>

/** \file
    \brief senflikely public header */

#ifndef HH_SENF_Utils_senflikely_
#define HH_SENF_Utils_senflikely_ 1

// Custom includes

//#include "senflikely.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#define SENF_LIKELY(x)      __builtin_expect(!!(x), 1)
#define SENF_UNLIKELY(x)    __builtin_expect(!!(x), 0)


//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "senflikely.cci"
//#include "senflikely.ct"
//#include "senflikely.cti"
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
