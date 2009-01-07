// $Id$
//
// Copyright (C) 2009 
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
    \brief Terminfo public header */

#ifndef HH_SENF_Utils_Termlib_Terminfo_
#define HH_SENF_Utils_Termlib_Terminfo_ 1

// Custom includes
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <boost/cstdint.hpp>
#include <boost/array.hpp>
#include <senf/Utils/Exception.hh>

//#include "Terminfo.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace term {

    class Terminfo
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        enum { NoValue = -1 };

        struct properties
        {
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
            
            static char const * const BooleanNames[];

            enum Numeric {
                Columns, InitTabs, Lines, LinesOfMemory, MagicCookieGlitch, PaddingBaudRate,
                VirtualTerminal, WidthStatusLine, NumLabels, LabelHeight, LabelWidth, MaxAttributes,
                MaximumWindows, MaxColors, MaxPairs, NoColorVideo, BufferCapacity, DotVertSpacing,
                DotHorzSpacing, MaxMicroAddress, MaxMicroJump, MicroColSize, MicroLineSize,
                NumberOfPins, OutputResChar, OutputResLine, OutputResHorzInch, OutputResVertInch,
                PrintRate, WideCharSize, Buttons, BitImageEntwining, BitImageType,
                MagicCookieGlitchUl, CarriageReturnDelay, NewLineDelay, BackspaceDelay,
                HorizontalTabDelay, NumberOfFunctionKeys };

            static char const * const NumericNames[];
            
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

            static char const * const StringNames[];
        };

        typedef boost::int16_t number_t;
        typedef char const* string_t;

        ///////////////////////////////////////////////////////////////////////////
        
        Terminfo();
        explicit Terminfo(std::string const & term);
        void load(std::string const & term);
        
        bool getFlag(properties::Boolean p) const;
        number_t getNumber(properties::Numeric p) const;
        string_t getString(properties::String p) const;
        bool hasProperty(properties::Boolean p) const;
        bool hasProperty(properties::Numeric p ) const;
        bool hasProperty(properties::String p ) const;
        std::string formatString(properties::String p,
                                 number_t arg1=NoValue, number_t arg2=NoValue, 
                                 number_t arg3=NoValue, number_t arg4=NoValue,
                                 number_t arg5=NoValue, number_t arg6=NoValue,
                                 number_t arg7=NoValue, number_t arg8=NoValue,
                                 number_t arg9=NoValue) const;

        ///////////////////////////////////////////////////////////////////////////

        void dump(std::ostream & os) const;

        struct InvalidTerminfoException : public senf::Exception
        { InvalidTerminfoException() : senf::Exception("Unreadable terminfo file") {} };

    private:
        typedef std::vector<bool> BoolVec;
        typedef std::vector<number_t> NumberVec;
        typedef std::vector<string_t> StringVec;
        typedef std::vector<char> StringPool;

        std::string findTerminfo(std::string const & name);
        void load(std::istream & is);

        std::string name_;
        BoolVec booleans_;
        NumberVec numbers_;
        StringVec strings_;
        StringPool stringPool_;
    };


    class KeyParser
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

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

        static char const * const KeyNames[];

        typedef wchar_t keycode_t;
        typedef std::string::size_type size_type;

        static keycode_t Ctrl(char ch) { return ch-'@'; }

        ///////////////////////////////////////////////////////////////////////////

        KeyParser();
        explicit KeyParser(Terminfo const & ti);
        void load(Terminfo const & ti);

        std::pair<keycode_t, size_type> lookup(std::string const & key) const;
        static std::string describe(keycode_t key);

        void dump(std::ostream & os) const;

    private:
        typedef std::map<std::string, KeyCode> Keytable;

        Keytable table_;
    };
        
}}

///////////////////////////////hh.e////////////////////////////////////////
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
