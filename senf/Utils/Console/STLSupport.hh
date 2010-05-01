// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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
    \brief STLSupport public header */

#ifndef HH_SENF_Utils_Console_STLSupport_
#define HH_SENF_Utils_Console_STLSupport_ 1

// Custom includes
#include <vector>
#include <list>
#include "Traits.hh"

//#include "STLSupport.mpp"
#include "STLSupport.ih"
///////////////////////////////hh.p////////////////////////////////////////

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

///////////////////////////////hh.e////////////////////////////////////////
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
