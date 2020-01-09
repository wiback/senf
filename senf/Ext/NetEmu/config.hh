// $Id: FdEvent.ih 2056 2014-03-14 13:59:24Z  $
//
// Copyright (C) 2011
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
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
//

#ifndef HH_SENF_Ext_NetEmu_config_
#define HH_SENF_Ext_NetEmu_config_ 1

// Custom includes

//-/////////////////////////////////////////////////////////////////////////////////////////////////

#include <senf/autoconf.hh>

#ifdef HAVE_NETLINK_VERSION_H
#  include <netlink/version.h>
#  ifndef LIBNL_VER_NUM
#    define LIBNL_VER_MAJ          2
#    define LIBNL_VER_MIN          0
#    define LIBNL_VER(maj,min)     ((maj) << 8 | (min))
#    define LIBNL_VER_NUM          LIBNL_VER(LIBNL_VER_MAJ, LIBNL_VER_MIN)
#  endif
#else
#  define LIBNL_STRING           "libnl 1.0"
#  define LIBNL_VERSION          "1.0"
#  define LIBNL_VER_MAJ          1
#  define LIBNL_VER_MIN          0
#  define LIBNL_VER(maj,min)     ((maj) << 8 | (min))
#  define LIBNL_VER_NUM          LIBNL_VER(LIBNL_VER_MAJ, LIBNL_VER_MIN)
#endif

#define SENF_EMU_MAXMTU 4096u

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u all_tests"
// End:
