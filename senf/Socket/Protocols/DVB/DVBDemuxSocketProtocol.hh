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
