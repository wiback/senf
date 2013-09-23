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
#include <boost/optional.hpp>
#include <boost/range/iterator_range.hpp>
#include "SocketPolicy.hh"
#include "FileHandle.hh"

//#include "QueueReadWritePolicy.mpp"
#include "QueueReadWritePolicy.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup policy_impl_group
    //\{

    /** \brief ReadPolicy for sockets readable via packet queue API

        This policy provides support for reading via the linux specific packet queue API
     */
    struct QueueReadPolicy
        : public ReadPolicyBase,
          private detail::QueuePolicyBase
    {
        typedef detail::QueuePolicyBase::Buffer Buffer;
        typedef detail::QueuePolicyBase::size_type size_type;

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
        typedef detail::QueuePolicyBase::Buffer Buffer;
        typedef detail::QueuePolicyBase::size_type size_type;

        static boost::optional<Buffer> allocate(FileHandle & handle);
        static void enqueue(FileHandle & handle, Buffer & buffer, size_type size);
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
