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
    \brief IOStreamTarget non-inline non-template implementation */

#include "IOStreamTarget.hh"
//#include "IOStreamTarget.ih"

// Custom includes
#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>

//#include "IOStreamTarget.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::log::IOStreamTarget

prefix_ void senf::log::IOStreamTarget::v_write(time_type timestamp,
                                                std::string const & stream,
                                                std::string const & area, unsigned level,
                                                std::string const & message)
{
    std::string m (message);
    boost::trim_right(m);
    detail::quoteNonPrintable(m);

    if (isPlainFormat())
        *stream_ << m << std::endl;
    else {
        std::string const & prf (prefix(timestamp, stream, area, level));

        typedef boost::char_separator<char> Separator;
        typedef boost::tokenizer<Separator> Tokenizer;
        Separator separator ("\n");
        Tokenizer tokenizer (m, separator);
        Tokenizer::iterator i (tokenizer.begin());
        Tokenizer::iterator const i_end (tokenizer.end());

        for (; i != i_end; ++i)
            *stream_ << prf << *i << "\n";
        *stream_ << std::flush;
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "IOStreamTarget.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
