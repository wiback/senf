// $Id$
//
// Copyright (C) 2007
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
        stream_ << m << std::endl;
    else {
        std::string const & prf (prefix(timestamp, stream, area, level));

        typedef boost::char_separator<char> Separator;
        typedef boost::tokenizer<Separator> Tokenizer;
        Separator separator ("\n");
        Tokenizer tokenizer (m, separator);
        Tokenizer::iterator i (tokenizer.begin());
        Tokenizer::iterator const i_end (tokenizer.end());

        for (; i != i_end; ++i)
            stream_ << prf << *i << "\n";
        stream_ << std::flush;
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
