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
    \brief Config unit tests */

// Custom includes
#include <boost/filesystem/operations.hpp>
#include "Console.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    std::string val1;
    bool val2 (false);

    void fun1(std::string p)
    { val1 = p; }

    void fun2()
    { val2 = true; }

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

SENF_AUTO_TEST_CASE(configBundle_empty)
{
    TempFile cfg ("test.cfg");
    cfg << TempFile::close;

    senf::console::ScopedDirectory<> root;
    root.add("fun2", senf::console::factory::Command(&fun2));

    senf::console::ConfigBundle bundle(root);
    bundle.add( senf::console::FileConfig(cfg.name()));

    SENF_CHECK_NO_THROW( bundle.parse() );
}

SENF_AUTO_TEST_CASE(configBundle)
{
    namespace fty = senf::console::factory;

    senf::console::ScopedDirectory<> root;
    senf::console::root().add("root", root);

    senf::console::ScopedDirectory<> chroot;
    senf::console::root().add("chroot", chroot);

    root.add("dir1", fty::Directory()).add("fun1", fty::Command(&fun1));
    root.add("fun2", fty::Command(&fun2));
    chroot.add("dir1", fty::Directory()).add("fun1", fty::Command(&fun1));
    chroot.add("fun2", fty::Command(&fun2));

    TempFile cfg ("test.cfg");
    cfg << "dir1/fun1 foo; fun2;" << TempFile::close;

    char const * argv[] = { "", "--dir1-fun1=bar" };

    senf::console::ConfigBundle bundle(root);
    bundle.add( senf::console::FileConfig(cfg.name()) );
    bundle.add( senf::console::OptionsConfig(sizeof(argv)/sizeof(argv[0]), argv) );

    SENF_CHECK_NO_THROW( bundle.parse() );
    BOOST_CHECK_EQUAL( val1, "bar" );
    BOOST_CHECK_EQUAL( val2, true );

    bundle.chroot( chroot);
    SENF_CHECK_NO_THROW( bundle.parse() );
    BOOST_CHECK_EQUAL( val1, "bar" );
    BOOST_CHECK_EQUAL( val2, true );
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
