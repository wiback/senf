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
    \brief ScopedDirectory unit tests */

//#include "ScopedDirectory.test.hh"
//#include "ScopedDirectory.test.ih"

// Custom includes
#include <sstream>
#include "Console.hh"
#include <boost/iterator/transform_iterator.hpp>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct TestObject {
        typedef TestObject Self;

        senf::console::ScopedDirectory<Self> dir;
        TestObject() : dir(this) {
            dir.add("member", &Self::member);
        }

        void member(std::ostream & os, senf::console::ParseCommandInfo const &) {
            os << "member";
        }
    };
}

BOOST_AUTO_UNIT_TEST(scopedDirectory)
{
    {
        TestObject ob;
        senf::console::root().add("ob",ob.dir);
        std::stringstream ss;
        senf::console::ParseCommandInfo info;
        senf::console::root()["ob"]("member")(ss, info);
        BOOST_CHECK_EQUAL( ss.str(), "member" );
    }
    BOOST_CHECK_THROW( senf::console::root()["ob"], senf::console::UnknownNodeNameException );
}

namespace {
    void callback(std::ostream & os, senf::console::ParseCommandInfo const &) {
        os << "cb";
    }
}

BOOST_AUTO_UNIT_TEST(scopedDirectoryVoid)
{
    {
        senf::console::ScopedDirectory<> dir;
        senf::console::root().add("dir", dir);
        dir.add("cb", &callback);
        std::stringstream ss;
        senf::console::ParseCommandInfo info;
        senf::console::root()["dir"]("cb")(ss, info);
        BOOST_CHECK_EQUAL( ss.str(), "cb" );
    }
    BOOST_CHECK_THROW( senf::console::root()["dir"],
                       senf::console::UnknownNodeNameException );
}

namespace {
    template <class T>
    struct select1st {
        typedef T result_type;
        template <class U> result_type operator()(U const & u) const { return u.first; }
    };
}

BOOST_AUTO_UNIT_TEST(scopedDirectoryBase)
{
    {
        senf::console::ScopedDirectory<> dir;
        senf::console::root().add("dir", dir);
        dir.mkdir("foo");
        dir.add("cb", &callback);
        BOOST_CHECK( &dir["foo"] == &dir.get("foo") );
        BOOST_CHECK( &dir("cb") == &dir.get("cb") );
        
        char const * const children[] = { "cb", "foo" };
        BOOST_CHECK_EQUAL_COLLECTIONS( 
            boost::make_transform_iterator(dir.children().begin(),
                                           select1st<std::string const &>()),
            boost::make_transform_iterator(dir.children().end(),
                                           select1st<std::string const &>()),
            children, 
            children+sizeof(children)/sizeof(children[0]) );
        
        dir.doc("dir");
        std::stringstream ss;
        dir.node().help(ss);
        BOOST_CHECK_EQUAL( ss.str(), "dir\n" );
    }
}

///////////////////////////////cc.e////////////////////////////////////////
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
