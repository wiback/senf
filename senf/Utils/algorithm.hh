// $Id$
//
// Copyright (C) 2012
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief Algorithm public header */

#ifndef HH_SENF_Utils_algorithm_
#define HH_SENF_Utils_algorithm_ 1

// Custom includes
#include <set>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_unordered_map.hpp>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    template <typename InputIterator, typename ValueType>
    bool contains(InputIterator first, InputIterator last, ValueType const & value);

    template <typename Container, typename ValueType>
    bool contains(Container const & container, ValueType const & value);

    template <typename ValueType>
    bool contains(std::set<ValueType> const & container, ValueType const & value);

    template <typename KeyType, typename ValueType>
    bool contains(std::map<KeyType, ValueType> const & container, KeyType const & key);

    template <typename KeyType, typename ValueType>
    bool contains(boost::ptr_map<KeyType, ValueType> const & container, KeyType const & key);

    template <typename KeyType, typename ValueType>
    bool contains(boost::ptr_unordered_map<KeyType, ValueType> const & container, KeyType const & key);

    bool contains(std::string const & str, std::string const & substr);
    bool contains(std::string const & str, char c);

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "algorithm.cci"
//#include "algorithm.ct"
#include "algorithm.cti"
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
