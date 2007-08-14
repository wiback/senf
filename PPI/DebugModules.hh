// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief DebugModules public header */

#ifndef HH_DebugModules_
#define HH_DebugModules_ 1

// Custom includes
#include <deque>
#include "Packets/Packets.hh"
#include "Module.hh"

//#include "DebugModules.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {
namespace debug {
    
    class ActivePacketSource
        : public Module
    {
    public:
        connector::ActiveOutput output;

        ActivePacketSource();

        void submit(Packet packet);
    };

    class PassivePacketSource
        : public Module
    {
        typedef std::deque<Packet> Queue;

    public:
        typedef Queue::size_type size_type;
        
        PassivePacketSource();
        
        connector::PassiveOutput output;
        
        void submit(Packet packet);

        bool empty();
        size_type size();

    private:
        void request();
        void init();
        
        Queue packets_;
    };

    class ActivePacketSink
        : public Module
    {
    public:
        connector::ActiveInput input;

        ActivePacketSink();

        Packet request();
    };

    class PassivePacketSink
        : public Module
    {
        typedef std::deque<Packet> Queue;

    public:
        typedef Queue::size_type size_type;
        typedef Queue::const_iterator iterator;

        connector::PassiveInput input;
        
        PassivePacketSink();

        bool empty();
        size_type size();
        iterator begin();
        iterator end();

        Packet front();
        Packet pop_front();

        void clear();

    private:
        void request();
        
        Queue packets_;
    };

}}}}

///////////////////////////////hh.e////////////////////////////////////////
#include "DebugModules.cci"
//#include "DebugModules.ct"
//#include "DebugModules.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
