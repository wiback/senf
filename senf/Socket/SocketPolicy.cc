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
    \brief SocketPolicy non-inline non-template implementation */

#include "SocketPolicy.hh"
#include "SocketPolicy.ih"

// Custom includes

//#include "SocketPolicy.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::AddressingPolicyBase::~AddressingPolicyBase()
{}

prefix_ senf::FramingPolicyBase::~FramingPolicyBase()
{}

prefix_ senf::CommunicationPolicyBase::~CommunicationPolicyBase()
{}

prefix_ senf::ReadPolicyBase::~ReadPolicyBase()
{}

prefix_ senf::WritePolicyBase::~WritePolicyBase()
{}

prefix_ senf::SocketPolicyBase::~SocketPolicyBase()
{}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "SocketPolicy.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
