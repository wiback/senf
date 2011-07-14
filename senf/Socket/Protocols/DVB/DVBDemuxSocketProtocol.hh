// $Id$
//
// Copyright (C) 2007
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief DVBDemuxSocketProtocol public header */

#ifndef HH_SENF_Socket_Protocols_DVB_DVBDemuxSocketProtocol_
#define HH_SENF_Socket_Protocols_DVB_DVBDemuxSocketProtocol_ 1

#include <linux/dvb/dmx.h>

// Custom includes
#include <senf/Socket/SocketProtocol.hh>

//#include "DVBSocketProtocol.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_facets_group
    //\{

    /** \todo Document me!
     */
    class DVBDemuxSocketProtocol
        : public virtual SocketProtocol
    {
    public:
        void setBufferSize(unsigned long size) const;
                                ///< set the size of the circular buffer used for filtered data.
                                /**< The default size is two maximum sized sections, i.e. if this
                                     function is not called a buffer size of 2 * 4096 bytes will
                                     be used.
                                     \param[in] size Size of circular buffer. */
        void startFiltering() const;
        void stopFiltering() const;

        ///\name Abstract Interface Implementation
        //\{

        bool eof() const;

        //\}
    };

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "DVBDemuxSocketProtocol.cci"
//#include "DVBDemuxSocketProtocol.ct"
//#include "DVBDemuxSocketProtocol.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
