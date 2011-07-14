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
    \brief Telnet non-inline non-template implementation */

#include "Telnet.hh"
//#include "Telnet.ih"

// Custom includes
#include <boost/algorithm/string/case_conv.hpp>
#include <senf/Utils/membind.hh>
#include <senf/Utils/Logger/SenfLog.hh>

//#include "Telnet.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::term::BaseTelnetProtocol::BaseTelnetProtocol(Handle handle)
    : handle_ (handle), charState_ (NORMAL), command_ (CMD_NONE), option_ (0),
      inputEvent_ ("senf::term::BaseTelnetProtocol::input",
                   membind(&BaseTelnetProtocol::readHandler, this), handle,
                   scheduler::FdEvent::EV_READ),
      outputEvent_ ("senf::term::BaseTelnetProtocol::output",
                    membind(&BaseTelnetProtocol::writeHandler, this), handle,
                    scheduler::FdEvent::EV_WRITE, false),
      pendingRequests_ (0u),
      requestTimeout_ (ClockService::milliseconds(DEFAULT_REQUEST_TIMEOUT_MS)),
      timeout_ ("senf::term::BaseTelnetProtocol::configTimeout",
                membind(&BaseTelnetProtocol::timeout, this))
{}

prefix_ senf::term::BaseTelnetProtocol::BaseTelnetProtocol()
    : handle_ (), charState_ (NORMAL), command_ (CMD_NONE), option_ (0),
      inputEvent_ ("senf::term::BaseTelnetProtocol::input", 0),
      outputEvent_ ("senf::term::BaseTelnetProtocol::output", 0),
      pendingRequests_ (0u),
      requestTimeout_ (ClockService::milliseconds(DEFAULT_REQUEST_TIMEOUT_MS)),
      timeout_ ("senf::term::BaseTelnetProtocol::timeout", 0)
{
    SENF_ASSERT( false,
                 "Missing BaseTelnetProtocol constructor call in derived class "
                 "(BaseTelnetProtocol is a VIRTUAL base and MUST be constructed explicitly "
                 "in the most derived class." );
}

prefix_ void senf::term::BaseTelnetProtocol::write(std::string const & s)
{
    for (std::string::const_iterator i (s.begin()); i != s.end(); ++i)
        write(*i);
}

prefix_ void senf::term::BaseTelnetProtocol::write(char c)
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
senf::term::BaseTelnetProtocol::sendOptionParameters(option_type option,
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

prefix_ void senf::term::BaseTelnetProtocol::v_handleNOP()
{}

prefix_ void senf::term::BaseTelnetProtocol::v_handleBRK()
{}

prefix_ void senf::term::BaseTelnetProtocol::v_handleIP()
{}

prefix_ void senf::term::BaseTelnetProtocol::v_handleAO()
{}

prefix_ void senf::term::BaseTelnetProtocol::v_handleAYT()
{}

prefix_ void senf::term::BaseTelnetProtocol::v_handleEC()
{}

prefix_ void senf::term::BaseTelnetProtocol::v_handleEL()
{}

prefix_ void senf::term::BaseTelnetProtocol::v_handleGA()
{}

prefix_ void senf::term::BaseTelnetProtocol::handleChar(char c)
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

prefix_ void senf::term::BaseTelnetProtocol::handleNormalChar(char c)
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

prefix_ void senf::term::BaseTelnetProtocol::handleCommand(char c)
{
    switch (static_cast<unsigned char>(c)) {
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

prefix_ void senf::term::BaseTelnetProtocol::handleOption(char c)
{
    option_ = c;
    processCommand();
    charState_ = NORMAL;
}

prefix_ void senf::term::BaseTelnetProtocol::handleCR(char c)
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

prefix_ void senf::term::BaseTelnetProtocol::handleSBOption(char c)
{
    option_ = c;
    charState_ = SB_DATA;
    data_.clear();
}

prefix_ void senf::term::BaseTelnetProtocol::handleSBData(char c)
{
    if (c == '\xff')
        charState_ = SB_IAC_SEEN;
    else
        data_.push_back(c);
}

prefix_ void senf::term::BaseTelnetProtocol::handleSBIAC(char c)
{
    switch (static_cast<unsigned char>(c)) {
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

prefix_ void senf::term::BaseTelnetProtocol::processCommand()
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

prefix_ void senf::term::BaseTelnetProtocol::transmit(char c)
{
    sendQueue_.push_back(c);
    outputEvent_.enable();
}

prefix_ void senf::term::BaseTelnetProtocol::readHandler(int state)
{
    if (state != scheduler::FdEvent::EV_READ || handle_.eof()) {
        inputEvent_.disable();
        v_eof();
        return;
    }
    std::string data;
    handle_.read(data, 0u);
    for (std::string::const_iterator i (data.begin()); i != data.end(); ++i)
        handleChar(*i);
}

prefix_ void senf::term::BaseTelnetProtocol::writeHandler(int state)
{
    if (state != scheduler::FdEvent::EV_WRITE) {
        outputEvent_.disable();
        inputEvent_.disable();
        v_eof();
        return;
    }
    sendQueue_.erase(sendQueue_.begin(),
                     handle_.write(boost::make_iterator_range(
                                       sendQueue_.begin(), sendQueue_.end())));
    if (sendQueue_.empty())
        outputEvent_.disable();
}

prefix_ void senf::term::BaseTelnetProtocol::timeout()
{
    if (pendingRequests_ > 0u) {
        pendingRequests_ = 0u;
        v_setupComplete();
    }
}

prefix_ senf::term::BaseTelnetProtocol::OptInfo &
senf::term::BaseTelnetProtocol::getOption(bool local, option_type option)
{
    OptionsMap::iterator i (options_.find(std::make_pair(local, option)));
    if (i == options_.end())
        i = options_.insert(std::make_pair(std::make_pair(local, option),
                                           OptInfo(local, option))).first;
    return i->second;
}

prefix_ void senf::term::BaseTelnetProtocol::request(OptInfo & info, bool enabled)
{
    info.wantState = enabled ? OptInfo::WANTED : OptInfo::DISABLED;
    if (enabled != info.enabled) {
        transmit(CMD_IAC);
        transmit((info.local ? CMD_WILL : CMD_DO) + (enabled ? 0 : 1));
        transmit(info.option);
        info.optionState = OptInfo::REQUEST_SENT;
        incrementRequestCounter();
    }
}

prefix_ void senf::term::BaseTelnetProtocol::response(OptInfo & info, bool enabled)
{
    bool decrementCount (false);

    // If this is a response, we need to unconditionally accept it.  If this is a remote
    // configuration request, we accept it if wantState is wither WANTED or ACCEPTED.  If this is a
    // response, we never send out a reply. If it is a remote request we send out a reply only if
    // either a) we reject the request or b) we accept it AND we have changed our own mode.
    if (info.optionState == OptInfo::REQUEST_SENT) {
        // This is a response
        info.optionState = OptInfo::ACKNOWLEDGED;
        info.enabled = enabled;
        decrementCount = true;
    }
    else if (enabled != info.enabled) {
        // Request to change the current state
        if (!enabled ||
            (enabled && (info.wantState == OptInfo::WANTED || info.wantState == OptInfo::ACCEPTED))) {
            // accept the request
            info.optionState = OptInfo::ACKNOWLEDGED;
            info.enabled = enabled;
        }   // else reject the request
        transmit(CMD_IAC);
        transmit((info.local ? CMD_WILL : CMD_DO) + (info.enabled ? 0 : 1));
        transmit(info.option);
    }
    else
        return;
    if (info.enabled) {
        OptionHandlerMap::const_iterator i (handlers_.find(info.option));
        if (i != handlers_.end())
            i->second->v_init();
    }
    if (decrementCount)
        // This call must be AFTER calling v_init since v_init might increment the request count
        // and v_setupComplete() might be called prematurely.
        decrementRequestCounter();
}

prefix_ void senf::term::BaseTelnetProtocol::decrementRequestCounter()
{
    if (pendingRequests_ > 0u) {
        -- pendingRequests_;
        if (pendingRequests_ == 0u) {
            timeout_.disable();
            v_setupComplete();
        }
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::term::telnethandler::TerminalType

prefix_ senf::term::telnethandler::TerminalType::TerminalType()
{
    registerHandler(this);
}

prefix_ void senf::term::telnethandler::TerminalType::nextTerminalType()
{
    sendOptionParameters(telnetopt::TERMINAL_TYPE, "\x01");
}

prefix_ void senf::term::telnethandler::TerminalType::
v_handleOptionParameters(std::string const & data)
{
    if (data.size() <= 0)
        return;
    if (data[0] == '\x00') {
        type_ = data.substr(1);
        boost::algorithm::to_lower(type_);
        decrementRequestCounter();
    }
}

prefix_ void senf::term::telnethandler::TerminalType::v_init()
{
    nextTerminalType();
    incrementRequestCounter();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::term::telnethandler::NAWS

prefix_ senf::term::telnethandler::NAWS::NAWS()
    : width_ (0u), height_ (0u)
{
    registerHandler(this);
}

prefix_ void senf::term::telnethandler::NAWS::v_init()
{
    incrementRequestCounter();
}

prefix_ void
senf::term::telnethandler::NAWS::v_handleOptionParameters(std::string const & data)
{
    if (data.size() != 4)
        return;
    width_ = (static_cast<unsigned char>(data[0])<<8)+static_cast<unsigned char>(data[1]);
    height_ = (static_cast<unsigned char>(data[2])<<8)+static_cast<unsigned char>(data[3]);
    if (! requestsPending())
        v_windowSizeChanged();
    decrementRequestCounter();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
