// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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
    \brief Parser framework
 */

#ifndef HH_ParserBase_
#define HH_ParserBase_ 1

// Custom includes
#include <utility>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/type_traits/is_member_function_pointer.hpp>

#include "ParserBase.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace pkf {
    
    namespace impl { struct ParserBase; }

    struct nil 
	: public boost::iterator_facade<nil,char,boost::random_access_traversal_tag>
    {
	// Theese are declared to make nil a valid iterator. All
	// access to an instance of this iterator however is invalid
	// (these members are not implemented only declared)
	char & dereference() const;
	bool equal(nil other) const;
	void increment();
	void decrement();
	void advance(int n);
	int distance_to(nil other) const;
    };

    /** \brief Parser framework

        This class is the baseclass for all parser classes of the
        parser framework. The parser framework is used to interpret
        byte-oriented data from arbitrary random access iterators. The
        framework is hierarchical in the sense, that parsers can be
        arbitrarily nested.

        All parser framework classes are as lightweight as
        possible. Most parser classes only have a single iterator as
        data member and (depending on the container) therefore have
        the same size as a single pointer. Parsers are therefore
        conceptually and in essence simply pointers decorated with
        type information.

        It is very important for parser classes to be lightweight and
        to have only simple constructors since parsers are passed
        around by value. Parser instances most of the time are
        temporaries. However, since they are only 'decorated'
        pointers, this should not have any performance impact.

        To implement a new parser, write a template implementing the
        following members:
        
        \code
            template <class Iterator=nil, class IPacket=nil>
            struct Parser_Example
                : protected satcom::pkf::ParserBase<Iterator,IPacket>
            {
                // fixed interface of all parser classes

                template <class I=nil, class P=nil>
                struct rebind { typedef Parse_Example<I,P> parser; }
                typedef Iterator byte_iterator;

                Parse_Example() {}
                Parse_Example(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}
        
                [static] unsigned bytes()
                {
                    // return the size of the parsed header. This
                    // method must be declared static if the size is
                    // constant, otherwise it must be a non-static
                    // member
                    return 14;
                }

                static bool check(Iterator const & begin, Iterator const & end)
                {
                    BOOST_ASSERT( end>=begin );
                    // return true, if the data in the range [begin,end)
                    // can be safely interpreted by the parser without
                    // causing invalid memory access. This means,
                    // check, wether the data is truncated
                    return static_cast<unsigned>(end-begin) >= bytes();
                }

                // optional, only needed if bytes() is non-static
                static unsigned min_bytes()
                {
                    // return the minimum size of the header. This
                    // is the amount of space needed to allocate
                    // an otherwise empty packet
                    return 10;
                }

                // optional
                void init()
                {
                    // initialize the packet if necessary
                }

                // optional
                void init(Iterator b, Iterator e)
                {
                    // initialize the packet with given payload
                }

                // example methods to parse fields

                typedef Parse_UInt16 < Iterator >                    Parse_Field1;
                typedef Parse_Array  < 3, Parse_UInt32<>, Iterator > Parser_Field2

                Parse_Field1 field1() const { return Parse_Field1 (this->i()); }
                Parse_Field2 field2() const { return Parse_Field2 (this->i()+2); }
            };
        \endcode
        
        Every parser must have some mandatory fixed members which are:
        
        - struct rebind: This structure allows the parser to be
          converted to a parser of the same type but with a different
          iterator. Parser may have more than the two standard
          template parameters. These parameters must then be added in
          front of the standard parameters. The rebind structure must
          however always have only two parameters. Additional
          parameters must be provided from the outside template

        - byte_iterator: A typedef for the Iterator class used
        
        - Non Iterator constructor: This constructor is only used when
          the parser is inherited into a Packet class.
        
        - Iterator constructor: This constructor must call the
          corresponding ParserBase constructor.

        - unsigned bytes() member: This member must return the number
          of bytes the parser interprets. This will be the size of the
          implemented header. If the header has a fixed size, this
          member must be static, if it is dynamic the member must be
          non-static

        - static bool check(Iterator b, Iterator e) member: This
          method must return true \e only if the range [b,e) contains
          a \e complete packet, that is, e-b >= bytes(). However, the
          call to bytes() might involve accessing data bytes which
          might not exist. The bytes() call cannot check this (it has
          no access to the \e end of the valid range). To keep the
          performance up, the validity check is performed once. The
          parser has to ensure, that validity is maintained even when
          changing the values. Validity in this context does not
          imply, that the packet is semantically correct, it only
          implies, that the packet can be parsed without risking
          invalid memory access.

        - The min_bytes() member is optional. It is only used, if the
          Parser implements a non-fixed-size Packet, that is, if the
          bytes() member is non-static. In this case, min_bytes() has
          to be implemented and must return the amount of space
          necessary to construct an empty instance. The construction
          will proceed by first allocating the necessary space
          somewhere, initializing this space with all zeros. Then a
          Parser instance is created at that space and the Parsers
          init() method is called.

        - The init() member is optional. If all-zero initialization of
          a new Packet is enough, this member can be
          skipped. Otherwise, the init() member can assume to have
          access to a min_buytes() sized area which is all-zero
          initialized.

        - The init(Packet::ptr payload) member is optional. By default
          it just calls the init() member. Here, special
          initialization regarding the payload may be done. As for
          min_bytes(Packet::ptr), the argument type is allowed to be
          templatized or may be a specific packet ptr thereby
          restricting the permissible payload packet types.

        - The parser then contains any additional methods to parse the
          header constituents.

        ParserBase provides the parser classes with access to the
        packet iterator. This class is templatized on the Iterator
        type and an optional baseclass type.

        If the baseclass is given, it is used to access the iterator
        directly using 'begin'. If it is not given, the instance has
        to be constructed with an iterator.

        This implementation ensures, that a parser can either be
        inherited into a Packet class or be used as a temporary.
      */
    template <class Iterator, class IPacket=nil>
    class ParserBase : public impl::ParserBase
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{
        
        // default default constructor
        // default copy constructor
        // default copy assignment
        // default destructor
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        Iterator i() const;
        static void init() {};

    private:
        
    };

    template <class Iterator>
    class ParserBase<Iterator,nil> : public impl::ParserBase
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        explicit ParserBase(Iterator const & i);

        // no default constructor
        // default copy constructor
        // default copy assignment
        // default destructor
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        Iterator i() const;
        static void init() {}
        template <class SomePacket>
        static void init(typename SomePacket::ptr) {}
        
    private:

        Iterator i_;
    };

    /** \brief Addtiional Parser information
        
        Parser_traits provids abstract information about an unknown
        parser. Besides the information already available within the
        Parser it provides an additional 'fixed_sized' member which is
        true if and only if the Parser has a static bytes() member.
     */
    template <class Parser>
    struct Parser_traits {
        typedef Parser parser;
        typedef typename Parser::byte_iterator byte_iterator;
        static const bool fixed_size = impl::Parser_traits_fixed_size<Parser>::fixed_size;

        template <class I=nil, class P=nil>
        struct rebind {
            typedef typename Parser::template rebind<I,P>::parser parser;
        };
    };

    template <class Parser, class Iterator>
    bool check(Iterator const & b, Iterator const & e);

    template <class Parser>
    unsigned min_bytes();
    
}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ParserBase.cci"
//#include "ParserBase.ct"
#include "ParserBase.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
