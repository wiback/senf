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
    \brief PacketData public header */

#ifndef HH_SENF_Packets_PacketData_
#define HH_SENF_Packets_PacketData_ 1

// Custom includes
#include <boost/noncopyable.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <senf/Utils/Exception.hh>

//#include "PacketData.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    class PacketInfo;

    /** \brief Packet data STL-sequence view

        The PacketData class provides an STL-sequence compatible view of the raw packet data. Each
        packet/header/interpreter in the chain references the same storage area, presenting a
        different (but nested/overlapping) section of the data.

        Whenever the data is manipulated through PacketData, the change is assumed to be within the
        data range of that packet: All insertions take place \e inside \c this packet and \e outside
        any following packets in the packet chain.

        \warning It is not permissible to change data belonging to a following
            packet/header/interpreter even though this data is part of \c this sequence. Doing so
            will corrupt the packet data.

        \par

        \warning When accessing packet data via the PacketData interface you are on your own: The
            packet is not validated in any way, you bypass all parsers.

        All public members are those of an STL random-access sequence.

        \implementation This class is very tightly integrated with PacketInterpreterBase /
            PacketInterpreter. It is separated out of those classes primarily to provide a clean
            sequence interface to the library user and not for implementation reasons (it would have
            been simpler to implement all these members in PacketInterpreterBase).

        \ingroup packet_module
      */
    class PacketData
        : boost::noncopyable
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef senf::detail::packet::iterator iterator;
        typedef senf::detail::packet::const_iterator const_iterator;
        typedef senf::detail::packet::size_type size_type;
        typedef senf::detail::packet::difference_type difference_type;
        typedef senf::detail::packet::byte byte;
        typedef byte value_type;
        typedef byte & reference;
        typedef byte const & const_reference;
        typedef byte * pointer;
        typedef byte const * const_pointer;

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        // no public constructors
        // no conversion constructors

        //\}
        //-////////////////////////////////////////////////////////////////////////

        ///\name Sequence interface to raw data
        //\{

        iterator begin() const; ///< Return iterator to beginning
                                /**< Returns an <em>random access iterator</em> referring to the
                                     first byte of the packet data. */
        iterator end() const; ///< Return iterator to end
                              /**< Returns an <em>random access iterator</em> referring to the
                                   byte past the end of the packet data. */
        size_type size() const; ///< Returns the number of bytes in the packet data.
        bool empty() const; ///< Test whether the packet data is empty.
                            /**< Returns whether the packet data is empty, i.e. whether its size
                                 is 0. This function does not modify the content of the packet
                                 data in any way. To clear the content use clear() */
        byte operator[](size_type n) const; ///< Access byte in the packet data
        byte & operator[](size_type n); ///< Access byte in the packet data

        // Modifying the raw packet data

        // IMPORTANT NOTE: It is not possible to insert data AFTER an empty packet
        // since for an empty packet begin() == end(). However, I hope this problem is
        // only academic since what should an empty packet be good for ?
        void insert(iterator pos, byte v);
        void insert(iterator pos, size_type n, byte v);
#       ifndef DOXYGEN
        template <class InputIterator>
        void insert(iterator pos, InputIterator f, InputIterator l,
                    typename boost::disable_if< boost::is_convertible<InputIterator,size_type> >::type * = 0);
#       else
        template <class InputIterator>
        void insert(iterator pos, InputIterator f, InputIterator l);
#       endif

        void erase(iterator pos);
        void erase(iterator first, iterator last);
        void clear(); ///< All bytes of the packet data dropped, leaving the container with a size of 0. */
        void zero(iterator first, iterator last);

        void resize(size_type n, byte v=0);

        bool usingExternalMemory() const; ///< Check for external memory usage
                                        /**< Returns \c true, if the packet data resides in external
                                             memory not managed by the packet, \c false otherwise.

                                             Normally, this member will return \c false. If however
                                             the Packet has been constructed using the special
                                             zero-copy external memory constructor, \c
                                             usingExternalMemory() will return \c true as long as no
                                             reallocation has occurred. */
        void releaseExternalMemory();   ///< Release external memory
                                        /**< If \c usingExternalMemory() returns \c true, this
                                             member will force a reallocation of the packet data
                                             into internally managed memory. This operation does
                                             not change the packet in any way it only changes the
                                             location, the packet data is stored at. This operation
                                             will invalidate any iterators.

                                             If \c usingExternalMemory() is \c false, this function
                                             is a no-op. */

        //\}

    protected:
        PacketData(size_type b, size_type e);

        /// Need to make this protected so we can change it in the derived class
        detail::PacketImpl * impl_;

        detail::PacketImpl & impl() const;

        bool valid();

    private:
        size_type begin_;
        size_type end_;

        friend class detail::PacketImpl;
        friend class PacketInfo;
    };


    /** \brief Invalid packet data access

        This exception is signaled whenever an operation tries to access an out-of-bounds data
        byte. If the packet has been implemented correctly, this signals a malformed packet.
     */
    struct TruncatedPacketException : public senf::Exception
    {
        TruncatedPacketException(std::string const & description = "")
            : senf::Exception("truncated packet") { append( description); }
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_PacketData_i_)
#define HH_SENF_Packets_PacketData_i_
#include "PacketData.cci"
//#include "PacketData.ct"
#include "PacketData.cti"
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
