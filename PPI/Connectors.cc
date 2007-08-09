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
    \brief Connectors non-inline non-template implementation */

#include "Connectors.hh"
//#include "Connectors.ih"

// Custom includes

//#include "Connectors.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// protected members

prefix_ senf::ppi::connector::Connector::~Connector()
{}

prefix_ void senf::ppi::connector::Connector::connect(Connector & target)
{
    peer_ = & target;
    target.peer_ = this;
}

///////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::InputConnector

////////////////////////////////////////
// private members

prefix_ void senf::ppi::connector::InputConnector::v_requestEvent()
{}

prefix_ void senf::ppi::connector::InputConnector::v_enqueueEvent()
{}

prefix_ void senf::ppi::connector::InputConnector::v_dequeueEvent()
{}

///////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::ActiveInput

////////////////////////////////////////
// private members

prefix_ void senf::ppi::connector::ActiveInput::v_requestEvent()
{
    request();
}

///////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::PassiveInput

////////////////////////////////////////
// private members 

prefix_ void senf::ppi::connector::PassiveInput::v_enqueueEvent()
{
    ///\fixme Emit notifications when qstate_ changes
    if (qdisc_)
        qstate_ = qdisc_->update(*this, QueueingDiscipline::ENQUEUE);
    else
        qstate_ = empty()?QueueingDiscipline::UNTHROTTLED:QueueingDiscipline::THROTTLED;
    emit();
}

prefix_ void senf::ppi::connector::PassiveInput::v_dequeueEvent()
{
    ///\fixme Emit notifications when qstate_ changes
    if (qdisc_)
        qstate_ = qdisc_->update(*this, QueueingDiscipline::DEQUEUE);
    else
        qstate_ = empty()?QueueingDiscipline::UNTHROTTLED:QueueingDiscipline::THROTTLED;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Connectors.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
