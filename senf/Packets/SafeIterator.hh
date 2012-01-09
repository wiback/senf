// $Id$
//
// Copyright (C) 2008
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
    \brief SafeIterator public header */

#ifndef HH_SENF_Packets_SafeIterator_
#define HH_SENF_Packets_SafeIterator_ 1

#ifndef HH_SENF_Packets_Packets_
#error "Don't include 'SafeIterator.hh' directly, include 'Packets.hh'"
#endif

// Custom includes
#include <boost/iterator/iterator_facade.hpp>

#include "SafeIterator.ih"
//#include "SafeIterator.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Re-validating data iterator

        This class is a wrapper around a PacketData::iterator instance. It will revalidate the
        iterator on every access. This keeps the iterator valid even when the data container is
        resized and thereby possibly relocated. The iterator will always point to the byte at the
        same offset from the packets beginning. If data is inserted before this iterators position,
        the data pointed to will of course change.

        For this to work, the safe_data_iterator must be initialized with the container to which the
        iterator belongs. After this initialization it can be used like any other iterator.
     */
    class safe_data_iterator
        : public boost::iterator_facade< safe_data_iterator,
                                         PacketData::value_type,
                                         boost::random_access_traversal_tag >,
          public senf::detail::packet::safe_data_iterator_base<PacketData::iterator>
    {
        typedef senf::detail::packet::safe_data_iterator_base<PacketData::iterator> Base;
    public:
        typedef PacketData::size_type size_type;

        safe_data_iterator(); ///< Make uninitialized iterator
        explicit safe_data_iterator(PacketData & data);
                                        ///< Construct iterator only setting the data container
        safe_data_iterator(PacketData & data, PacketData::iterator i);
                                        ///< Initialize iterator to given position
        explicit safe_data_iterator(PacketParserBase const & parser);
                                        ///< Initialize iterator from parser
                                        /**< The iterator will point to the parsers start
                                             position. */

        safe_data_iterator & operator=(PacketData::iterator i); ///< Assign iterator
                                        /**< The iterator \a i must be from the container which \c
                                             this iterator has been initialized. */
        safe_data_iterator & operator=(PacketParserBase const & parser);
                                        ///< Assign iterator from parser
                                        /**< The iterator will point to the parser start
                                             position. */

        PacketData & data() const;      ///< Access data container

    private:
        friend class boost::iterator_core_access;

        // iterator_facade interface

        value_type & dereference() const;
        bool equal(safe_data_iterator const & other) const;
        difference_type distance_to(safe_data_iterator const & other) const;
        void increment();
        void decrement();
        void advance(difference_type n);
    };

    /** \brief Iterator re-validating Parser wrapper

        An ordinary parser will be invalidated whenever the raw data container's size is
        changed. This can complicate some algorithms considerably.

        This wrapper will update the parsers iterator (the value returned by the i() member) on
        every access. This ensures that the iterator will stay valid.

        \attention Beware however, if you insert or remove data before the safe wrapper, the
            location will \e not be updated accordingly and therefore the parser will be
            invalid.

        Additionally a SafePacketParserWrapper has an uninitialized state. The only allowed operations in
        this state are the boolean test for validity and assigning another parser.

        \ingroup packetparser
      */
    template <class Parser>
    class SafePacketParserWrapper
        : public safe_bool< SafePacketParserWrapper<Parser> >
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        // default copy constructor
        // default copy assignment
        // default destructor
        SafePacketParserWrapper();             ///< Create an empty uninitialized SafePacketParserWrapper

        // conversion constructors
        SafePacketParserWrapper(Parser parser); ///< Initialize SafePacketParserWrapper from \a parser

        SafePacketParserWrapper & operator=(Parser parser); ///< Assign \a parser to \c this

        //\}
        //-////////////////////////////////////////////////////////////////////////

        Parser & operator*() const;       ///< Access the stored parser
                                        /**< On every access, the stored parsers iterator will be
                                             updated / re-validated. */
        Parser * operator->() const; ///< Access the stored parser
                                        /**< On every access, the stored parsers iterator will be
                                             updated / re-validated. */
        bool boolean_test() const;      ///< Check validity

    protected:

    private:
        mutable boost::optional<Parser> parser_;
        senf::safe_data_iterator i_;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_SafeIterator_i_)
#define HH_SENF_Packets_SafeIterator_i_
#include "SafeIterator.cci"
//#include "SafeIterator.ct"
#include "SafeIterator.cti"
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
