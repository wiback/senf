// $Id$
//
// Copyright (C) 2009
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
    \brief Terminfo public header */

#ifndef HH_SENF_Utils_Termlib_Terminfo_
#define HH_SENF_Utils_Termlib_Terminfo_ 1

// Custom includes
#include <string>
#include <vector>
#include <map>
#include <iosfwd>
#include <boost/cstdint.hpp>
#include <senf/Utils/Exception.hh>

//#include "Terminfo.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace term {

    /** \defgroup terminfo_group Terminfo

        This facility provides access to the terminfo database. It partially re-implements the
        terminfo suppoprt in \c libncurses.

        \li The senf::term::Terminfo class provides basic terminfo property access and supports terminfo string
            formatting
        \li The senf::term::KeyParser class uses the properties found in a Terminfo instance to
            parser keyboard escape sequences into key codes

        The terminfo implementation is based on the \c utio library by Mike Sharov
        <msharov@users.sourceforge.net> found at http://sourceforge.net/projects/utio.
     */

    /** \brief Terminfo database entry

        This class reads a single terminfo database entry and allows to access the terminfo
        properties.

        \ingroup terminfo_group
     */
    class Terminfo
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        /** \brief NoValue constant
            This value represents the absence of a numeric property value */
        enum { NoValue = -1 };

        /** \brief Terminfo property constants

            This class provides a namespace for all Terminfo property constants
         */
        struct properties
        {
            /** \brief Boolean terminfo properties */
            enum Boolean {
                AutoLeftMargin, AutoRightMargin, NoEscCtlc, CeolStandoutGlitch, EatNewlineGlitch,
                EraseOverstrike, GenericType, HardCopy, HasMetaKey, HasStatusLine, InsertNullGlitch,
                MemoryAbove, MemoryBelow, MoveInsertMode, MoveStandoutMode, OverStrike,
                StatusLineEscOk, DestTabsMagicSmso, TildeGlitch, TransparentUnderline, XonXoff,
                NeedsXonXoff, PrtrSilent, HardCursor, NonRevRmcup, NoPadChar, NonDestScrollRegion,
                CanChange, BackColorErase, HueLightnessSaturation, ColAddrGlitch,
                CrCancelsMicroMode, HasPrintWheel, RowAddrGlitch, SemiAutoRightMargin,
                CpiChangesRes, LpiChangesRes, BackspacesWithBs, CrtNoScrolling,
                NoCorrectlyWorkingCr, GnuHasMetaKey, LinefeedIsNewline, HasHardwareTabs,
                ReturnDoesClrEol };

            /** \brief Boolean property names
                \hideinitializer */
            static char const * const BooleanNames[];

            /** \brief Numeric terminfo properties */
            enum Numeric {
                Columns, InitTabs, Lines, LinesOfMemory, MagicCookieGlitch, PaddingBaudRate,
                VirtualTerminal, WidthStatusLine, NumLabels, LabelHeight, LabelWidth, MaxAttributes,
                MaximumWindows, MaxColors, MaxPairs, NoColorVideo, BufferCapacity, DotVertSpacing,
                DotHorzSpacing, MaxMicroAddress, MaxMicroJump, MicroColSize, MicroLineSize,
                NumberOfPins, OutputResChar, OutputResLine, OutputResHorzInch, OutputResVertInch,
                PrintRate, WideCharSize, Buttons, BitImageEntwining, BitImageType,
                MagicCookieGlitchUl, CarriageReturnDelay, NewLineDelay, BackspaceDelay,
                HorizontalTabDelay, NumberOfFunctionKeys };

            /** \brief Numeric property names
                \hideinitializer */
            static char const * const NumericNames[];

            /** \brief String terminfo properties */
            enum String {
                BackTab, Bell, CarriageReturn, ChangeScrollRegion, ClearAllTabs, ClearScreen,
                ClrEol, ClrEos, ColumnAddress, CommandCharacter, CursorAddress, CursorDown,
                CursorHome, CursorInvisible, CursorLeft, CursorMemAddress, CursorNormal,
                CursorRight, CursorToLl, CursorUp, CursorVisible, DeleteCharacter, DeleteLine,
                DisStatusLine, DownHalfLine, EnterAltCharsetMode, EnterBlinkMode, EnterBoldMode,
                EnterCaMode, EnterDeleteMode, EnterDimMode, EnterInsertMode, EnterSecureMode,
                EnterProtectedMode, EnterReverseMode, EnterStandoutMode, EnterUnderlineMode,
                EraseChars, ExitAltCharsetMode, ExitAttributeMode, ExitCaMode, ExitDeleteMode,
                ExitInsertMode, ExitStandoutMode, ExitUnderlineMode, FlashScreen, FormFeed,
                FromStatusLine, Init1string, Init2string, Init3string, InitFile, InsertCharacter,
                InsertLine, InsertPadding, KeyBackspace, KeyCatab, KeyClear, KeyCtab, KeyDc, KeyDl,
                KeyDown, KeyEic, KeyEol, KeyEos, KeyF0, KeyF1, KeyF10, KeyF2, KeyF3, KeyF4, KeyF5,
                KeyF6, KeyF7, KeyF8, KeyF9, KeyHome, KeyIc, KeyIl, KeyLeft, KeyLl, KeyNpage,
                KeyPpage, KeyRight, KeySf, KeySr, KeyStab, KeyUp, KeypadLocal, KeypadXmit, LabF0,
                LabF1, LabF10, LabF2, LabF3, LabF4, LabF5, LabF6, LabF7, LabF8, LabF9, MetaOff,
                MetaOn, Newline, PadChar, ParmDch, ParmDeleteLine, ParmDownCursor, ParmIch,
                ParmIndex, ParmInsertLine, ParmLeftCursor, ParmRightCursor, ParmRindex,
                ParmUpCursor, PkeyKey, PkeyLocal, PkeyXmit, PrintScreen, PrtrOff, PrtrOn,
                RepeatChar, Reset1string, Reset2string, Reset3string, ResetFile, RestoreCursor,
                RowAddress, SaveCursor, ScrollForward, ScrollReverse, SetAttributes, SetTab,
                SetWindow, Tab, ToStatusLine, UnderlineChar, UpHalfLine, InitProg, KeyA1, KeyA3,
                KeyB2, KeyC1, KeyC3, PrtrNon, CharPadding, AcsChars, PlabNorm, KeyBtab,
                EnterXonMode, ExitXonMode, EnterAmMode, ExitAmMode, XonCharacter, XoffCharacter,
                EnaAcs, LabelOn, LabelOff, KeyBeg, KeyCancel, KeyClose, KeyCommand, KeyCopy,
                KeyCreate, KeyEnd, KeyEnter, KeyExit, KeyFind, KeyHelp, KeyMark, KeyMessage,
                KeyMove, KeyNext, KeyOpen, KeyOptions, KeyPrevious, KeyPrint, KeyRedo, KeyReference,
                KeyRefresh, KeyReplace, KeyRestart, KeyResume, KeySave, KeySuspend, KeyUndo,
                KeySbeg, KeyScancel, KeyScommand, KeyScopy, KeyScreate, KeySdc, KeySdl, KeySelect,
                KeySend, KeySeol, KeySexit, KeySfind, KeyShelp, KeyShome, KeySic, KeySleft,
                KeySmessage, KeySmove, KeySnext, KeySoptions, KeySprevious, KeySprint, KeySredo,
                KeySreplace, KeySright, KeySrsume, KeySsave, KeySsuspend, KeySundo, ReqForInput,
                KeyF11, KeyF12, KeyF13, KeyF14, KeyF15, KeyF16, KeyF17, KeyF18, KeyF19, KeyF20,
                KeyF21, KeyF22, KeyF23, KeyF24, KeyF25, KeyF26, KeyF27, KeyF28, KeyF29, KeyF30,
                KeyF31, KeyF32, KeyF33, KeyF34, KeyF35, KeyF36, KeyF37, KeyF38, KeyF39, KeyF40,
                KeyF41, KeyF42, KeyF43, KeyF44, KeyF45, KeyF46, KeyF47, KeyF48, KeyF49, KeyF50,
                KeyF51, KeyF52, KeyF53, KeyF54, KeyF55, KeyF56, KeyF57, KeyF58, KeyF59, KeyF60,
                KeyF61, KeyF62, KeyF63, ClrBol, ClearMargins, SetLeftMargin, SetRightMargin,
                LabelFormat, SetClock, DisplayClock, RemoveClock, CreateWindow, GotoWindow, Hangup,
                DialPhone, QuickDial, Tone, Pulse, FlashHook, FixedPause, WaitTone, User0, User1,
                User2, User3, User4, User5, User6, User7, User8, User9, OrigPair, OrigColors,
                InitializeColor, InitializePair, SetColorPair, SetForeground, SetBackground,
                ChangeCharPitch, ChangeLinePitch, ChangeResHorz, ChangeResVert, DefineChar,
                EnterDoublewideMode, EnterDraftQuality, EnterItalicsMode, EnterLeftwardMode,
                EnterMicroMode, EnterNearLetterQuality, EnterNormalQuality, EnterShadowMode,
                EnterSubscriptMode, EnterSuperscriptMode, EnterUpwardMode, ExitDoublewideMode,
                ExitItalicsMode, ExitLeftwardMode, ExitMicroMode, ExitShadowMode, ExitSubscriptMode,
                ExitSuperscriptMode, ExitUpwardMode, MicroColumnAddress, MicroDown, MicroLeft,
                MicroRight, MicroRowAddress, MicroUp, OrderOfPins, ParmDownMicro, ParmLeftMicro,
                ParmRightMicro, ParmUpMicro, SelectCharSet, SetBottomMargin, SetBottomMarginParm,
                SetLeftMarginParm, SetRightMarginParm, SetTopMargin, SetTopMarginParm,
                StartBitImage, StartCharSetDef, StopBitImage, StopCharSetDef, SubscriptCharacters,
                SuperscriptCharacters, TheseCauseCr, ZeroMotion, CharSetNames, KeyMouse, MouseInfo,
                ReqMousePos, GetMouse, SetAForeground, SetABackground, PkeyPlab, DeviceType,
                CodeSetInit, Set0DesSeq, Set1DesSeq, Set2DesSeq, Set3DesSeq, SetLrMargin,
                SetTbMargin, BitImageRepeat, BitImageNewline, BitImageCarriageReturn, ColorNames,
                DefineBitImageRegion, EndBitImageRegion, SetColorBand, SetPageLength, DisplayPcChar,
                EnterPcCharsetMode, ExitPcCharsetMode, EnterScancodeMode, ExitScancodeMode,
                PcTermOptions, ScancodeEscape, AltScancodeEsc, EnterHorizontalHlMode,
                EnterLeftHlMode, EnterLowHlMode, EnterRightHlMode, EnterTopHlMode,
                EnterVerticalHlMode, SetAAttributes, SetPglenInch, TermcapInit2, TermcapReset,
                LinefeedIfNotLf, BackspaceIfNotBs, OtherNonFunctionKeys, ArrowKeyMap, AcsUlcorner,
                AcsLlcorner, AcsUrcorner, AcsLrcorner, AcsLtee, AcsRtee, AcsBtee, AcsTtee, AcsHline,
                AcsVline, AcsPlus, MemoryLock, MemoryUnlock, BoxChars1 };

            /** \brief String property names
                \hideinitializer */
            static char const * const StringNames[];
        };

        typedef boost::int16_t number_t; ///< Numeric terminfo property type
        typedef char const* string_t;   ///< String terminfo property type

        //-////////////////////////////////////////////////////////////////////////

        Terminfo();
        explicit Terminfo(std::string const & term); ///< Load terminfo entry \a term
        void load(std::string const & term); ///< Load terminfo entry \a term

        bool getFlag(properties::Boolean p) const; ///< Get boolean property value
        number_t getNumber(properties::Numeric p) const; ///< Get numeric property value
        string_t getString(properties::String p) const; ///< Get string property value
                                        /**< If the property does not exist, 0 is returned */
        bool hasProperty(properties::Boolean p) const; ///< \c true, if boolean property \a p exists
        bool hasProperty(properties::Numeric p ) const; ///< \c true, if numeric property \a p exists
        bool hasProperty(properties::String p ) const; ///< \c true, if string property \a p exists

        std::string formatString(properties::String p,
                                 number_t arg1=NoValue, number_t arg2=NoValue,
                                 number_t arg3=NoValue, number_t arg4=NoValue,
                                 number_t arg5=NoValue, number_t arg6=NoValue,
                                 number_t arg7=NoValue, number_t arg8=NoValue,
                                 number_t arg9=NoValue) const;
                                        ///< Format string property value
                                        /**< Formats the string property \a p containing special
                                             terminfo codes. Terminfo supports up to 9 parameters. */

        //-////////////////////////////////////////////////////////////////////////

        void dump(std::ostream & os) const; ///< Dump a description of the terminfo entry

        /** \brief Invalid, incomplete or non-existent terminfo entry exception */
        struct InvalidTerminfoException : public senf::Exception {
            InvalidTerminfoException(std::string const & term = "");
        };

    private:
        typedef std::vector<bool> BoolVec;
        typedef std::vector<number_t> NumberVec;
        typedef std::vector<string_t> StringVec;
        typedef std::vector<char> StringPool;

        static std::string findTerminfo(std::string const & name);
        void load(std::istream & is);

        std::string name_;
        BoolVec booleans_;
        NumberVec numbers_;
        StringVec strings_;
        StringPool stringPool_;
    };

    /** \brief Parse key escape sequences

        The KeyParser will read the relevant properties from a terminfo entry and then provides
        functions to parser keyboard escape sequences.

        All keys are returned as keyboard code's. Values 0 to 255 represent ordinary ASCII
        characters, larger values are special keys taken from the KeyCode \c enum

        \ingroup terminfo_group
     */
    class KeyParser
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        /** \brief Special keyboard key codes */
        enum KeyCode {
            Space = ' ', Tab = '\t', Return = '\r', First = 0xE000, Esc = First, Backspace, Backtab,
            Begin, CATab, CTab, Cancel, Center, Clear, ClearToEOL, ClearToEOS, Close, Command, Copy,
            Create, Delete, DeleteLine, Down, DownLeft, DownRight, End, Enter, Exit, F0, F1, F2, F3,
            F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22,
            F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39,
            F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, F50, F51, F52, F53, F54, F55, F56,
            F57, F58, F59, F60, F61, F62, F63, Find, Help, Home, Insert, InsertLine, Left, Mark,
            Message, Mouse, Move, Next, Open, Options, PageDown, PageUp, Previous, Print, Redo,
            Reference, Refresh, Replace, Restart, Resume, Right, Save, Select, ShiftBegin,
            ShiftCancel, ShiftCommand, ShiftCopy, ShiftCreate, ShiftDelete, ShiftDeleteLine,
            ShiftEnd, ShiftClearToEOL, ShiftExit, ShiftFind, ShiftHelp, ShiftHome, ShiftInsert,
            ShiftLeft, ShiftMessage, ShiftMove, ShiftNext, ShiftOptions, ShiftPrevious, ShiftPrint,
            ShiftRedo, ShiftReplace, ShiftResume, ShiftRight, ShiftSave, ShiftSuspend, ShiftTab,
            ShiftUndo, Suspend, Undo, Up, UpLeft, UpRight, Incomplete = 0xE0FF };

        /** \brief Special key code names
            \hideinitializer */
        static char const * const KeyNames[];

        typedef wchar_t keycode_t;      ///< Key code data type
        typedef std::string::size_type size_type; ///< String length type

        /** \brief Helper to convert uppercase char to Control key code */
        static keycode_t Ctrl(char ch) { return ch-'@'; }

        //-////////////////////////////////////////////////////////////////////////

        KeyParser();
        explicit KeyParser(Terminfo const & ti); ///< Load keymap information from \a ti
        void load(Terminfo const & ti); ///< Load keymap information from \a ti

        std::pair<keycode_t, size_type> lookup(std::string const & key) const;
                                        ///< Lookup up string \a key
                                        /**< This call will look up the string in \a key in the
                                             keymap. The return value is a pair.
                                             \li the first value is the key code
                                             \li the second value is the number of characters in \a
                                                 key which have been interpreted and returned in the
                                                 first value

                                             If \a key is not a complete key sequence, the return
                                             key code will be the \c Invalid value and the returned
                                             escape sequence size will be 0.

                                             If \a key does not start with an escape sequence, the
                                             returned key code will be the first character from \a
                                             key and the escape sequence size will be 1.

                                             Otherwise \a key starts with a complete escape
                                             sequence. The returned key code will be a value from
                                             the KeyCode enum and the escape sequence size will be
                                             returned accordingly. */
        static std::string describe(keycode_t key); ///< Return descriptive, printable key name

        void dump(std::ostream & os) const; ///< Dump keymap for debug purposes

    private:
        typedef std::map<std::string, KeyCode> Keytable;

        Keytable table_;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Terminfo.cci"
//#include "Terminfo.ct"
//#include "Terminfo.cti"
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
