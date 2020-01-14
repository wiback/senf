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
    \brief ConfigFile unit tests */

// Custom includes
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include "Console.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    int var1 (0);
    bool var2 (false);

    void fun1(int v) { var1 = v; }
    void fun2() { var2 = true; }

    class TempFile
    {
    public:
        TempFile(std::string const & name) : name_ (name), file_ (name_.c_str()) {}
        ~TempFile() { file_.close(); boost::filesystem::remove(name_); }

        template <class T> TempFile & operator<<(T const & v) { file_ << v; return *this; }
        enum Closer { close }; void operator<<(Closer) { file_.close(); }
        std::string const & name() { return name_; }

    private:
        std::string name_;
        std::ofstream file_;
    };

}

#define SENF_CHECK_THROW_SYSTEMEXCEPTION( expr, errorNumber, msg)   \
    try {                                                           \
        BOOST_TEST_PASSPOINT();                                     \
        expr;                                                       \
        BOOST_ERROR( "senf::SystemException is expected");          \
    } catch( senf::SystemException const & ex ) {                   \
        BOOST_CHECK( ex.anyOf( errorNumber));                       \
        BOOST_CHECK( ex.message().find(msg) != std::string::npos);  \
    }                                                               \


SENF_AUTO_TEST_CASE(configFile)
{
    namespace fty = senf::console::factory;

    TempFile cfgf ("test.cfg");
    cfgf << "dir1/fun1 10;\n"
         << TempFile::close;

    senf::console::ScopedDirectory<> dir1;
    senf::console::root().add("dir1", dir1);
    dir1.add("fun1",fty::Command(&fun1));

    {
        senf::console::ConfigFile cfg (cfgf.name());

        var1 = 0;
        SENF_CHECK_NO_THROW( cfg.parse() );
        BOOST_CHECK_EQUAL( var1, 10 );

        var1 = 0;
        SENF_CHECK_NO_THROW( cfg.parse() );
        BOOST_CHECK_EQUAL( var1, 0 );

        var1 = 0;
        cfg.reset();
        SENF_CHECK_NO_THROW( cfg.parse() );
        BOOST_CHECK_EQUAL( var1, 10 );
    }

    {
        senf::console::ConfigFile cfg ("i don't exist");
        SENF_CHECK_THROW_SYSTEMEXCEPTION(cfg.parse(), ENOENT, "i don't exist");
        cfg.ignoreMissing();
        SENF_CHECK_NO_THROW( cfg.parse() );
    }
    {
        std::string etc_shaddow ("/etc/shadow");
        if (getuid() != 0 && boost::filesystem::exists(etc_shaddow)) {
            senf::console::ConfigFile cfg (etc_shaddow);
            SENF_CHECK_THROW_SYSTEMEXCEPTION( cfg.parse(), EACCES, etc_shaddow);
        }
    }
}

SENF_AUTO_TEST_CASE(configFileRestrict)
{
    namespace fty = senf::console::factory;

    TempFile cfgf ("test.cfg");
    cfgf << "dir1/fun1 10;\n"
         << "dir2/fun2;\n"
         << TempFile::close;

    senf::console::ScopedDirectory<> dir1;
    senf::console::root().add("dir1", dir1);
    dir1.add("fun1",fty::Command(&fun1));

    {
        var1 = 0;
        var2 = false;
        senf::console::ConfigFile cfg (cfgf.name());
        SENF_CHECK_NO_THROW( cfg.parse(dir1) );
        BOOST_CHECK_EQUAL( var1, 10 );
        BOOST_CHECK_EQUAL( var2, false );
        BOOST_CHECK( cfg.parsed(dir1) );
        BOOST_CHECK( ! cfg.complete() );

        senf::console::ScopedDirectory<> dir2;
        senf::console::root().add("dir2", dir2);
        dir2.add("fun2",fty::Command(&fun2));

        var1 = 0;
        var2 = false;
        SENF_CHECK_NO_THROW( cfg.parse() );
        BOOST_CHECK_EQUAL( var1, 0 );
        BOOST_CHECK_EQUAL( var2, true );
        BOOST_CHECK( cfg.complete() );
    }
}

SENF_AUTO_TEST_CASE(configFileSkipGroup)
{
    namespace fty = senf::console::factory;

    TempFile cfgf ("test.cfg");
    cfgf << "dir1/fun1 10;\n"
         << "dir2 { dir3 { fun2; } fun1 5; }"
         << TempFile::close;

    senf::console::ScopedDirectory<> dir1;
    senf::console::root().add("dir1", dir1);
    dir1.add("fun1",fty::Command(&fun1));

    senf::console::ScopedDirectory<> dir2;
    senf::console::root().add("dir2", dir2);

    dir2.add("dir3",fty::Directory()).add("fun2", fty::Command(&fun2));
    dir2.add("fun1", fty::Command(&fun1));

    {
        var1 = 0;
        var2 = false;
        senf::console::ConfigFile cfg (cfgf.name());
        SENF_CHECK_NO_THROW( cfg.parse(dir1) );
        BOOST_CHECK_EQUAL( var1, 10 );
        BOOST_CHECK_EQUAL( var2, false );
        BOOST_CHECK( cfg.parsed(dir1) );

        var1 = 0;
        var2 = false;
        SENF_CHECK_NO_THROW( cfg.parse(dir2["dir3"]) );
        BOOST_CHECK_EQUAL( var1, 0 );
        BOOST_CHECK_EQUAL( var2, true );
        BOOST_CHECK( ! cfg.parsed(dir2) );

        var1 = 0;
        var2 = false;
        SENF_CHECK_NO_THROW( cfg.parse() );
        BOOST_CHECK_EQUAL( var1, 5 );
        BOOST_CHECK_EQUAL( var2, false );
    }
}

SENF_AUTO_TEST_CASE(configRestrictAndLink)
{
    namespace fty = senf::console::factory;

    TempFile cfgf ("test.cfg");
    cfgf << "dir1/fun1 10;\n"
         << "link1 { dir3 { fun2; } fun1 5; }"
         << TempFile::close;

    senf::console::ScopedDirectory<> dir1;
    senf::console::root().add("dir1", dir1);
    dir1.add("fun1",fty::Command(&fun1));

    senf::console::ScopedDirectory<> dir2;
    dir1.add("dir2", dir2);

    dir2.add("dir3",fty::Directory()).add("fun2", fty::Command(&fun2));
    dir2.add("fun1", fty::Command(&fun1));

    senf::console::ScopedDirectory<> dir4;
    senf::console::root().add("dir4", dir4);
    dir4.add("link1", fty::Link(dir2));

    {
        senf::console::ConfigFile cfg (cfgf.name(), dir4);

        var1 = 0;
        var2 = false;
        SENF_CHECK_NO_THROW( cfg.parse(dir2["dir3"]) );
        BOOST_CHECK_EQUAL( var1, 0 );
        BOOST_CHECK_EQUAL( var2, true );

        BOOST_CHECK_THROW( cfg.parse(), senf::console::SyntaxErrorException );
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
