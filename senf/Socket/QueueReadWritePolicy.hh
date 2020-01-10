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
        std::uint32_t const * timestampPtr() const;
                                        ///< return a pointer to the tp_hdr sec/nsec fieldes
        senf::LLSocketAddress address() const;
                                        ///< return incoming interface address
        boost::optional<unsigned> vlan() const;
                                        ///< return the preprocessed VLAN TCI if any
        unsigned tpid() const;
                                        ///< return the preprocessed VLAN TPID

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
        static bool send(FileHandle & handle);
        static void flush(FileHandle & handle);

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
