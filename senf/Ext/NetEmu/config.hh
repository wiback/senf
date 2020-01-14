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
