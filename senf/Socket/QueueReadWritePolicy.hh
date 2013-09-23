// $Id$
//
// Copyright (C) 2013
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
    \brief QueueReadWritePolicy public header */

#ifndef HH_SENF_QueueReadWritePolicy_
#define HH_SENF_QueueReadWritePolicy_ 1

// Custom includes
#include <linux/if_packet.h>
#include <boost/optional.hpp>
#include <boost/range/iterator_range.hpp>
#include "Protocols/Raw/LLAddressing.hh"
#include "SocketPolicy.hh"
#include "FileHandle.hh"

//#include "QueueReadWritePolicy.mpp"
#include "QueueReadWritePolicy.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    class QueueReadPolicy;
    class QueueWritePolicy;

    /** \brief Buffer used by QueueReadPolicy / QueueWritePolicy

        This class provides a convenient interface to the frame buffers as utilized by the queue
        based read and write policy. This class resembles an iterator range with additional access
        to information found in the buffer header.

        The SocketQueueBuffer is a fixed space container which may be resized within the space
        available. There are several views into that space:

        \li \c begin(), \c end(), \c size() and \c empty() provide access to the active frame data
        \li \c frameBegin(), \c frameEnd() and \c frameSize() provide access to the maximum space
            usable for packet data
        \li \c spaceBegin(), \c spaceEnd() and \c spaceSize() provide access to additional space
            which is available for use but will overwrite any header information if used
     */
    class SocketQueueBuffer
    {
    public:
        typedef char * iterator;
        typedef char const * const_iterator;
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

        iterator spaceBegin();
        iterator spaceEnd();
        const_iterator spaceBegin() const;
        const_iterator spaceEnd() const;
        size_type spaceSize() const;

        senf::ClockService::clock_type timestamp() const;
                                        ///< return packet receive timestamp
        senf::LLSocketAddress address() const;
                                        ///< return incoming interface address

        void resize(size_type sz);      ///< resize active packet data to given size

    private:
        SocketQueueBuffer(char * b, char * e);

        struct ::tpacket2_hdr & hdr();
        struct ::tpacket2_hdr const & hdr() const;

        char * b_;
        char * e_;

        friend class QueueReadPolicy;
        friend class QueueWritePolicy;
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
