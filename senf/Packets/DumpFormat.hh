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
    \brief DumpFormat public header */

#ifndef HH_SENF_Packets_DumpFormat_
#define HH_SENF_Packets_DumpFormat_ 1

#ifndef HH_SENF_Packets_Packets_
#error "Don't include 'DumpFormat.hh' directly, include 'Packets.hh'"
#endif

// Custom includes

//#include "DumpFormat.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    std::string fieldName(std::string const & s);

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_DumpFormat_i_)
#define HH_SENF_Packets_DumpFormat_i_
//#include "DumpFormat.cci"
//#include "DumpFormat.ct"
//#include "DumpFormat.cti"
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
