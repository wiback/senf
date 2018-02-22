//
// Copyright (C) 2014
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
//

// Custom includes
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
