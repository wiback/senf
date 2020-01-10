//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief Traits non-inline non-template implementation */

#include "Traits.hh"

// Custom includes
#include <senf/Utils/String.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ long senf::console::detail::parseEnum(EnumTable const & table,
                                              ParseCommandInfo::TokensRange const & tokens)
{
    if (tokens.size() != 1)
        throw SyntaxErrorException("parameter syntax error");

    std::string sym (tokens.begin()[0].value());
    boost::algorithm::to_lower(sym);
    EnumTable::left_map::const_iterator i1 (table.left.lower_bound(sym));
    EnumTable::left_map::const_iterator i2 (table.left.lower_bound(sym+"\xff"));
    if (i1 == i2)
        throw SyntaxErrorException("parameter syntax error: invalid enum value: ")
            << tokens.begin()[0].value();
    long v (i1->second);
    if (boost::algorithm::to_lower_copy(i1->first) == sym)
        return v;
    ++i1;
    if (i1 != i2)
        throw SyntaxErrorException("parameter syntax error: ambiguous enum value: ")
            << tokens.begin()[0].value();
    return v;
}

prefix_ std::string senf::console::detail::formatEnum(EnumTable const & table, long value)
{
    EnumTable::right_map::const_iterator i (table.right.find(value));
    if (i == table.right.end())
        return "invalid enum value: " + senf::str(value);
    return i->second;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
