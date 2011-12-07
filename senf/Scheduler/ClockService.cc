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
    \brief ClockService non-inline non-template implementation */

#include "ClockService.hh"
//#include "ClockService.ih"

// Custom includes
#include <boost/regex.hpp>
#include <senf/Utils/Console/Traits.hh>
#include <senf/Utils/ClockTypeMacros.hh>
//#include "ClockService.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void
senf::parseClockServiceInterval(console::ParseCommandInfo::TokensRange const & tokens,
                                ClockService::clock_type & out)
{
    out = senf::ClockService::clock_type(0);
    std::string value;
    {
        console::CheckedArgumentIteratorWrapper arg (tokens);
        console::parse( *(arg++), value );
    }
    static boost::sregex_iterator::regex_type rx ("[mun]?[dhms]");
    boost::sregex_iterator i (value.begin(), value.end(), rx);
    boost::sregex_iterator const i_end;
    std::string::const_iterator j (value.begin());
    for (; i != i_end; ++i) {
        boost::sregex_iterator::value_type::value_type match ((*i)[0]);
        long double v (boost::lexical_cast<long double>(std::string(j, match.first)));
        char scale (0);
        char unit (0);
        if (match.length() == 2) {
            scale = *match.first;
            unit = *boost::next(match.first);
        } else {
            SENF_ASSERT( match.length() == 1,
                         "Internal failure: RegEx match returns weird number of matches" );
            unit = *match.first;
        }
        switch (scale) {
        case 0: break;
        case 'n': v /= 1000.0;
        case 'u': v /= 1000.0;
        case 'm': v /= 1000.0;
        }
        switch (unit) {
        case 'd': v *= 24.0;
        case 'h': v *= 60.0;
        case 'm': v *= 60.0;
        case 's': v *= 1000000000.0;
        }
        out += ClockService::nanoseconds(ClockService::int64_type(v));
        j = match.second;
    }
    if (j != value.end())
        throw console::SyntaxErrorException();
}

prefix_ void senf::formatClockServiceInterval(ClockService::clock_type interval,
                                              std::ostream & os)
{
    os << interval << "ns";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "ClockService.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
