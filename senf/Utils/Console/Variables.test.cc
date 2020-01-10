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
    \brief Variables unit tests */

// Custom includes
#include <sstream>
#include "Console.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(variables)
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

SENF_AUTO_TEST_CASE(memberVariables)
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
