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
    \brief Log public header */

#ifndef HH_SENF_Ext_NetEmu_Log_
#define HH_SENF_Ext_NetEmu_Log_ 1

// Custom includes
#include <senf/Packets.hh>
#include <senf/Utils/Logger/Log.hh>

//#include "Log.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    SENF_LOG_DEFINE_STREAM( Log, senf::log::MESSAGE, senf::log::MESSAGE, senf::log::NOTICE );
    SENF_LOG_DEFAULT_STREAM( Log );

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Log.cci"
//#include "Log.ct"
//#include "Log.cti"
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
