// $Id$
//
// Copyright (C) 2009
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
    \brief STLSupport public header */

#ifndef HH_SENF_Utils_Console_STLSupport_
#define HH_SENF_Utils_Console_STLSupport_ 1

// Custom includes
#include <vector>
#include <list>
#include "Traits.hh"

//#include "STLSupport.mpp"
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
