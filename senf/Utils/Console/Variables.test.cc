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
    \brief Variables unit tests */

//#include "Variables.test.hh"
//#include "Variables.test.ih"

// Custom includes
#include <sstream>
#include "Console.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    void testParser(senf::console::ParseCommandInfo::TokensRange const &, int & value)
    { value = 0; }

    void testFormatter(int value, std::ostream & os)
    { os << '[' << value << ']'; }

    static bool changed_ (false);
    void testCallback(int oldValue)
    { changed_ = true; }
}

SENF_AUTO_UNIT_TEST(variables)
{
    namespace fty = senf::console::factory;

    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);

    int var (5);

    std::stringstream ss;
    dir.add("var", fty::Variable(var)
        .doc("Current blorg limit")
        .formatter(&testFormatter)
        .parser(&testParser)
        .typeName("number")
        .onChange(&testCallback)
        );
    parser.parse("test/var; test/var 10; test/var",
                 boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
    BOOST_CHECK_EQUAL( ss.str(), "[5]\n[0]\n" );
    BOOST_CHECK( changed_ );

    ss.str("");
    dir("var").help(ss);
    BOOST_CHECK_EQUAL(ss.str(),
                      "Usage:\n"
                      "    1- var\n"
                      "    2- var new_value:number\n"
                      "\n"
                      "Current blorg limit\n");

    senf::console::CommandNode & refvar (dir.add("refvar", fty::Variable(boost::ref(var))
        .doc("Current blorg limit")
        .formatter(&testFormatter)
        .parser(&testParser)
        .typeName("number")
    ));

    senf::IGNORE( refvar );

    dir.add("crefvar", fty::Variable(boost::cref(var))
        .doc("Current blorg limit")
        .formatter(&testFormatter)
        );
}

namespace {

    class Test2
    {
    public:
        senf::console::ScopedDirectory<Test2> dir;

        Test2() : dir(this), var_(0)
            { dir.add("var", senf::console::factory::Variable(boost::ref(var_))); }

    private:
        int var_;
    };

}

SENF_AUTO_UNIT_TEST(memberVariables)
{
    Test2 test2ob;
    BOOST_CHECK( true );
}

#ifdef COMPILE_CHECK

COMPILE_FAIL(constVariable)
{
    senf::console::ScopedDirectory<> dir;
    int var;
    dir.add("var", boost::cref(var))
        .parser(&testParser);
}

#endif

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
