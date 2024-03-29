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
    \brief PacketVector public header */

#ifndef HH_SENF_Packets_PacketVector_
#define HH_SENF_Packets_PacketVector_ 1

// Custom includes
#include <boost/noncopyable.hpp>
#include <boost/cstdint.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <senf/config.hh>
#include <cstddef>

//#include "PacketVector.mpp"
///////////////////////////////hh.p////////////////////////////////////////

#ifndef SENF_PACKET_STD_CONTAINER

namespace senf {

    /** \brief
      */
    class PacketVector
        : boost::noncopyable
    {
        struct PacketVectorPoolTag {};
        typedef boost::singleton_pool<
            PacketVectorPoolTag, 1u << SENF_PACKET_VECTOR_SIZE_INDEX> Pool;

    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::uint8_t value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef value_type * iterator;
        typedef value_type const * const_iterator;

        static size_type const ChunkSize = 1u << SENF_PACKET_VECTOR_SIZE_INDEX;
        static size_type const ChunkSizeIndex = SENF_PACKET_VECTOR_SIZE_INDEX;
        static size_type const HeadRoom = SENF_PACKET_VECTOR_HEADROOM;
        static size_type const TailRoom = SENF_PACKET_VECTOR_TAILROOM;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // my default constructor
        // disabled copy constructor
        // disabled copy assignment
        // my destructor

        PacketVector();
        PacketVector(size_type requestSize, value_type initValue);
        template <class ForwardIterator>
        PacketVector(ForwardIterator f, ForwardIterator l);
        PacketVector(value_type * data, size_type size,
                     size_type chunkSize = 0u, size_type offset = 0u);

        ~PacketVector();

        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        iterator begin();
        iterator end();
        const_iterator begin() const;
        const_iterator end() const;

        size_type size() const;
        void clear();

        bool external() const;
        void releaseExternal();

        void erase(iterator pos);
        void erase(iterator first, iterator last);
        void truncateFront(iterator pos);
        void truncateBack(iterator pos);

        void insert(iterator pos, value_type v);
        void insert(iterator pos, size_type n, value_type v);
        template <class ForwardIterator>
        void insert(iterator pos, ForwardIterator f, ForwardIterator l);

    protected:

    private:
        static size_type allocationSize(size_type sz);
        iterator move(iterator pos, size_type n);
        iterator moveGrow(iterator pos, size_type n);
        iterator grow(iterator pos, size_type n);

        size_type size_;
        value_type * data_;
        iterator b_;
        iterator e_;
        bool owner_;
    };

}

#endif

///////////////////////////////hh.e////////////////////////////////////////
#include "PacketVector.cci"
//#include "PacketVector.ct"
#include "PacketVector.cti"
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
