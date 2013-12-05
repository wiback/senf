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
    \brief Node unit tests */

//#include "Node.test.hh"
//#include "Node.test.ih"

// Custom includes
#include <sstream>
#include "Console.hh"
#include <boost/iterator/transform_iterator.hpp>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
using namespace senf;

SENF_AUTO_UNIT_TEST(genericNode)
{
    namespace fty = console::factory;

    console::GenericNode & node (
        console::root()
        .add("dir1", fty::Directory())
        .add("dir2", fty::Directory().doc("help info")));
    console::GenericNode::weak_ptr wp (node.thisptr());

    BOOST_CHECK_EQUAL( node.name(), "dir2" );
    BOOST_CHECK( node.parent() );
    BOOST_CHECK_EQUAL( node.path(), "/dir1/dir2" );
    BOOST_CHECK( node.active() );
    std::stringstream ss;
    node.help(ss);
    BOOST_CHECK_EQUAL( ss.str(), "help info\n" );

    {
        console::GenericNode::ptr p (console::root()["dir1"].unlink());
        BOOST_CHECK( ! node.active() );
        BOOST_CHECK( ! wp.expired() );
    }
    BOOST_CHECK( wp.expired() );
}

namespace {
    void callback(std::ostream & os, console::ParseCommandInfo const & command)
    {
        os << "callback";
    }

    template <class T>
    struct select1st {
        typedef T result_type;
        template <class U> result_type operator()(U const & u) const { return u.first; }
    };
}

SENF_AUTO_UNIT_TEST(directoryNode)
{
    namespace fty = console::factory;

    console::DirectoryNode::ptr p (console::DirectoryNode::create());

    BOOST_CHECK( & console::root().add("dir1", p) == p.get() );

    console::SimpleCommandNode & fnnode (
        console::root().add( "fn", console::SimpleCommandNode::create(&callback) ));
    BOOST_CHECK( &console::root()["dir1"] == p.get() );
    BOOST_CHECK_THROW( console::root()["dir2"], console::UnknownNodeNameException );
    BOOST_CHECK_THROW( console::root()("dir1"), std::bad_cast );
    BOOST_CHECK( &console::root()("fn") == &fnnode );
    BOOST_CHECK_THROW( console::root()("fn2"), console::UnknownNodeNameException );
    BOOST_CHECK_THROW( console::root()["fn"], std::bad_cast );
    BOOST_CHECK( &console::root().get("dir1") == p.get() );

    console::root()
        .add("dir2", fty::Directory())
        .add("dir3", fty::Directory());
    char const * const children[] = { "dir1", "dir2", "fn", "sys" };
    BOOST_CHECK_EQUAL_COLLECTIONS(
        boost::make_transform_iterator(console::root().children().begin(),
                                       select1st<std::string const &>()),
        boost::make_transform_iterator(console::root().children().end(),
                                       select1st<std::string const &>()),
        children,
        children+sizeof(children)/sizeof(children[0]) );

    char const * const completions[] = { "dir1", "dir2" };
    BOOST_CHECK_EQUAL_COLLECTIONS(
        boost::make_transform_iterator(console::root().completions("dir").begin(),
                                       select1st<std::string const &>()),
        boost::make_transform_iterator(console::root().completions("dir").end(),
                                       select1st<std::string const &>()),
        completions,
        completions+sizeof(completions)/sizeof(completions[0]) );

    p->doc("test doc\nline 2");
    std::stringstream ss;
    p->help(ss);
    BOOST_CHECK_EQUAL( ss.str(), "test doc\nline 2\n" );
    BOOST_CHECK_EQUAL( p->shorthelp(), "test doc" );
    p->doc("");
    BOOST_CHECK_EQUAL( p->shorthelp(), "" );
    p->shortdoc("short doc");
    BOOST_CHECK_EQUAL( p->shorthelp(), "short doc" );

    ss.str("");
    console::root()["dir2"].add("dir4", fty::Directory());
    console::root()["dir2"].add("link", fty::Link(*p));
    console::root()["dir2"]["dir4"].add("link", fty::Link(console::root()));
    console::dump(ss, console::root()["dir2"]);
    BOOST_CHECK_EQUAL( ss.str(),
                       "dir3/\n"
                       "dir4/\n"
                       "  link@ -> /\n"
                       "link@ -> /dir1\n" );

    BOOST_CHECK( console::root().remove("dir1") == p );
    console::root().remove("dir2");
    console::root().remove("fn");

    BOOST_CHECK_EQUAL( std::distance(console::root().children().begin(),
                                     console::root().children().end()), 1 );
}

SENF_AUTO_UNIT_TEST(linkNode)
{
    namespace fty = console::factory;

    console::root().add("dir1", fty::Directory());
    console::root().add("link1", fty::Link(console::root()["dir1"]));

    BOOST_CHECK( console::root()["dir1"] == console::root()["link1"] );

    console::root().remove("dir1");
    console::root().remove("link1");
}

namespace {
    struct Functor {
        void operator()(std::ostream & os, console::ParseCommandInfo const &) {
            os << "functor";
        }
    };
}

SENF_AUTO_UNIT_TEST(factory)
{
    namespace fty = console::factory;

    console::root().add("fn1", fty::SimpleCommand(&callback));
    console::root().add("fn2", fty::SimpleCommand(Functor()));

    console::ParseCommandInfo info;

    {
        std::stringstream ss;
        console::root()("fn1")(ss, info);
        BOOST_CHECK_EQUAL( ss.str(), "callback" );
    }

    {
        std::stringstream ss;
        console::root()("fn2")(ss, info);
        BOOST_CHECK_EQUAL( ss.str(), "functor" );
    }

    console::root().remove("fn1");
    console::root().remove("fn2");
}

SENF_AUTO_UNIT_TEST(simpleCommandNode)
{
    console::root().add("fn", console::SimpleCommandNode::create(&callback))
        .doc("help text");
    {
        std::stringstream ss;
        console::ParseCommandInfo info;
        console::root()("fn")(ss, info);
        BOOST_CHECK_EQUAL( ss.str(), "callback" );
    }

    {
        std::stringstream ss;
        console::root().get("fn").help(ss);
        BOOST_CHECK_EQUAL( ss.str(), "help text\n" );
    }

    console::root().remove("fn");
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
