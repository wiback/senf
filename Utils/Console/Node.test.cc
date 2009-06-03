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
    \brief Node unit tests */

//#include "Node.test.hh"
//#include "Node.test.ih"

// Custom includes
#include <sstream>
#include "Node.hh"
#include <boost/iterator/transform_iterator.hpp>

#include "../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(genericNode)
{
    senf::console::GenericNode & node (
        senf::console::root().mkdir("dir1").mkdir("dir2").doc("help info"));
    senf::console::GenericNode::weak_ptr wp (node.thisptr());

    BOOST_CHECK_EQUAL( node.name(), "dir2" );
    BOOST_CHECK( node.parent() );
    BOOST_CHECK_EQUAL( node.path(), "/dir1/dir2" );
    BOOST_CHECK( node.active() );
    std::stringstream ss;
    node.help(ss);
    BOOST_CHECK_EQUAL( ss.str(), "help info\n" );
    
    {
        senf::console::GenericNode::ptr p (senf::console::root()["dir1"].unlink());
        BOOST_CHECK( ! node.active() );
        BOOST_CHECK( ! wp.expired() );
    }
    BOOST_CHECK( wp.expired() );
}

namespace {
    void callback(std::ostream & os, senf::console::ParseCommandInfo const & command)
    {
        os << "callback";
    }

    template <class T>
    struct select1st {
        typedef T result_type;
        template <class U> result_type operator()(U const & u) const { return u.first; }
    };
}

BOOST_AUTO_UNIT_TEST(directoryNode)
{
    senf::console::DirectoryNode::ptr p (senf::console::DirectoryNode::create());

    BOOST_CHECK( & senf::console::root().add("dir1", p) == p.get() );

    senf::console::SimpleCommandNode & fnnode (
        senf::console::root().add( "fn", senf::console::SimpleCommandNode::create(&callback) ));
    BOOST_CHECK( &senf::console::root()["dir1"] == p.get() );
    BOOST_CHECK_THROW( senf::console::root()["dir2"], senf::console::UnknownNodeNameException );
    BOOST_CHECK_THROW( senf::console::root()("dir1"), std::bad_cast );
    BOOST_CHECK( &senf::console::root()("fn") == &fnnode );
    BOOST_CHECK_THROW( senf::console::root()("fn2"), senf::console::UnknownNodeNameException );
    BOOST_CHECK_THROW( senf::console::root()["fn"], std::bad_cast );
    BOOST_CHECK( &senf::console::root().get("dir1") == p.get() );
    
    senf::console::root().mkdir("dir2").mkdir("dir3");
    char const * const children[] = { "dir1", "dir2", "fn", "sys" };
    BOOST_CHECK_EQUAL_COLLECTIONS( 
        boost::make_transform_iterator(senf::console::root().children().begin(), 
                                       select1st<std::string const &>()),
        boost::make_transform_iterator(senf::console::root().children().end(),
                                       select1st<std::string const &>()),
        children, 
        children+sizeof(children)/sizeof(children[0]) );

    char const * const completions[] = { "dir1", "dir2" };
    BOOST_CHECK_EQUAL_COLLECTIONS(
        boost::make_transform_iterator(senf::console::root().completions("dir").begin(), 
                                       select1st<std::string const &>()),
        boost::make_transform_iterator(senf::console::root().completions("dir").end(),
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

    BOOST_CHECK( senf::console::root().remove("dir1") == p );
    senf::console::root().remove("dir2");
    senf::console::root().remove("fn");

    BOOST_CHECK_EQUAL( std::distance(senf::console::root().children().begin(),
                                     senf::console::root().children().end()), 1 );
}

BOOST_AUTO_UNIT_TEST(linkNode)
{
    senf::console::root().mkdir("dir1");
    senf::console::root().link("link1", senf::console::root()["dir1"]);

    BOOST_CHECK( senf::console::root()["dir1"] == senf::console::root()["link1"] );

    senf::console::root().remove("dir1");
    senf::console::root().remove("link1");
}

namespace {
    struct Functor {
        void operator()(std::ostream & os, senf::console::ParseCommandInfo const &) {
            os << "functor";
        }
    };
}

BOOST_AUTO_UNIT_TEST(senfConsoleAddNode)
{
    senf::console::root().add("fn1", &callback);
    senf::console::root().add("fn2", Functor());
    
    senf::console::ParseCommandInfo info;

    {
        std::stringstream ss;
        senf::console::root()("fn1")(ss, info);
        BOOST_CHECK_EQUAL( ss.str(), "callback" );
    }

    {
        std::stringstream ss;
        senf::console::root()("fn2")(ss, info);
        BOOST_CHECK_EQUAL( ss.str(), "functor" );
    }
    
    senf::console::root().remove("fn1");
    senf::console::root().remove("fn2");
}

BOOST_AUTO_UNIT_TEST(simpleCommandNode)
{
    senf::console::root().add("fn", senf::console::SimpleCommandNode::create(&callback))
        .doc("help text");
    {
        std::stringstream ss;
        senf::console::ParseCommandInfo info;
        senf::console::root()("fn")(ss, info);
        BOOST_CHECK_EQUAL( ss.str(), "callback" );
    }
    
    {
        std::stringstream ss;
        senf::console::root().get("fn").help(ss);
        BOOST_CHECK_EQUAL( ss.str(), "help text\n" );
    }

    senf::console::root().remove("fn");
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
