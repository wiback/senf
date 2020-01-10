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
    \brief ScopedDirectory unit tests */

// Custom includes
#include <sstream>
#include <boost/iterator/transform_iterator.hpp>
#include "Console.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(scopedDirectory)
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

SENF_AUTO_TEST_CASE(scopedDirectoryVoid)
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

SENF_AUTO_TEST_CASE(scopedDirectoryBase)
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
