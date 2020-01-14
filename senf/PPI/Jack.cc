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
    \brief Jack non-inline non-template implementation */

//#include "Jack.ih"

// Custom includes
#include "Setup.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::ppi::connector::GenericActiveInputJack::reset(GenericActiveInput & input)
{
    if (input_->connected()) {
        GenericPassiveOutput & peer (input_->peer());
        input_->disconnect();
        connect(peer, input);
    }
    input_ = & input;
}

prefix_ void senf::ppi::connector::GenericActiveOutputJack::reset(GenericActiveOutput & output)
{
    if (output_->connected()) {
        GenericPassiveInput & peer (output_->peer());
        output_->disconnect();
        connect(output, peer);
    }
    output_ = & output;
}

prefix_ void senf::ppi::connector::GenericPassiveInputJack::reset(GenericPassiveInput & input)
{
    if (input_->connected()) {
        GenericActiveOutput & peer (input_->peer());
        input_->disconnect();
        connect(peer, input);
    }
    input_ = & input;
}

prefix_ void senf::ppi::connector::GenericPassiveOutputJack::reset(GenericPassiveOutput & output)
{
    if (output_->connected()) {
        GenericActiveInput & peer (output_->peer());
        output_->disconnect();
        connect(output, peer);
    }
    output_ = & output;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
