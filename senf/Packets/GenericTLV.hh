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
    \brief GenericTLV public header */

#ifndef HH_SENF_Packets_GenericTLV_
#define HH_SENF_Packets_GenericTLV_ 1

// Custom includes
#include <boost/ptr_container/ptr_map.hpp>
#include <senf/Utils/type_traits.hh>
#include <senf/Utils/singleton.hh>
#include "Packets.hh"

#include "GenericTLV.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Base class for generic TLV parsers

       This abstract base class can be used to define generic TLV parsers. The following
       class structure is assumed:
       \image html GenericTLV.png

        Your TLVParser base class has to define a \c type and a \c length field:
        \code
        struct MyTLVParserBase : public senf::PacketParserBase
        {
        #   include SENF_PARSER()
            SENF_PARSER_FIELD   ( type,   senf::UInt8Parser );
            SENF_PARSER_FIELD_RO( length, senf::UInt8Parser );
            SENF_PARSER_FINALIZE( MyTLVParserBase           );
        };
        \endcode

        Your concrete TLV parsers will inherit from this base class and have to define a specific
        value field and a \c typeId member:
        \code
        struct MyConcreteTLVParser : public MyTLVParserBase
        {
        #   include SENF_PARSER()
            SENF_PARSER_INHERIT ( MyTLVParserBase             );
            SENF_PARSER_FIELD   ( myValue, senf::UInt32Parser );
            SENF_PARSER_FINALIZE( MyConcreteTLVParser         );

            SENF_PARSER_INIT() {
                type() = typeId;
                length_() = 4;
            }
            static const type_t::value_type typeId = 0x42;
        };
        \endcode

        With GenericTLVParserBase you can define a generic parser class which provides
        members to access the value data and and to cast the parser to a concrete tlv
        parser:
        \code
        struct MyGenericTLVParser : public senf::GenericTLVParserBase<MyTLVParserBase>
        {
            typedef senf::GenericTLVParserBase<MyTLVParserBase> base;
            MyGenericTLVParser(data_iterator i, state_type s) : base(i,s) {}

            // members for your generic TLV parser...
        };
        \endcode

        If your generic TLV parser just inherits from GenericTLVParserBase and doesn't
        add any additional functionality you can use a simple \c typedef as well:
        \code
        typedef senf::GenericTLVParserBase<MyTLVParserBase> MyGenericTLVParser;
        \endcode

        This generic tlv parser can now be used for example in a list:
        \code
        class MyTestPacketParser : public senf::PacketParserBase
        {
        #   include SENF_PARSER()
            SENF_PARSER_FIELD_RO( list_length, senf::UInt8Parser );
            SENF_PARSER_LIST    ( tlv_list, list_length, MyGenericTLVParser );
            SENF_PARSER_FINALIZE( MyTestPacketParser );
        };
        \endcode

        Now, you can access the TLV parsers in the list in a generic way or you
        can cast the parsers to some concrete tlv parser:
        \code
        MyTestPacket p (...
        typedef MyTestPacket::Parser::tlv_list_t::container_type container_t;
        container_t tlvContainer (p->tlv_list() );
        optContainer_t::iterator listIter (tlvContainer.begin());

        // listIter points to a MyGenericTLVParser, so you have generic access:
        listIter->type() = 0x42;
        listIter->value(someRangeOfValueData);

        // cast to an instance of MyConcreteTLVParser:
        if (listIter->is<MyConcreteTLVParser>()) {
            MyConcreteTLVParser concreteTLVParser (listIter->as<MyConcreteTLVParser>());
            concreteTLVParser.myValue() = 0xabababab;
        }

        // add a MyConcreteTLV to the list:
        MyConcreteTLVParser tlv (tlvContainer.push_back_space().init<MyConcreteTLVParser>());
        tlv.myValue() = 0xffff;
        \endcode

        \see
            IPv6GenericOptionParser, WLANGenericInfoElementParser, MIHGenericTLVParser \n
            GenericTLVParserRegistry
     */
    template <class Base>
    class GenericTLVParserBase
        : private detail::GenericTLVParserBaseTag,
          public Base
    {
    public:
        GenericTLVParserBase(PacketParserBase::data_iterator i, PacketParserBase::state_type s)
            : Base(i,s) {}

        static PacketParserBase::size_type const init_bytes = senf::init_bytes<Base>::value;

        PacketParserBase::size_type bytes() const;
        void init() const;

        template <class Parser>
        Parser init();

        template <class Parser>
        Parser as() const;

        template <class Parser>
        bool is() const;

        PacketInterpreterBase::range value() const;

        void dump(std::ostream & os) const;

#ifndef DOXYGEN
        template<class ForwardReadableRange>
        void value(ForwardReadableRange const & val,
                typename boost::disable_if<senf::is_pair<ForwardReadableRange> >::type * = 0);

        template<class First, class Second>
        void value(std::pair<First, Second> const & val,
                typename boost::disable_if<boost::is_convertible<First, typename Base::type_t::value_type> >::type * = 0);

        template <class Type, class ForwardReadableRange>
        void value(std::pair<Type, ForwardReadableRange> const & val,
                typename boost::enable_if<boost::is_convertible<Type, typename Base::type_t::value_type> >::type * = 0);
#else
        template<class ForwardReadableRange>
        void value(ForwardReadableRange const & val);

        template <class ForwardReadableRange>
        void value(std::pair<typename Base::type_t::value_type, ForwardReadableRange> const & val);
#endif

    private:
        template<class ForwardReadableRange>
        void value_(ForwardReadableRange const &range);

        Base & self();
        Base const & self() const;
    };


    /** \brief TLV parser registration facility

        The %GenericTLVParserRegistry provides a generic facility to globally register concrete
        TLV parser by the type value. The concrete TLV parser must therefore provide a \c typeId
        member. See GenericTLVParserBase for details about the assumed class structure.

        Every registry is identified by the base tlv parser class. Parsers can be registered
        statically only:
        \code
        GenericTLVParserRegistry<MyTLVParserBase>::RegistrationProxy<ConcreteTLVParser>
            registerConcreteTLVParser;
        \endcode
        This global variable declaration will register ConcreteTLVParser. The variable
        registerConcreteTLVParser is a dummy. It's only function is to force the call of
        it's constructor during global construction time. This static registration only
        works when the symbol is included into the final binary.

        To simplify the registration the \ref SENF_PACKET_TLV_REGISTRY_REGISTER macro can be used.
        The \c ConreteTLVParser must therefore provide a \c Registry typedef pointing to the
        %GenericTLVParserRegistry; typically you put this typedef to the TLVBaseParser class.
        \code
        struct MyTLVParserBase : public senf::PacketParserBase
        {
            ...
            typedef GenericTLVParserRegistry<MyTLVParserBase> Registry;
        };
        struct MyConcreteTLVParser : public MyTLVParserBase
        {
            ....
            static const type_t::value_type typeId = 0x42;
            void dump(std::ostream & os) const;
        };

        // register MyConcreteTLVParser to the MyTLVParserBase-Registry with the type id 0x42:
        SENF_PACKET_TLV_REGISTRY_REGISTER( MyConcreteTLVParser );
        \endcode

        The registry provides a dump() member to dump an instance of a generic TLV parser.
        If the type value of the given TLV parser is registered the generic tlv will be
        casted to the registered concrete TLV parser and the dump member from this parser
        will be called.

        \see
            GenericTLVParserBase for the general TLV class structure \n
            IPv6OptionParser::Registry, WLANInfoElementParser::Registry,
            MIHBaseTLVParser::Registry
     */
    template <class BaseParser, class Keytype = typename BaseParser::type_t::value_type>
    class GenericTLVParserRegistry
        : public singleton<GenericTLVParserRegistry<BaseParser,Keytype> >
    {
        GenericTLVParserRegistry() {};

        typedef boost::ptr_map<Keytype,
            detail::GenericTLVParserRegistry_EntryBase<BaseParser> > Map;
        Map map_;

    public:
        using singleton<GenericTLVParserRegistry<BaseParser,Keytype> >::instance;
        friend class singleton<GenericTLVParserRegistry<BaseParser,Keytype> >;

        template <class PacketParser>
        struct RegistrationProxy {
            RegistrationProxy();
        };

        template <typename Parser>
        void registerParser();

        typedef GenericTLVParserBase<BaseParser> GenericTLVParser;

        bool isRegistered(GenericTLVParserBase<BaseParser> const & parser) const;
        bool isRegistered(Keytype const & key) const;

        void dump(GenericTLVParser const & parser, std::ostream & os) const;
        void dump(GenericTLVParser const & parser, Keytype const & key, std::ostream & os) const;

        PacketParserBase::size_type bytes(GenericTLVParser const & parser) const;
        PacketParserBase::size_type bytes(GenericTLVParser const & parser, Keytype const & key) const;

        typedef typename Map::const_iterator const_iterator;
        const_iterator begin() const;
        const_iterator end() const;
    };

    struct TLVParserNotRegisteredException : public senf::Exception
    {
        TLVParserNotRegisteredException() : senf::Exception("tlv parser not registered") {}
    };


    template <class ListPolicy>
    class TLVListParser
        : public ListParser<ListPolicy>
    {
    public:
        TLVListParser(PacketParserBase::data_iterator i, PacketParserBase::state_type s);
        TLVListParser(ListPolicy policy, PacketParserBase::data_iterator i, PacketParserBase::state_type s);

        template <typename TLVParser>
        bool contains() const;
        bool contains(typename ListPolicy::element_type::type_t::value_type typeId) const;

        template <typename TLVParser>
        TLVParser find() const;
    };


    /** \brief Statically add an entry to a TLV parser registry

        This macro will declare an anonymous global variable in such a way, that constructing
        this variable will register the given tlv parser.

        \hideinitializer
        \see senf::GenericTLVParserRegistry
     */
#   define SENF_PACKET_TLV_REGISTRY_REGISTER( ConreteTLVParser )                \
        namespace {                                                             \
            ConreteTLVParser::Registry::RegistrationProxy<ConreteTLVParser>     \
                    BOOST_PP_CAT(tlvparserRegistration_, __LINE__);             \
        }

#   define SENF_PARSER_TLV_LIST SENF_PARSER_LIST
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "GenericTLV.cci"
#include "GenericTLV.ct"
#include "GenericTLV.cti"
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
