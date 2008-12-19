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
    \brief Telnet non-inline non-template implementation */

#include "Telnet.hh"
//#include "Telnet.ih"

// Custom includes
#include "../membind.hh"
#include "../Logger/SenfLog.hh"

//#include "Telnet.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::console::detail::BaseTelnetProtocol::BaseTelnetProtocol(Handle handle)
    : handle_ (handle), charState_ (NORMAL), command_ (CMD_NONE), option_ (0),
      inputEvent_ ("senf::console::detail::BaseTelnetProtocol::input",
                   senf::membind(&BaseTelnetProtocol::readHandler, this), handle, 
                   senf::scheduler::FdEvent::EV_READ),
      outputEvent_ ("senf::console::detail::BaseTelnetProtocol::output",
                    senf::membind(&BaseTelnetProtocol::writeHandler, this), handle,
                    senf::scheduler::FdEvent::EV_WRITE, false)
{}

prefix_ senf::console::detail::BaseTelnetProtocol::BaseTelnetProtocol()
    : handle_ (), charState_ (NORMAL), command_ (CMD_NONE), option_ (0),
      inputEvent_ ("senf::console::detail::BaseTelnetProtocol::input", 0),
      outputEvent_ ("senf::console::detail::BaseTelnetProtocol::output", 0)
{}

prefix_ void senf::console::detail::BaseTelnetProtocol::write(std::string const & s)
{
    for (std::string::const_iterator i (s.begin()); i != s.end(); ++i)
        write(*i);
}

prefix_ void senf::console::detail::BaseTelnetProtocol::write(char c)
{
    switch (c) {
    case '\r':
        transmit('\r');
        transmit('\0');
        break;
    case '\n':
        transmit('\r');
        transmit('\n');
        break;
    case '\xff':
        transmit('\xff');
        transmit('\xff');
        break;
    default:
        transmit(c);
        break;
    }
}

prefix_ void
senf::console::detail::BaseTelnetProtocol::sendOptionParameters(option_type option,
                                                                std::string const & data)
{
    transmit(CMD_IAC);
    transmit(CMD_SB);
    transmit(option);
    for (std::string::const_iterator i (data.begin()); i != data.end(); ++i)
        if (*i == '\xff') {
            transmit('\xff');
            transmit('\xff');
        }
        else
            transmit(*i);
    transmit(CMD_IAC);
    transmit(CMD_SE);
}

prefix_ void senf::console::detail::BaseTelnetProtocol::v_handleNOP()
{}

prefix_ void senf::console::detail::BaseTelnetProtocol::v_handleBRK()
{}

prefix_ void senf::console::detail::BaseTelnetProtocol::v_handleIP()
{}

prefix_ void senf::console::detail::BaseTelnetProtocol::v_handleAO()
{}

prefix_ void senf::console::detail::BaseTelnetProtocol::v_handleAYT()
{}

prefix_ void senf::console::detail::BaseTelnetProtocol::v_handleEC()
{}

prefix_ void senf::console::detail::BaseTelnetProtocol::v_handleEL()
{}

prefix_ void senf::console::detail::BaseTelnetProtocol::v_handleGA()
{}

prefix_ void senf::console::detail::BaseTelnetProtocol::handleChar(char c)
{
    switch (charState_) {
    case NORMAL:
        handleNormalChar(c);
        break;
    case IAC_SEEN:
        handleCommand(static_cast<unsigned char>(c));
        break;
    case EXPECT_OPTION:
        handleOption(c);
        break;
    case CR_SEEN:
        handleCR(c);
        break;
    case SB_OPTION:
        handleSBOption(c);
        break;
    case SB_DATA:
        handleSBData(c);
        break;
    case SB_IAC_SEEN:
        handleSBIAC(c);
        break;
    }
}

prefix_ void senf::console::detail::BaseTelnetProtocol::handleNormalChar(char c)
{
    switch (c) {
    case '\r':
        charState_ = CR_SEEN;
        break;
    case '\xff':
        charState_ = IAC_SEEN;
        break;
    default:
        emit(c);
        break;
    }
}

prefix_ void senf::console::detail::BaseTelnetProtocol::handleCommand(char c)
{
    switch (c) {
    case CMD_SE:
        // Ignore spurious SE commands .. they should only occur while in subnegotiation mode
        charState_ = NORMAL;
        break;
    case CMD_NOP:
    case CMD_DM:
    case CMD_BRK:
    case CMD_IP:
    case CMD_AO:
    case CMD_AYT:
    case CMD_EC:
    case CMD_EL:
    case CMD_GA:
        command_ = Command(static_cast<unsigned char>(c));
        processCommand();
        charState_ = NORMAL;
        break;
    case CMD_SB:
        command_ = CMD_SB;
        charState_ = SB_OPTION;
        break;
    case CMD_WILL:
    case CMD_WONT:
    case CMD_DO:
    case CMD_DONT:
        command_ = Command(static_cast<unsigned char>(c));
        charState_ = EXPECT_OPTION;
        break;
    case CMD_IAC:
        charState_ = NORMAL;
        emit(CMD_IAC);
        break;
    default:
        emit(CMD_IAC);
        charState_ = NORMAL;
        handleChar(c);
        break;
    }
}        

prefix_ void senf::console::detail::BaseTelnetProtocol::handleOption(char c)
{
    option_ = c;
    processCommand();
    charState_ = NORMAL;
}

prefix_ void senf::console::detail::BaseTelnetProtocol::handleCR(char c)
{
    switch (c) {
    case '\0':
        emit('\r');
        charState_ = NORMAL;
        break;
    case '\n':
        emit('\n');
        charState_ = NORMAL;
        break;
    default:
        emit('\r');
        charState_ = NORMAL;
        handleChar(c);
        break;
    }
}

prefix_ void senf::console::detail::BaseTelnetProtocol::handleSBOption(char c)
{
    option_ = c;
    charState_ = SB_DATA;
    data_.clear();
}

prefix_ void senf::console::detail::BaseTelnetProtocol::handleSBData(char c)
{
    if (c == '\xff')
        charState_ = SB_IAC_SEEN;
    else
        data_.push_back(c);
}

prefix_ void senf::console::detail::BaseTelnetProtocol::handleSBIAC(char c)
{
    switch (c) {
    case CMD_IAC:
        data_.push_back(c);
        charState_ = SB_DATA;
        break;
    case CMD_SE:
        processCommand();
        charState_ = NORMAL;
        break;
    default:
        charState_ = IAC_SEEN;
        handleChar(c);
        break;
    }
}

prefix_ void senf::console::detail::BaseTelnetProtocol::processCommand()
{
    switch (command_) {
    case CMD_NONE:
    case CMD_SE:
    case CMD_DM:
    case CMD_IAC:
        break;
    case CMD_NOP:
        v_handleNOP();
        break;
    case CMD_BRK:
        v_handleBRK();
        break;
    case CMD_IP:
        v_handleIP();
        break;
    case CMD_AO:
        v_handleAO();
        break;
    case CMD_AYT:
        v_handleAYT();
        break;
    case CMD_EC:
        v_handleEC();
        break;
    case CMD_EL:
        v_handleEL();
        break;
    case CMD_GA:
        v_handleGA();
        break;
    case CMD_SB:
    {
        OptionHandlerMap::const_iterator i (handlers_.find(option_));
        if (i != handlers_.end())
            i->second->v_handleOptionParameters(data_);
        break;
    }
    case CMD_WILL:
    case CMD_WONT:
        response(getOption(false, option_), command_ == CMD_WILL);
        break;
    case CMD_DO:
    case CMD_DONT:
        response(getOption(true, option_),  command_ == CMD_DO);
        break;
    }
}

prefix_ void senf::console::detail::BaseTelnetProtocol::transmit(char c)
{
    sendQueue_.push_back(c);
    outputEvent_.enable();
}

prefix_ void senf::console::detail::BaseTelnetProtocol::readHandler(int state)
{
    if (state != senf::scheduler::FdEvent::EV_READ || handle_.eof()) {
        inputEvent_.disable();
        v_eof();
        return;
    }
    std::string data;
    handle_.read(data, 0u);
    for (std::string::const_iterator i (data.begin()); i != data.end(); ++i)
        handleChar(*i);
}

prefix_ void senf::console::detail::BaseTelnetProtocol::writeHandler(int state)
{ 
    if (state != senf::scheduler::FdEvent::EV_WRITE) {
        outputEvent_.disable();
        inputEvent_.disable();
        v_eof();
        return;
    }
    sendQueue_.erase(sendQueue_.begin(), 
                     handle_.write(std::make_pair(sendQueue_.begin(), sendQueue_.end())));
    if (sendQueue_.empty())
        outputEvent_.disable();
}

prefix_ senf::console::detail::BaseTelnetProtocol::OptInfo &
senf::console::detail::BaseTelnetProtocol::getOption(bool local, option_type option)
{
    OptionsMap::iterator i (options_.find(std::make_pair(local, option)));
    if (i == options_.end())
        i = options_.insert(std::make_pair(std::make_pair(local, option),
                                           OptInfo(local, option))).first;
    return i->second;
}

prefix_ void senf::console::detail::BaseTelnetProtocol::request(OptInfo & info, bool enabled)
{
    info.wantState = enabled ? OptInfo::WANTED : OptInfo::DISABLED;
    if (enabled != info.enabled) {
        transmit(CMD_IAC);
        transmit((info.local ? CMD_WILL : CMD_DO) + (enabled ? 0 : 1));
        transmit(info.option);
        info.optionState = OptInfo::REQUEST_SENT;
    }
}

prefix_ void senf::console::detail::BaseTelnetProtocol::response(OptInfo & info, bool enabled)
{
    // If this is a response, we need to unconditionally accept it.  If this is a remote
    // configuration request, we accept it if wantState is wither WANTED or ACCEPTED.  If this is a
    // response, we never send out a reply. If it is a remote request we send out a reply only if
    // either a) we reject the request or b) we accept it AND we have changed our own mode.
    if (info.optionState == OptInfo::REQUEST_SENT) {
        // This is a response
        info.optionState = OptInfo::ACKNOWLEDGED;
        info.enabled = enabled;
    }
    else if (enabled != info.enabled) {
        // Request to change the current state
        bool accept (enabled);
        if (!enabled || 
            enabled && (info.wantState == OptInfo::WANTED || info.wantState == OptInfo::ACCEPTED)) {
            // Accept the request
            info.optionState = OptInfo::ACKNOWLEDGED;
            info.enabled = enabled;
        }
        else
            // Reject the request
            accept = info.enabled;
        transmit(CMD_IAC);
        transmit((info.local ? CMD_WILL : CMD_DO) + (accept ? 0 : 1));
        transmit(info.option);
    }
    else
        return;
    if (info.enabled) {
        OptionHandlerMap::const_iterator i (handlers_.find(info.option));
        if (i != handlers_.end())
            i->second->v_init();
    }
}

///////////////////////////////////////////////////////////////////////////
// senf::console::detail::telnethandler::TerminalType

prefix_ senf::console::detail::telnethandler::TerminalType::TerminalType()
{
    registerHandler(this);
}

prefix_ void senf::console::detail::telnethandler::TerminalType::nextTerminalType()
{
    sendOptionParameters(telnetopt::TERMINAL_TYPE, "\x01");
}

prefix_ void senf::console::detail::telnethandler::TerminalType::
v_handleOptionParameters(std::string const & data)
{
    if (data.size() <= 0)
        return;
    if (data[0] == '\x00')
        v_handleTerminalType(data.substr(1));
}

prefix_ void senf::console::detail::telnethandler::TerminalType::v_init()
{
    nextTerminalType();
}

///////////////////////////////////////////////////////////////////////////
// senf::console::detail::telnethandler::NAWS

prefix_ senf::console::detail::telnethandler::NAWS::NAWS()
{
    registerHandler(this);
}

prefix_ void senf::console::detail::telnethandler::NAWS::v_init()
{}

prefix_ void
senf::console::detail::telnethandler::NAWS::v_handleOptionParameters(std::string const & data)
{
    if (data.size() != 4)
        return;
    v_handleWindowSize(
        (static_cast<unsigned char>(data[0])<<8)+static_cast<unsigned char>(data[1]),
        (static_cast<unsigned char>(data[2])<<8)+static_cast<unsigned char>(data[3]));
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Telnet.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
