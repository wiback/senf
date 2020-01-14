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
    \brief HardwareDVBInterface unit tests */

#include "HardwareDVBInterface.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
using namespace senf;

SENF_AUTO_TEST_CASE(hardwareDVBSenderInterface)
{
    SENF_RETURN_NO_ROOT_PRIVILEGES("Cannot test senf::emu::HardwareDVBInterface as non-root user");

    emu::HardwareDVBSenderInterface("lo");
    emu::HardwareDVBReceiverInterface receiver;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
