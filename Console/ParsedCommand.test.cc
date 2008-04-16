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
    \brief ParsedCommand.test unit tests */

//#include "ParsedCommand.test.hh"
//#include "ParsedCommand.test.ih"

// Custom includes
#include <iostream>
#include <sstream>
#include "ParsedCommand.hh"
#include "Executor.hh"
#include "Parse.hh"
#include "ScopedDirectory.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    int cb1(int a, double b) { return int(a+b); }
    double cb2(){ return 1.2; }
    void cb3(int i) { }
    std::string cb4(std::ostream & os) { os << "text\n"; return "value"; }
    void cb5(std::ostream & os, std::string const & v) { os << "Value: " << v << "\n"; }

    struct TestParser 
    {
        void operator()(senf::console::ParseCommandInfo::TokensRange const &,
                        std::string & out) const
            { out = "true"; }
    };

    void testFormatter(double, std::ostream & os)
    { os << "formatter"; }
}

BOOST_AUTO_UNIT_TEST(parsedCommand)
{
    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);

    {
        std::stringstream ss;
        dir.add("cb1", &cb1);
        parser.parse("test/cb1 2 3.2", 
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
        BOOST_CHECK_EQUAL( ss.str(), "5\n" );
    }

    {
        std::stringstream ss;
        dir.add("cb2", &cb2);
        parser.parse("test/cb2",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
        BOOST_CHECK_EQUAL( ss.str(), "1.2\n" );
    }

    {
        std::stringstream ss;
        dir.add("cb3", &cb3);
        parser.parse("test/cb3 234",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
        BOOST_CHECK_EQUAL( ss.str(), "" );
    }

    {
        std::stringstream ss;
        dir.add("cb4", &cb4);
        parser.parse("test/cb4",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
        BOOST_CHECK_EQUAL( ss.str(), "text\n" "value\n" );
    }

    {
        std::stringstream ss;
        dir.add("cb5", &cb5);
        parser.parse("test/cb5 1234",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
        BOOST_CHECK_EQUAL( ss.str(), "Value: 1234\n" );
    }

    {
        std::stringstream ss;

        BOOST_CHECK_THROW( 
            parser.parse("test/cb1 2 3.2 foo", 
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )),
            senf::console::SyntaxErrorException );

        BOOST_CHECK_THROW(
            parser.parse("test/cb1 2", 
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )),
            senf::console::SyntaxErrorException );

        BOOST_CHECK_THROW(
            parser.parse("test/cb1 2 foo", 
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )),
            senf::console::SyntaxErrorException );
    }

    {
        namespace kw = senf::console::kw;
        std::stringstream ss;

        // Just for the fun of it, use a functor and not a function pointer as parser ...
        dir.add("cb6", &cb5)
            .arg( kw::parser = TestParser() );
        BOOST_CHECK_NO_THROW(
            parser.parse("test/cb6 false",
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
        BOOST_CHECK_EQUAL( ss.str(), "Value: true\n" );
    }

    {
        std::stringstream ss;

        // This tests adding boost::function objects and at the same time validates, that 
        // compatible types also work
        dir.add("cb7", boost::function<float()>(&cb2))
            .formatter( &testFormatter );
        BOOST_CHECK_NO_THROW(
            parser.parse("test/cb7",
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
        BOOST_CHECK_EQUAL( ss.str(), "formatter\n" );
    }

    {
        std::stringstream ss;

        using namespace senf::console::kw;

        dir.add("cb", &cb1)
            .doc(
                "Ops fortunate, ops me ut orgia vociferatio contumax per, rudo re loco emitto\n"
                "intolerabiliter ita iugo. Subcribo gravo. Devenio luna fonticulus Castanea\n"
                "horum fascino Os interpretor non ipse conjuratio hora, qui filius denuntio ait\n"
                "sono te odium Anhelo. Dum Cedo audax celox alius una Agnosco hic, ibi retineo\n"
                "lux sto ioco. Per Re dono. Copiose reus scitus jus diligens sis scapulare\n"
                "Servitium transi." )

            .overloadDoc(
                "Lo nam balnearius Opprimo Pennatus, no decentia sui, dicto esse se pulchritudo,\n"
                "pupa Sive res indifferenter. Captivo pa." )

            .arg( description   = "Bar didelfrump di desgorb. Nu widsoflar brimeldrgf." )

            .arg( name          = "checkup", 
                  type_name     = "number",
                  description   = "Florgel, dargel and durgel",
                  default_value = 2.1,
                  default_doc   = "(double) 2.1" );

        senf::console::OverloadedCommandNode & cbNode (
            dir.add("cb", &cb5)
                .overloadDoc(
                    "Uus Primordia fundo falsidicus corium, diurnitas humo pro leto. Sui Ueraciter\n"
                    "hio eruca lenis qua Agalmate ut fors penitentia. Iugum obdormio anxio nuncupo\n"
                    "iam, in vos nam Custodi." ) 
                .arg( "text", default_value = "" ) );

        (void) cbNode;

        BOOST_CHECK_NO_THROW(
            parser.parse("test/cb 111 222.4",
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
        BOOST_CHECK_NO_THROW(
            parser.parse("test/cb 222",
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
        BOOST_CHECK_NO_THROW(
            parser.parse("test/cb foo",
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
        BOOST_CHECK_NO_THROW(
            parser.parse("test/cb",
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );

        BOOST_CHECK_EQUAL( ss.str(), "333\n" "224\n" "Value: foo\n" "Value: \n" );
    }

    {
        std::stringstream ss;
        senf::console::root()["test"]("cb").help(ss);
        BOOST_CHECK_EQUAL( 
            ss.str(), 
            "Usage:\n"
            "    1- cb arg11:int [checkup:number]\n"
            "    2- cb [text:string]\n"
            "\n"
            "With:\n"
            "    arg11     Bar didelfrump di desgorb. Nu widsoflar brimeldrgf.\n"
            "    checkup   Florgel, dargel and durgel\n"
            "        default: (double) 2.1\n"
            "    text      \n"
            "        default: (empty)\n"
            "\n"
            "Ops fortunate, ops me ut orgia vociferatio contumax per, rudo re loco emitto\n"
            "intolerabiliter ita iugo. Subcribo gravo. Devenio luna fonticulus Castanea\n"
            "horum fascino Os interpretor non ipse conjuratio hora, qui filius denuntio ait\n"
            "sono te odium Anhelo. Dum Cedo audax celox alius una Agnosco hic, ibi retineo\n"
            "lux sto ioco. Per Re dono. Copiose reus scitus jus diligens sis scapulare\n"
            "Servitium transi.\n"
            "\n"
            "Variant 1:\n"
            "Lo nam balnearius Opprimo Pennatus, no decentia sui, dicto esse se pulchritudo,\n"
            "pupa Sive res indifferenter. Captivo pa.\n"
            "\n"
            "Variant 2:\n"
            "Uus Primordia fundo falsidicus corium, diurnitas humo pro leto. Sui Ueraciter\n"
            "hio eruca lenis qua Agalmate ut fors penitentia. Iugum obdormio anxio nuncupo\n"
            "iam, in vos nam Custodi.\n" );
    }
}

namespace {

    struct Test 
    {
        senf::console::ScopedDirectory<Test> dir;
        std::string name_;

        Test(std::string const & name) : dir(this), name_ (name) {
            dir.add("name", &Test::name);
        }

        std::string name(std::string const & suffix) {
            return name_ + suffix;
        }
    };

}

BOOST_AUTO_UNIT_TEST(memberParsedCommand)
{
    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);
    
    {
        Test obj ("bar");
        dir.add("obj", obj.dir);
        
        std::stringstream ss;
        parser.parse("test/obj/name \": foo\"",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
        BOOST_CHECK_EQUAL( ss.str(), "bar: foo\n" );
    }
}

#ifdef COMPILE_CHECK

COMPILE_FAIL(argParser)
{
    senf::console::ScopedDirectory<> dir;

    // Fails, since there are only two arguments defined
    dir.add("cb", &cb1)
        .arg()
        .arg()
        .arg();
}

COMPILE_FAIL(defaultDoc)
{
    senf::console::ScopedDirectory<> dir;
    using namespace senf::console::kw;

    // Fails, since default_value is missing but default_doc is given
    dir.add("cb",&cb1)
        .arg(default_doc = "doc");
}

#endif
    
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
