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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief MMapSocketProtocol public header */

#ifndef HH_SENF_Socket_Protocols_Raw_MMapSocketProtocol_
#define HH_SENF_Socket_Protocols_Raw_MMapSocketProtocol_ 1

// Custom includes
#include <senf/Socket/SocketProtocol.hh>
#include <senf/Socket/QueueReadWritePolicy.hh>

//#include "MMapSocketProtocol.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    class MMapSocketProtocol
        : public virtual SocketProtocol,
          private FileHandleAccess
    {
    public:
        void close();
        void terminate() const;

    protected:
        void init_mmap(unsigned frameSize, unsigned rxqlen, unsigned txqlen, unsigned reserve) const;
        void close_mmap() const;
        void terminate_mmap() const;

        mutable detail::QueueInfo qi_;
    };

    class MMapReadableSocketProtocol
        : public virtual SocketProtocol,
          private FileHandleAccess
    {
    public:
        unsigned available() const;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "MMapSocketProtocol.cci"
//#include "MMapSocketProtocol.ct"
//#include "MMapSocketProtocol.cti"
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
