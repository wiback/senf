// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief GenericTLV public header */

#ifndef HH_SENF_Packets_GenericTLV_
#define HH_SENF_Packets_GenericTLV_ 1

// Custom includes
#include <boost/ptr_container/ptr_map.hpp>
#include <senf/Packets/Packets.hh>
#include <senf/Utils/type_traits.hh>
#include <senf/Utils/singleton.hh>

//#include "GenericTLV.hh.mpp"
///////////////////////////////hh.p////////////////////////////////////////

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
            SENF_PARSER_FIELD    ( type,   senf::UInt8Parser );
            SENF_PARSER_FIELD_RO ( length, senf::UInt8Parser );
            SENF_PARSER_FINALIZE ( MyTLVParserBase           );
        };
        \endcode
       
        Your concrete TLV parsers will inherit from this base class and have to define a specific
        value field and a \c typeId member:
        \code
        struct MyConcreteTLVParser : public MyTLVParserBase
        {
        #   include SENF_PARSER()
            SENF_PARSER_INHERIT  ( MyTLVParserBase             );
            SENF_PARSER_FIELD    ( myValue, senf::UInt32Parser );
            SENF_PARSER_FINALIZE ( MyConcreteTLVParser         );
         
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
            SENF_PARSER_FIELD_RO ( list_length, senf::UInt8Parser );
            SENF_PARSER_LIST     ( tlv_list, list_length, MyGenericTLVParser );
            SENF_PARSER_FINALIZE ( MyTestPacketParser );
        };
        \endcode
        
        Now, you can access the TLV parsers in the list in a generic way or you
        can cast the parsers to some concrete tlv parser:
        \code
        MyTestPacket p (...
        typedef MyTestPacket::Parser::tlv_list_t::container container_t;
        container_t tlvContainer (p->tlv_list() );
        optContainer_t::iterator listIter (tlvContainer.begin());
        
        // listIter points to a MyGenericTLVParser, so you have generic access:
        listIter->type() = 0x42;
        listIter->value( someRangeOfValueData);
        
        // cast to an instance of MyConcreteTLVParser:
        if (listIter->is<MyConcreteTLVParser>()) {
            MyConcreteTLVParser concreteTLVParser ( listIter->as<MyConcreteTLVParser>());
            concreteTLVParser.myValue() = 0xabababab;
        }
        
        // add a MyConcreteTLV to the list:
        MyConcreteTLVParser tlv ( tlvContainer.push_back_space().init<MyConcreteTLVParser>());
        tlv.myValue() = 0xffff;
        \endcode  

        \see 
            IPv6GenericOptionTLVParser, WLANGenericInfoElementParser, MIHGenericTLVParser 
     */
    template <class Base>
    class GenericTLVParserBase : public Base
    {
    public:
        GenericTLVParserBase(senf::PacketParserBase::data_iterator i, senf::PacketParserBase::state_type s) 
            : Base(i,s) {}
    
        senf::PacketParserBase::size_type bytes();
        void init() const;

        template <class Parser>
        Parser init();
        
        template <class Parser>
        Parser as() const;
        
        template <class Parser>
        bool is() const;

        senf::PacketInterpreterBase::range value() const;
        
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
    
    
    namespace detail {
        template <class BaseParser>
        struct GenericTLVParserRegistry_EntryBase {
            virtual void dump(GenericTLVParserBase<BaseParser> const & parser, std::ostream & os) = 0;
        };
    
        template <class BaseParser, class Parser>
        struct GenericTLVParserRegistry_Entry
            : GenericTLVParserRegistry_EntryBase<BaseParser>
        {
            virtual void dump(GenericTLVParserBase<BaseParser> const & parser, std::ostream & os);
        };
    }
    
    template <class BaseParser>
    class GenericTLVParserRegistry
        : public senf::singleton<GenericTLVParserRegistry<BaseParser> >
    {
        typedef boost::ptr_map<
            typename BaseParser::type_t::value_type, 
            detail::GenericTLVParserRegistry_EntryBase<BaseParser> > Map;
        Map map_;
        
        GenericTLVParserRegistry() {};
    public:
        using senf::singleton<GenericTLVParserRegistry<BaseParser> >::instance;
        friend class senf::singleton<GenericTLVParserRegistry<BaseParser> >;
        
        template <class PacketParser>
        struct RegistrationProxy {
            RegistrationProxy();
        };

        template <typename Parser>
        void registerParser();
        
        void dump(GenericTLVParserBase<BaseParser> const & parser, std::ostream & os);
    };
        
#   define SENF_PACKET_TLV_REGISTRY_REGISTER( BaseTLVParser, ConreteTLVParser )     \
        namespace {                                                                 \
            senf::GenericTLVParserRegistry<BaseTLVParser>                           \
                ::RegistrationProxy<ConreteTLVParser>                               \
                    BOOST_PP_CAT(tlvparserRegistration_, __LINE__);                 \
        }
        
}

///////////////////////////////hh.e////////////////////////////////////////
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
