// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief DVBDemuxSocketProtocol public header */

#ifndef HH_SENF_Socket_Protocols_DVB_DVBDemuxSocketProtocol_
#define HH_SENF_Socket_Protocols_DVB_DVBDemuxSocketProtocol_ 1

#include <linux/dvb/dmx.h>

// Custom includes
#include <senf/Socket/SocketProtocol.hh>

//#include "DVBSocketProtocol.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_facets_group
    /// @{

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
        ///@{

        bool eof() const;

        ///@}
    };

    /// @}
}

///////////////////////////////hh.e////////////////////////////////////////
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
