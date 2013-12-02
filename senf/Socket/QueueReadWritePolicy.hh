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
    \brief QueueReadWritePolicy public header */

#ifndef HH_SENF_Socket_QueueReadWritePolicy_
#define HH_SENF_Socket_QueueReadWritePolicy_ 1

// Custom includes
#include <linux/if_packet.h>
#include <boost/optional.hpp>
#include "Protocols/Raw/LLAddressing.hh"
#include "SocketPolicy.hh"
#include "FileHandle.hh"

//#include "QueueReadWritePolicy.mpp"
#include "QueueReadWritePolicy.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    struct QueueReadPolicy;
    struct QueueWritePolicy;

    /** \brief Buffer used by QueueReadPolicy / QueueWritePolicy

        This class provides a convenient interface to the frame buffers as utilized by the queue
        based read and write policy. This class resembles an iterator range with additional access
        to information found in the buffer header.

        The SocketQueueBuffer is a fixed space container which may be resized within the space
        available. There are several views into that space:

        \li \c begin(), \c end(), \c size() and \c empty() provide access to the active frame data
        \li \c frameBegin(), \c frameEnd() and \c frameSize() provide access to the maximum space
            usable for packet data
     */
    class SocketQueueBuffer
    {
    public:
        typedef unsigned char * iterator;
        typedef unsigned char const * const_iterator;
        typedef std::size_t size_type;

        SocketQueueBuffer();

        iterator begin();
        iterator end();
        const_iterator begin() const;
        const_iterator end() const;
        size_type size() const;
        bool empty() const;

        iterator frameBegin();
        iterator frameEnd();
        const_iterator frameBegin() const;
        const_iterator frameEnd() const;
        size_type frameSize() const;

        unsigned dataOffset() const;

        senf::ClockService::clock_type timestamp() const;
                                        ///< return packet receive timestamp
        senf::LLSocketAddress address() const;
                                        ///< return incoming interface address
        boost::optional<unsigned> vlan() const;
                                        ///< return the preprocessed VLAN TCI if any

        void resize(size_type sz
#ifdef SENF_ENABLE_TPACKET_OFFSET
                    , int offset=-1
#endif
            );      ///< resize active packet data to given size

    private:
        SocketQueueBuffer(unsigned char * b, unsigned char * e, unsigned hdrlen);

        struct ::tpacket2_hdr & hdr();
        struct ::tpacket2_hdr const & hdr() const;

        unsigned char * b_;
        unsigned char * e_;
        unsigned hdrlen_;

        friend struct QueueReadPolicy;
        friend struct QueueWritePolicy;
    };

    /// \addtogroup policy_impl_group
    //\{

    /** \brief ReadPolicy for sockets readable via packet queue API

        This policy provides support for reading via the linux specific packet queue API
     */
    struct QueueReadPolicy
        : public ReadPolicyBase,
          private detail::QueuePolicyBase
    {
        typedef SocketQueueBuffer Buffer;
        typedef SocketQueueBuffer::size_type size_type;

        static boost::optional<Buffer> dequeue(FileHandle & handle);
        static void release(FileHandle & handle);

        // classic read API
        static unsigned read(FileHandle & handle, char * buffer, unsigned size);
    };

    /** \brief WritePolicy for sockets writeable via packet queue API

        This policy provides support for writing via the linux specific packet queue API
     */
    struct QueueWritePolicy
        : public WritePolicyBase,
          private detail::QueuePolicyBase
    {
        typedef SocketQueueBuffer Buffer;
        typedef SocketQueueBuffer::size_type size_type;

        static boost::optional<Buffer> enqueue(FileHandle & handle);
        static void send(FileHandle & handle);

        // classic write API
        static unsigned write(FileHandle & handle, char const * buffer, unsigned size);
    };

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "QueueReadWritePolicy.cci"
//#include "QueueReadWritePolicy.ct"
//#include "QueueReadWritePolicy.cti"
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
