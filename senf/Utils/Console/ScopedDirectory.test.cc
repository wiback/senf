// $Id$
//
// Copyright (C) 2008
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
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct TestObject {
        typedef TestObject Self;

        senf::console::ScopedDirectory<Self> dir;
        TestObject() : dir(this) {
            dir.add("member", senf::console::factory::Command(&Self::member, this));
        }

        void member(std::ostream & os, senf::console::ParseCommandInfo const &) {
            os << "member";
        }
    };
}

SENF_AUTO_UNIT_TEST(scopedDirectory)
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

SENF_AUTO_UNIT_TEST(scopedDirectoryVoid)
{
    namespace fty = senf::console::factory;

    {
        senf::console::ScopedDirectory<> dir;
        senf::console::root().add("dir", dir);
        dir.add("cb", fty::Command(&callback));
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

SENF_AUTO_UNIT_TEST(scopedDirectoryBase)
{
    namespace fty = senf::console::factory;

    {
        senf::console::ScopedDirectory<> dir;
        senf::console::root().add("dir", dir);
        dir.add("foo",fty::Directory());
        dir.add("cb", fty::Command(&callback));
        BOOST_CHECK( &dir["foo"] == &dir.get("foo") );
        BOOST_CHECK( &dir("cb") == &dir.get("cb") );
        BOOST_CHECK_EQUAL(dir.name(), "dir");

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
