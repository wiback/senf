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
    \brief STLSupport public header */

#ifndef HH_SENF_Utils_Console_STLSupport_
#define HH_SENF_Utils_Console_STLSupport_ 1

// Custom includes
#include <vector>
#include <list>
#include "Traits.hh"
#ifdef SENF_CXX11_ENABLED
#  include <tuple>
#endif

#include "STLSupport.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace console {

#ifndef DOXYGEN

    template <class T, class Alloc>
    struct ArgumentTraits< std::vector<T,Alloc> >
        : public detail::CollectionArgumentTraits< std::vector<T,Alloc>,
                                                   detail::PushBackFunctor >
    {};

    template <class T, class Alloc>
    struct ReturnValueTraits< std::vector<T,Alloc> >
        : public detail::CollectionReturnValueTraits< std::vector<T,Alloc> >
    {};

    template <class T, class Alloc>
    struct ArgumentTraits< std::list<T,Alloc> >
        : public detail::CollectionArgumentTraits< std::list<T,Alloc>,
                                                   detail::PushBackFunctor >
    {};

    template <class T, class Alloc>
    struct ReturnValueTraits< std::list<T,Alloc> >
        : public detail::CollectionReturnValueTraits< std::list<T,Alloc> >
    {};

    template <class Key, class Compare, class Alloc>
    struct ArgumentTraits< std::set<Key,Compare,Alloc> >
        : public detail::CollectionArgumentTraits< std::set<Key,Compare,Alloc>,
                                                   detail::InsertFunctor >
    {};

    template <class Key, class Compare, class Alloc>
    struct ReturnValueTraits< std::set<Key,Compare,Alloc> >
        : public detail::CollectionReturnValueTraits< std::set<Key,Compare,Alloc> >
    {};

    template <class Key, class Compare, class Alloc>
    struct ArgumentTraits< std::multiset<Key,Compare,Alloc> >
        : public detail::CollectionArgumentTraits< std::multiset<Key,Compare,Alloc>,
                                                   detail::InsertFunctor >
    {};

    template <class Key, class Compare, class Alloc>
    struct ReturnValueTraits< std::multiset<Key,Compare,Alloc> >
        : public detail::CollectionReturnValueTraits< std::multiset<Key,Compare,Alloc> >
    {};

    template <class Key, class Data, class Compare, class Alloc>
    struct ArgumentTraits< std::map<Key,Data,Compare,Alloc> >
        : public detail::MapArgumentTraits< std::map<Key,Data,Compare,Alloc> >
    {};

    template <class Key, class Data, class Compare, class Alloc>
    struct ReturnValueTraits< std::map<Key,Data,Compare,Alloc> >
        : public detail::MapReturnValueTraits< std::map<Key,Data,Compare,Alloc> >
    {};

    template <class Key, class Data, class Compare, class Alloc>
    struct ArgumentTraits< std::multimap<Key,Data,Compare,Alloc> >
        : public detail::MapArgumentTraits< std::multimap<Key,Data,Compare,Alloc> >
    {};

    template <class Key, class Data, class Compare, class Alloc>
    struct ReturnValueTraits< std::multimap<Key,Data,Compare,Alloc> >
        : public detail::MapReturnValueTraits< std::multimap<Key,Data,Compare,Alloc> >
    {};

    template <class T1, class T2>
    struct ArgumentTraits< std::pair<T1,T2> >
    {
        typedef std::pair<T1,T2> type;
        static bool const singleToken = false;

        static void parse(ParseCommandInfo::TokensRange const & tokens, type & out);
        static std::string description();
        static std::string str(type const & value);
    };

    template <class T1, class T2>
    struct ReturnValueTraits< std::pair<T1,T2> >
    {
        typedef std::pair<T1,T2> type;

        static void format(type const & value, std::ostream & os);
    };

#ifdef SENF_CXX11_ENABLED

    template<typename ...Types>
    struct ArgumentTraits< std::tuple<Types...> >
    {
        typedef std::tuple<Types...> type;
        static bool const singleToken = false;

        static void parse(ParseCommandInfo::TokensRange const & tokens, type & out);
        static std::string description();
        static std::string str(type const & value);
    };

    template<typename ...Types>
    struct ReturnValueTraits< std::tuple<Types...> >
    {
        typedef std::tuple<Types...> type;

        static void format(type const & value, std::ostream & os);
    };

#endif
#endif

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "STLSupport.cci"
#include "STLSupport.ct"
#include "STLSupport.cti"
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
