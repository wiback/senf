//
// Copyright (C) 2014
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
//

// Custom includes
#include <senf/Ext/NetEmu/WLAN/CRDA.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char const * argv[])
{
    return senf::emu::CRDA::instance().run(argc, argv);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
