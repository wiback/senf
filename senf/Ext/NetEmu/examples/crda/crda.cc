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


#include <boost/filesystem.hpp>
#include <senf/Ext/NetEmu/WLAN/CRDA.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char const * argv[])
{
//    if (boost::filesystem::path(argv[0]).filename() == senf::emu::CRDA::instance().slaveName()) {
        senf::emu::CRDA::instance().init();
        return senf::emu::CRDA::instance().run(argc, argv);
//    }
//
//    // true ==> MasterMode
//    if (!senf::emu::CRDA::instance().init(true)) {
//        std::cerr << "Failed to init CRDA ?!?" << std::endl;
//        return 1;
//    }
//
//    return senf::emu::CRDA::instance().run(argc, argv);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
