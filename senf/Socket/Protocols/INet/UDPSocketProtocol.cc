// $Id$
//
// Copyright (C) 2006
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief UDPSocketProtocol non-inline non-template implementation
 */

#include "UDPSocketProtocol.hh"
//#include "UDPSocketProtocol.ih"

// Custom includes
#include <sys/ioctl.h>
#include <linux/sockios.h> // for SIOCINQ / SIOCOUTQ

//#include "UDPSocketProtocol.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ unsigned senf::UDPSocketProtocol::available()
    const
{
    int n;
    if (::ioctl(fd(),SIOCINQ,&n) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("could not call ::ioctl(SIOCINQ) in UDPSocketProtocol::available()");
    return n;
}

prefix_ bool senf::UDPSocketProtocol::eof()
    const
{
    return false;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "UDPSocketProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
