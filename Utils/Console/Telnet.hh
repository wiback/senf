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
    \brief Telnet public header */

#ifndef HH_SENF_Scheduler_Console_Telnet_
#define HH_SENF_Scheduler_Console_Telnet_ 1

// Custom includes
#include <vector>
#include <map>
#include <senf/Socket.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Scheduler/ClockService.hh>

//#include "Telnet.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {
namespace detail {

    /** \brief  Telnet server

        \see 
            <a href="http://tools.ietf.org/html/rfc854>RFC 854</a> The Telnet protocol \n
            <a href="http://tools.ietf.org/html/rfc854>RFC 855</a> Telnet option specifications

        \todo SYNCH handling
     */
    class BaseTelnetProtocol
    {
    public:
        static unsigned const DEFAULT_REQUEST_TIMEOUT_MS = 500u;

        typedef ClientSocketHandle<senf::MakeSocketPolicy<
            ConnectedCommunicationPolicy, 
            StreamFramingPolicy, 
            ReadablePolicy,
            WriteablePolicy>::policy> Handle;

        typedef unsigned char option_type;

        struct TelnetHandler;

        void write(std::string const & s);
        void write(char c);

        void sendNOP();
        void sendBRK();
        void sendIP();
        void sendAO();
        void sendAYT();
        void sendEC();
        void sendEL();
        void sendGA();
        
        void sendOptionParameters(option_type option, std::string const & data);

        void requestLocalOption(option_type option, bool enabled = true);
        void acceptLocalOption(option_type option, bool enabled = true);

        void requestPeerOption(option_type option, bool enabled = true);
        void acceptPeerOption(option_type option, bool enabled = true);
        
    protected:
        explicit BaseTelnetProtocol(Handle handle);
        BaseTelnetProtocol();
        virtual ~BaseTelnetProtocol();

        template <class Handler>
        void registerHandler(Handler * h, bool request=true);

        void incrementRequestCounter();
        void decrementRequestCounter();
        bool requestsPending();

    private:

#ifndef DOXYGEN
    private:
#endif
        virtual void v_setupComplete() = 0;
        virtual void v_charReceived(char c) = 0;
        virtual void v_eof() = 0;

        virtual void v_handleNOP();
        virtual void v_handleBRK();
        virtual void v_handleIP();
        virtual void v_handleAO();
        virtual void v_handleAYT();
        virtual void v_handleEC();
        virtual void v_handleEL();
        virtual void v_handleGA();
        
#ifdef DOXYGEN
    private:
#endif
        void handleChar(char c);
        void handleNormalChar(char c);
        void handleCommand(char c);
        void handleOption(char c);
        void handleCR(char c);
        void handleSBOption(char c);
        void handleSBData(char c);
        void handleSBIAC(char c);
        void emit(char c);
        void processCommand();
        void transmit(char c);

        void sendWILL(char option);
        void sendWONT(char option);
        void sendDO(char option);
        void sendDONT(char option);

        void readHandler(int state);
        void writeHandler(int state);
        void timeout();

        enum Command { 
            CMD_NONE = 0,
            CMD_SE = 240,
            CMD_NOP = 241,
            CMD_DM = 242,
            CMD_BRK = 243,
            CMD_IP = 244,
            CMD_AO = 245,
            CMD_AYT = 246,
            CMD_EC = 247,
            CMD_EL = 248,
            CMD_GA = 249,
            CMD_SB = 250,
            CMD_WILL = 251,
            CMD_WONT = 252,
            CMD_DO = 253,
            CMD_DONT = 254,
            CMD_IAC = 255,
        };

        struct OptInfo
        {
            enum WantState { WANTED, ACCEPTED, DISABLED };
            enum OptionState { NONE, REQUEST_SENT, ACKNOWLEDGED };

            OptInfo();
            OptInfo(bool local, option_type option);

            ///////////////////////////////////////////////////////////////

            bool const local;
            option_type const option;

            WantState wantState;
            OptionState optionState;
            bool enabled;
            
        };

        OptInfo & getOption(bool local, option_type option);
        void request(OptInfo & info, bool enabled);
        void response(OptInfo & info, bool enabled);

        typedef std::map<std::pair<bool, option_type>, OptInfo> OptionsMap;
        OptionsMap options_;

        typedef std::map<option_type, TelnetHandler*> OptionHandlerMap;
        OptionHandlerMap handlers_;

        Handle handle_;

        typedef std::vector<char> SendQueue;
        SendQueue sendQueue_;

        enum CharState { NORMAL, IAC_SEEN, EXPECT_OPTION, CR_SEEN, 
                         SB_OPTION, SB_DATA, SB_IAC_SEEN };
        CharState charState_;

        Command command_;
        option_type option_;
        std::string data_;

        senf::scheduler::FdEvent inputEvent_;
        senf::scheduler::FdEvent outputEvent_;

        unsigned pendingRequests_;

        ClockService::clock_type requestTimeout_;
        scheduler::TimerEvent timeout_;

        friend class TelnetHandler;
    };

    struct BaseTelnetProtocol::TelnetHandler
        : public virtual BaseTelnetProtocol
    {
        virtual ~TelnetHandler();
        virtual void v_init() = 0;
        virtual void v_handleOptionParameters(std::string const & data) = 0;
    };

    // See http://www.iana.org/assignments/telnet-options for a list of options
    namespace telnetopt { BaseTelnetProtocol::option_type const ECHO = 1u; }
    namespace telnetopt { BaseTelnetProtocol::option_type const TRANSMIT_BINARY = 0u; }
    namespace telnetopt { BaseTelnetProtocol::option_type const SUPPRESS_GO_AHEAD = 3u; }
    namespace telnetopt { BaseTelnetProtocol::option_type const TERMINAL_TYPE = 24u; }
    namespace telnetopt { BaseTelnetProtocol::option_type const NAWS = 31u; }
    namespace telnetopt { BaseTelnetProtocol::option_type const LINEMODE = 34u; }

namespace telnethandler {

    class TerminalType
        : public BaseTelnetProtocol::TelnetHandler
    {
    public:
        static option_type const OPTION_CODE = telnetopt::TERMINAL_TYPE;

        void nextTerminalType();
        std::string const & terminalType() const;
        
    protected:
        TerminalType();

    private:
        virtual void v_init();
        virtual void v_handleOptionParameters(std::string const & data);

        std::string type_;
    };

    class NAWS 
        : public BaseTelnetProtocol::TelnetHandler
    {
    public:
        static option_type const OPTION_CODE = telnetopt::NAWS;

        unsigned width() const;
        unsigned height() const;

    protected:
        NAWS();
        
    private:
        virtual void v_windowSizeChanged() = 0;

        virtual void v_init();
        virtual void v_handleOptionParameters(std::string const & data);

        unsigned width_;
        unsigned height_;
    };
    
}

}}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Telnet.cci"

//#include "Telnet.ct"
#include "Telnet.cti"
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
