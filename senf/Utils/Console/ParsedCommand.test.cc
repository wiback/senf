//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief ParsedCommand unit tests */

// Custom includes
#include <sstream>
#include "Console.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    int cb1(int a, double b) { return int(a+b); }
    double cb2() { return 1.2; }
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

SENF_AUTO_TEST_CASE(parsedCommand)
{
    namespace fty = senf::console::factory;

    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);

    {
        std::stringstream ss;
        dir.add("cb1", fty::Command(&cb1));
        parser.parse("test/cb1 2 3.2",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
        BOOST_CHECK_EQUAL( ss.str(), "5\n" );
    }

    {
        std::stringstream ss;
        dir.add("cb2", fty::Command(&cb2));
        parser.parse("test/cb2",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
        BOOST_CHECK_EQUAL( ss.str(), "1.2\n" );
    }

    {
        std::stringstream ss;
        dir.add("cb3", fty::Command(&cb3));
        parser.parse("test/cb3 234",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
        BOOST_CHECK_EQUAL( ss.str(), "" );
    }

    {
        std::stringstream ss;
        dir.add("cb4", fty::Command(&cb4));
        parser.parse("test/cb4",
                     boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
        BOOST_CHECK_EQUAL( ss.str(), "text\n" "value\n" );
    }

    {
        std::stringstream ss;
        dir.add("cb5", fty::Command(&cb5));
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
        dir.add("cb6",
                fty::Command(&cb5)
                    .arg( kw::parser = TestParser() )
            );
        SENF_CHECK_NO_THROW(
            parser.parse("test/cb6 false",
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
        BOOST_CHECK_EQUAL( ss.str(), "Value: true\n" );
    }

    {
        std::stringstream ss;

        // This tests adding boost::function objects and at the same time validates, that
        // compatible types also work
        dir.add("cb7",
                fty::Command(boost::function<float()>(&cb2))
                    .formatter( &testFormatter )
            );
        SENF_CHECK_NO_THROW(
            parser.parse("test/cb7",
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
        BOOST_CHECK_EQUAL( ss.str(), "formatter\n" );
    }

    {
        std::stringstream ss;

        using namespace senf::console::kw;

        dir.add("cb", fty::Command(&cb1)
            .doc(
                "Ops fortunate, ops me ut orgia vociferatio contumax per, rudo re loco emitto\n"
                "intolerabiliter ita iugo. Subcribo gravo. Devenio luna fonticulus Castanea\n"
                "horum fascino Os interpretor non ipse conjuratio hora, qui filius denuntio ait\n"
                "sono te odium Anhelo. Dum Cedo audax celox alius una Agnosco hic, ibi retineo\n"
                "lux sto ioco. Per Re dono. Copiose reus scitus jus diligens sis scapulare\n"
                "Servitium transi." )

            .shortdoc(
                "Ops fortunate." )

            .overloadDoc(
                "Lo nam balnearius Opprimo Pennatus, no decentia sui, dicto esse se pulchritudo,\n"
                "pupa Sive res indifferenter. Captivo pa." )

            .arg( description   = "Bar didelfrump di desgorb. Nu widsoflar brimeldrgf." )

            .arg( name          = "checkup",
                  type_name     = "number",
                  description   = "Florgel, dargel and durgel",
                  default_value = 2.1,
                  default_doc   = "(double) 2.1" )
            );

        senf::console::OverloadedCommandNode & cbNode (
            dir.add("cb", fty::Command(&cb5)
                .overloadDoc(
                    "Uus Primordia fundo falsidicus corium, diurnitas humo pro leto. Sui Ueraciter\n"
                    "hio eruca lenis qua Agalmate ut fors penitentia. Iugum obdormio anxio nuncupo\n"
                    "iam, in vos nam Custodi." )
                .arg( "text", default_value = "" ) )
            );

        senf::IGNORE( cbNode );

        SENF_CHECK_NO_THROW(
            parser.parse("test/cb 111 222.4",
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
        SENF_CHECK_NO_THROW(
            parser.parse("test/cb 222",
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
        SENF_CHECK_NO_THROW(
            parser.parse("test/cb foo",
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
        SENF_CHECK_NO_THROW(
            parser.parse("test/cb",
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );

        BOOST_CHECK_EQUAL( ss.str(), "333\n" "224\n" "Value: foo\n" "Value: \n" );

        BOOST_CHECK_EQUAL( dir("cb").shorthelp(), "Ops fortunate." );
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
            "        default: \"\"\n"
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
            dir.add("name", senf::console::factory::Command(
                        SENF_MEMBINDFNP(std::string, Test, name, (std::string const &))));
        }

        std::string name(std::string const & suffix) {
            return name_ + suffix;
        }
    };

}

SENF_AUTO_TEST_CASE(memberParsedCommand)
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

namespace {

    senf::console::DirectoryNode::ptr dircb()
    {
        namespace fty = senf::console::factory;

        senf::console::DirectoryNode & dir (
            senf::console::root()["test"].add("dircb", fty::Directory()));
        dir.add("cb1", fty::Command(&cb1));
        return dir.thisptr();
    }

}

SENF_AUTO_TEST_CASE(directoryReturn)
{
    namespace fty = senf::console::factory;

    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);
    dir.add("test", fty::Command(&dircb));

    {
        std::stringstream ss;
        SENF_CHECK_NO_THROW(
            parser.parse("test/test { ll; }",
                         boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 )) );
        BOOST_CHECK_EQUAL( ss.str(),
                           "<Directory at '/test/dircb'>\n"
                           "cb1                         \n" );
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
