// $Id$
//
// Copyright (C) 2007
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
    \brief ClockService non-inline non-template implementation */

#include "ClockService.hh"
//#include "ClockService.ih"

// Custom includes
#include <boost/regex.hpp>
#include <senf/Utils/Console/Traits.hh>

//#include "ClockService.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void
senf::parseClockServiceInterval(console::ParseCommandInfo::TokensRange const & tokens,
                                ClockService::clock_type & out)
{
    out = 0;
    std::string value;
    {
        senf::console::CheckedArgumentIteratorWrapper arg (tokens);
        senf::console::parse( *(arg++), value );
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
        out += senf::ClockService::nanoseconds(senf::ClockService::int64_type(v));
        j = match.second;
    }
    if (j != value.end())
        throw senf::console::SyntaxErrorException();
}

prefix_ void senf::formatClockServiceInterval(ClockService::clock_type interval,
                                              std::ostream & os)
{
    os << interval << "ns";
}

///////////////////////////////cc.e////////////////////////////////////////
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
