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
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Scheduler/ClockService.hh>

//#include "Telnet.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace term {

    /** \defgroup telnet_group Telnet protocol

        The telnet protocol implementation is based on the relevant RFCs:

        \li <a href="http://tools.ietf.org/html/rfc854">RFC 854</a> The Telnet protocol
        \li <a href="http://tools.ietf.org/html/rfc854">RFC 855</a> Telnet option specifications

        BaseTelnetProtocol provides the basic protocol implementation with generic telnet option
        support. Several other classes provide additional support for further telnet options:

        \li telnethandler::TerminalType provides support for the TERMINAL_TYPE option
        \li telnethandler::NAWS provides support for the NAWS (Negotiation About Window Size) option

        There are some symbolic constants for telnet option in the senf::term::telnetopt namespace.
     */

    /** \brief Telnet protocol implementation

        This class implements the basic telnet protocol. It provides option parsing and negotiation
        and implements the necessary character quoting.

        This class is a base class. Several virtual function hooks are provided which will be called
        by the protocol implementation.

        The telnet protocol implementation is symmetric: At this level, there is no difference
        between a telnet client and a telnet server.

        This telnet protocol base class integrates with the SENF Scheduler, it relies on the
        scheduler main loop to be active.

        \section telnethandlers Telnet option handlers

        To support complex telnet option which require subnegotiation, TelnetHandler's are
        provided. A telnet handler is a class derived from TelnetHandler and then inherited into the
        target telnet implementation class:

        \code
        class MyTelnet
            : public senf::term::telnethandler::TerminalType,
              public senf::term::telnethandler::NAWS
        {
            MyTelnet(Handle handle) : BaseTelnetProtocol(handle) {}

            // ...
        };
        \endcode

        It is very important, the BaseTelnetProtocol is \e always inherited by \c public \c virtual
        inheritance. This allows the BaseTelnetProtocol constructor to be called by the most derived
        class directly as above.

        Each terminal handler may provide an additional API and/or additional virtual function
        callbacks.

        To implement a new terminal handler, derive from TelnetHandler and register the
        handler. BaseTelnetProtocol will automatically request the corresponding option to be
        enabled and will call the handlers \c v_init() member as soon as the peer as asserted the
        option.

        Whenever a subnegotiation for the registered handler is received, the handlers
        \c v_handleOptionParameters() member is called.

        \code
        class MyTelnetHandler
            : public senf::term::BaseTelnetProtocol::TelnetHandler
        {
        public:
            // This constant is MANDATORY and gives the option code which this handler services
            static option_type const OPTION_CODE = MY_OPTION_CODE;

            void frobble() { // ... }

        protected:
            MyTelnetHandler() { registerHandler(this); }

        private:
            virtual void v_init()
            {
                sendOptionParameters(OPTION_CODE, "my special subnegotiation");
                incrementRequestCounter();
            }

            virtual void v_handleOptionParameters(std::string const & data)
            {
                if (data == "another special subnegotiation")
                    decrementRequestCounter();
            }
        };
        \endcode

        \todo SYNCH handling

        \ingroup telnet_group
     */
    class BaseTelnetProtocol
    {
    public:
        static unsigned const DEFAULT_REQUEST_TIMEOUT_MS = 500u;

        typedef ClientSocketHandle<senf::MakeSocketPolicy<
            ConnectedCommunicationPolicy,
            StreamFramingPolicy,
            ReadablePolicy,
            WriteablePolicy>::policy> Handle; ///< Type of socket handle required

        typedef unsigned char option_type; ///< Type of telnet option numbers

        struct TelnetHandler;

        void write(std::string const & s); ///< Send string to peer
                                        /**< The string will be correctly quoted and newline chars
                                             will be sent as CR/LF pairs. */
        void write(char c);             ///< Send single character to peer
                                        /**< The character will be correctly quoted and newline
                                             chars will be sent as CR/LF pairs. */

        Handle handle();                ///< Get socket handle

        void sendNOP();                 ///< Send NOP to peer
        void sendBRK();                 ///< Send BReaK to peer
        void sendIP();                  ///< Send InterruptProcess to peer
        void sendAO();                  ///< Send AbortOutput to peer
        void sendAYT();                 ///< Send AreYouThere to peer
        void sendEC();                  ///< Send EraseCharacter to peer
        void sendEL();                  ///< Send EraseLine to peer
        void sendGA();                  ///< Send GoAhead to peer

        void sendOptionParameters(option_type option, std::string const & data);
                                        ///< Send extended option parameter to peer
                                        /**< This will send \a data as extended option parameter of
                                             option \a option to peer via a subnegotiation. */

        void requestLocalOption(option_type option, bool enabled = true);
                                        ///< Request option to be enabled here
                                        /**< This will send a WILL \a option request to the peer */
        void acceptLocalOption(option_type option, bool enabled = true);
                                        ///< Accept a request for an option to be enabled here
                                        /**< If the peer sends a DO \a option request, the request
                                             will be granted */

        void requestPeerOption(option_type option, bool enabled = true);
                                        ///< Request peer to enable an option
                                        /**< This will send a DO \a option request to the peer */
        void acceptPeerOption(option_type option, bool enabled = true);
                                        ///< Accept a request by the peer to enable an option
                                        /**< If the peer sends a WILL \a option request, the request
                                             will be ganted */

        bool localOption(option_type option); ///< \c true, if \a option locally enabled
        bool peerOption(option_type option); ///< \c true, if \a option enabled in peer

    protected:
        explicit BaseTelnetProtocol(Handle handle); ///< Construct telnet protocol handler
        BaseTelnetProtocol();           ///< Provided for TelnetHandler mixins only

        virtual ~BaseTelnetProtocol();

        template <class Handler>
        void registerHandler(Handler * h, bool request=true);
                                        ///< Register a TelnetHandler

        void incrementRequestCounter(); ///< Increment request counter
                                        /**< This member may be called by a telnet handler to wait
                                             for additional negotiations. It must be paired with a
                                             corresponding decrementRequestCounter() call when that
                                             negotiation is received. */
        void decrementRequestCounter(); ///< Decrement request counter
                                        /**< \see inrementRequestCounter() */
        bool requestsPending();         ///< \c true, if there are pending requests

#ifndef DOXYGEN
    private:
#endif
        virtual void v_setupComplete() = 0; ///< Called, when no further requests are pending
                                        /**< This callback will be called, when no further
                                             negotiations are to be expected. The default
                                             negotiation timeout is 500ms. If no reply is received
                                             in this time, the request is abandoned and
                                             v_setupComplete() is called.

                                             mixin TelnetHandler's may additionally increment the
                                             request counter to wait for specific
                                             subnegotiations. v_setupComplete() will be called, when
                                             all these negotiations are complete or have timed
                                             out. */
        virtual void v_charReceived(char c) = 0; ///< Called whenever a data character is received
        virtual void v_eof() = 0;       ///< Called on input EOF

        virtual void v_handleNOP();     ///< Called, when the peer sends a NOP
        virtual void v_handleBRK();     ///< Called, when the peer sends a BReaK
        virtual void v_handleIP();      ///< Called, when the peer sends an InterruptProcess
        virtual void v_handleAO();      ///< Called, when the peer sends an AbortOutput
        virtual void v_handleAYT();     ///< Called, when the peer sends an AreYouThere
        virtual void v_handleEC();      ///< Called, when the peer sends an EraseCharacter
        virtual void v_handleEL();      ///< Called, when the peer sends an EraseLine
        virtual void v_handleGA();      ///< Called, when the peer sends a GoAhead

    private:
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

            //-////////////////////////////////////////////////////////////

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

        scheduler::FdEvent inputEvent_;
        scheduler::FdEvent outputEvent_;

        unsigned pendingRequests_;

        ClockService::clock_type requestTimeout_;
        scheduler::TimerEvent timeout_;

        friend class TelnetHandler;
    };

    /** \brief Telnet handler base class

        \see BaseTelnetProtocol
     */
    struct BaseTelnetProtocol::TelnetHandler
        : public virtual BaseTelnetProtocol
    {
        virtual ~TelnetHandler();
        virtual void v_init() = 0;      ///< Called, after option has been enabled
        virtual void v_handleOptionParameters(std::string const & data) = 0;
                                        ///< Called, whenever a subnegotiation is received
    };

    /** \brief Telnet option codes

        See http://www.iana.org/assignments/telnet-options for a list of options

        \ingroup telnet_group
     */
    namespace telnetopt { BaseTelnetProtocol::option_type const ECHO = 1u; }
    namespace telnetopt { BaseTelnetProtocol::option_type const TRANSMIT_BINARY = 0u; }
    namespace telnetopt { BaseTelnetProtocol::option_type const SUPPRESS_GO_AHEAD = 3u; }
    namespace telnetopt { BaseTelnetProtocol::option_type const TERMINAL_TYPE = 24u; }
    namespace telnetopt { BaseTelnetProtocol::option_type const NAWS = 31u; }
    namespace telnetopt { BaseTelnetProtocol::option_type const LINEMODE = 34u; }

/** \brief Telnet handlers

    \ingroup telnet_group */
namespace telnethandler {

    /** \brief Implement TERMINAL_TYPE option

        This telnet handler implements the TERMINAL_TYPE option. The handler automatically requests
        the first terminal type during initialization. Further terminal types may then be requested
        by calling nextTerminalType().

        The last received terminal type will be returned by the terminalType() member.

        This implementation only provides server support (querying the terminal type).

        \see BaseTelnetProtocol for how to integrate this handler \n
            <a href="http://tools.ietf.org/html/rfc1091">RFC 1091</a> Telnet Terminal-Type Option
     */
    class TerminalType
        : public BaseTelnetProtocol::TelnetHandler
    {
    public:
        static option_type const OPTION_CODE = telnetopt::TERMINAL_TYPE;

        void nextTerminalType();        ///< Request another terminal type
        std::string const & terminalType() const; ///< Return current terminal type

    protected:
        TerminalType();

    private:
        virtual void v_init();
        virtual void v_handleOptionParameters(std::string const & data);

        std::string type_;
    };

    /** \brief Implement NAWS (Negotiation About Window Size) option

        This telnet handler implements the NAWS option. The client terminals window size will be
        requested during initialization. The current window size may always be accessed using the
        width() and height() members.

        Whenever the window size is changed, the v_windowSizeChanged() function is called. This
        function must be implemented in a derived class.

        \see BaseTelnetProtocol for how to integrate this handler \n
            <a href="http://tools.ietf.org/html/rfc1073">RFC 1073</a> Telnet Window Size Option
     */
    class NAWS
        : public BaseTelnetProtocol::TelnetHandler
    {
    public:
        static option_type const OPTION_CODE = telnetopt::NAWS;

        unsigned width() const;         ///< Get current client window width
        unsigned height() const;        ///< Get current client window height

    protected:
        NAWS();

#   ifndef DOXYGEN
    private:
#   endif
        virtual void v_windowSizeChanged() = 0; ///< Called, whenever window size changes
                                        /**< This member is called for every window size change \e
                                             after initialization. */

    private:
        virtual void v_init();
        virtual void v_handleOptionParameters(std::string const & data);

        unsigned width_;
        unsigned height_;
    };

}

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
