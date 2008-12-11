// $Id$
//
// Copyright (C) 2008 
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
    \brief SyslogUDPTarget non-inline non-template implementation */

#include "SyslogUDPTarget.hh"
//#include "SyslogUDPTarget.ih"

// Custom includes
#include <sstream>
#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>
#include "../../Socket/Protocols/INet/ConnectedUDPSocketHandle.hh"
#include "IOStreamTarget.hh"

//#include "SyslogUDPTarget.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::log::SyslogUDPTarget::SyslogUDPTarget(senf::INet4Address const & target,
                                                    int facility)
    : facility_ (facility), tag_ (detail::getDefaultTag()),
      handle_ ( senf::ConnectedUDPv4ClientSocketHandle(senf::INet4SocketAddress(target, 514u)) ),
      showStream_ (false), showLevel_ (false), showArea_ (true)
{}

prefix_ senf::log::SyslogUDPTarget::SyslogUDPTarget(senf::INet4SocketAddress const & target,
                                                    int facility)
    : facility_ (facility), tag_ (detail::getDefaultTag()),
      handle_ ( senf::ConnectedUDPv4ClientSocketHandle(target) ),
      showStream_ (false), showLevel_ (false), showArea_ (true)
{}

prefix_ senf::log::SyslogUDPTarget::SyslogUDPTarget(senf::INet6Address const & target,
                                                    int facility)
    : facility_ (facility), tag_ (detail::getDefaultTag()),
      handle_ ( senf::ConnectedUDPv6ClientSocketHandle(senf::INet6SocketAddress(target, 514u)) ),
      showStream_ (false), showLevel_ (false), showArea_ (true)
{}

prefix_ senf::log::SyslogUDPTarget::SyslogUDPTarget(senf::INet6SocketAddress const & target,
                                                    int facility)
    : facility_ (facility), tag_ (detail::getDefaultTag()),
      handle_ ( senf::ConnectedUDPv6ClientSocketHandle(target) ),
      showStream_ (false), showLevel_ (false), showArea_ (true)
{}

prefix_ void senf::log::SyslogUDPTarget::v_write(time_type timestamp, std::string const & stream,
                                                 std::string const & area, unsigned level,
                                                 std::string const & message)
{
    std::stringstream prefix;

    prefix << '<' << (facility_ | senf::log::SyslogTarget::LEVELMAP[level]) << "> ";
    if (!tag_.empty())
        prefix << tag_ << ": ";
    if (showStream_)
        prefix << '[' << stream << "] ";
    if (showLevel_)
        prefix << '[' << LEVELNAMES[level] << "] ";
    if (showArea_ && area != "senf::log::DefaultArea")
        prefix << '[' << area << "] ";
    std::string m (message);
    boost::trim_right(m);
    detail::quoteNonPrintable(m);

    typedef boost::char_separator<char> Separator;
    typedef boost::tokenizer<Separator> Tokenizer;
    Separator separator ("\n");
    Tokenizer tokenizer (m, separator);
    Tokenizer::iterator i (tokenizer.begin());
    Tokenizer::iterator const i_end (tokenizer.end());

    std::string line;
    unsigned sz (896-prefix.str().size());
    for (; i != i_end; ++i) 
        for (unsigned j (0); j < i->size(); j += sz) {
            line = prefix.str();
            line += std::string(*i, j, sz);
            handle_.write(line);
        }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "SyslogUDPTarget.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
