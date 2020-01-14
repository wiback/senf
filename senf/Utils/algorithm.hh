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
