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
    \brief Config unit tests */

//#include "Config.test.hh"
//#include "Config.test.ih"

// Custom includes
#include "Console.hh"
#include <boost/filesystem/operations.hpp>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

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

SENF_AUTO_UNIT_TEST(configBundle_empty)
{
    TempFile cfg ("test.cfg");
    cfg << TempFile::close;

    senf::console::ScopedDirectory<> root;
    root.add("fun2", senf::console::factory::Command(&fun2));

    senf::console::ConfigBundle bundle(root);
    bundle.add( senf::console::FileConfig(cfg.name()));

    SENF_CHECK_NO_THROW( bundle.parse() );
}

SENF_AUTO_UNIT_TEST(configBundle)
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
