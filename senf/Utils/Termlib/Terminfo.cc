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
    \brief Terminfo non-inline non-template implementation */

#include "Terminfo.hh"
//#include "Terminfo.ih"

// Custom includes
#include <fstream>
#include <iomanip>
#include <boost/filesystem/operations.hpp>
#include <senf/config.hh>
#include <senf/Utils/hexdump.hh>
#include <unistd.h>
#include <string.h>

// Compatibility with glibc < 2.9
#if !defined(le16toh)
#  include <byteswap.h>
#  include <arpa/inet.h>
#  define le16toh(x)  (ntohs(bswap_16(x)))
#endif

//#include "Terminfo.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

char const * const senf::term::Terminfo::properties::BooleanNames [] = {
    "AutoLeftMargin", "AutoRightMargin", "NoEscCtlc", "CeolStandoutGlitch", "EatNewlineGlitch",
    "EraseOverstrike", "GenericType", "HardCopy", "HasMetaKey", "HasStatusLine", "InsertNullGlitch",
    "MemoryAbove", "MemoryBelow", "MoveInsertMode", "MoveStandoutMode", "OverStrike",
    "StatusLineEscOk", "DestTabsMagicSmso", "TildeGlitch", "TransparentUnderline", "XonXoff",
    "NeedsXonXoff", "PrtrSilent", "HardCursor", "NonRevRmcup", "NoPadChar", "NonDestScrollRegion",
    "CanChange", "BackColorErase", "HueLightnessSaturation", "ColAddrGlitch", "CrCancelsMicroMode",
    "HasPrintWheel", "RowAddrGlitch", "SemiAutoRightMargin", "CpiChangesRes", "LpiChangesRes",
    "BackspacesWithBs", "CrtNoScrolling", "NoCorrectlyWorkingCr", "GnuHasMetaKey",
    "LinefeedIsNewline", "HasHardwareTabs", "ReturnDoesClrEol" };

char const * const senf::term::Terminfo::properties::NumericNames[] = {
    "Columns", "InitTabs", "Lines", "LinesOfMemory", "MagicCookieGlitch", "PaddingBaudRate",
    "VirtualTerminal", "WidthStatusLine", "NumLabels", "LabelHeight", "LabelWidth", "MaxAttributes",
    "MaximumWindows", "MaxColors", "MaxPairs", "NoColorVideo", "BufferCapacity", "DotVertSpacing",
    "DotHorzSpacing", "MaxMicroAddress", "MaxMicroJump", "MicroColSize", "MicroLineSize",
    "NumberOfPins", "OutputResChar", "OutputResLine", "OutputResHorzInch", "OutputResVertInch",
    "PrintRate", "WideCharSize", "Buttons", "BitImageEntwining", "BitImageType",
    "MagicCookieGlitchUl", "CarriageReturnDelay", "NewLineDelay", "BackspaceDelay",
    "HorizontalTabDelay", "NumberOfFunctionKeys" };

char const * const senf::term::Terminfo::properties::StringNames[] = {
     "BackTab", "Bell", "CarriageReturn", "ChangeScrollRegion", "ClearAllTabs", "ClearScreen",
     "ClrEol", "ClrEos", "ColumnAddress", "CommandCharacter", "CursorAddress", "CursorDown",
     "CursorHome", "CursorInvisible", "CursorLeft", "CursorMemAddress", "CursorNormal",
     "CursorRight", "CursorToLl", "CursorUp", "CursorVisible", "DeleteCharacter", "DeleteLine",
     "DisStatusLine", "DownHalfLine", "EnterAltCharsetMode", "EnterBlinkMode", "EnterBoldMode",
     "EnterCaMode", "EnterDeleteMode", "EnterDimMode", "EnterInsertMode", "EnterSecureMode",
     "EnterProtectedMode", "EnterReverseMode", "EnterStandoutMode", "EnterUnderlineMode",
     "EraseChars", "ExitAltCharsetMode", "ExitAttributeMode", "ExitCaMode", "ExitDeleteMode",
     "ExitInsertMode", "ExitStandoutMode", "ExitUnderlineMode", "FlashScreen", "FormFeed",
     "FromStatusLine", "Init1string", "Init2string", "Init3string", "InitFile", "InsertCharacter",
     "InsertLine", "InsertPadding", "KeyBackspace", "KeyCatab", "KeyClear", "KeyCtab", "KeyDc",
     "KeyDl", "KeyDown", "KeyEic", "KeyEol", "KeyEos", "KeyF0", "KeyF1", "KeyF10", "KeyF2", "KeyF3",
     "KeyF4", "KeyF5", "KeyF6", "KeyF7", "KeyF8", "KeyF9", "KeyHome", "KeyIc", "KeyIl", "KeyLeft",
     "KeyLl", "KeyNpage", "KeyPpage", "KeyRight", "KeySf", "KeySr", "KeyStab", "KeyUp",
     "KeypadLocal", "KeypadXmit", "LabF0", "LabF1", "LabF10", "LabF2", "LabF3", "LabF4", "LabF5",
     "LabF6", "LabF7", "LabF8", "LabF9", "MetaOff", "MetaOn", "Newline", "PadChar", "ParmDch",
     "ParmDeleteLine", "ParmDownCursor", "ParmIch", "ParmIndex", "ParmInsertLine", "ParmLeftCursor",
     "ParmRightCursor", "ParmRindex", "ParmUpCursor", "PkeyKey", "PkeyLocal", "PkeyXmit",
     "PrintScreen", "PrtrOff", "PrtrOn", "RepeatChar", "Reset1string", "Reset2string",
     "Reset3string", "ResetFile", "RestoreCursor", "RowAddress", "SaveCursor", "ScrollForward",
     "ScrollReverse", "SetAttributes", "SetTab", "SetWindow", "Tab", "ToStatusLine", "UnderlineChar",
     "UpHalfLine", "InitProg", "KeyA1", "KeyA3", "KeyB2", "KeyC1", "KeyC3", "PrtrNon", "CharPadding",
     "AcsChars", "PlabNorm", "KeyBtab", "EnterXonMode", "ExitXonMode", "EnterAmMode", "ExitAmMode",
     "XonCharacter", "XoffCharacter", "EnaAcs", "LabelOn", "LabelOff", "KeyBeg", "KeyCancel",
     "KeyClose", "KeyCommand", "KeyCopy", "KeyCreate", "KeyEnd", "KeyEnter", "KeyExit", "KeyFind",
     "KeyHelp", "KeyMark", "KeyMessage", "KeyMove", "KeyNext", "KeyOpen", "KeyOptions",
     "KeyPrevious", "KeyPrint", "KeyRedo", "KeyReference", "KeyRefresh", "KeyReplace", "KeyRestart",
     "KeyResume", "KeySave", "KeySuspend", "KeyUndo", "KeySbeg", "KeyScancel", "KeyScommand",
     "KeyScopy", "KeyScreate", "KeySdc", "KeySdl", "KeySelect", "KeySend", "KeySeol", "KeySexit",
     "KeySfind", "KeyShelp", "KeyShome", "KeySic", "KeySleft", "KeySmessage", "KeySmove", "KeySnext",
     "KeySoptions", "KeySprevious", "KeySprint", "KeySredo", "KeySreplace", "KeySright", "KeySrsume",
     "KeySsave", "KeySsuspend", "KeySundo", "ReqForInput", "KeyF11", "KeyF12", "KeyF13", "KeyF14",
     "KeyF15", "KeyF16", "KeyF17", "KeyF18", "KeyF19", "KeyF20", "KeyF21", "KeyF22", "KeyF23",
     "KeyF24", "KeyF25", "KeyF26", "KeyF27", "KeyF28", "KeyF29", "KeyF30", "KeyF31", "KeyF32",
     "KeyF33", "KeyF34", "KeyF35", "KeyF36", "KeyF37", "KeyF38", "KeyF39", "KeyF40", "KeyF41",
     "KeyF42", "KeyF43", "KeyF44", "KeyF45", "KeyF46", "KeyF47", "KeyF48", "KeyF49", "KeyF50",
     "KeyF51", "KeyF52", "KeyF53", "KeyF54", "KeyF55", "KeyF56", "KeyF57", "KeyF58", "KeyF59",
     "KeyF60", "KeyF61", "KeyF62", "KeyF63", "ClrBol", "ClearMargins", "SetLeftMargin",
     "SetRightMargin", "LabelFormat", "SetClock", "DisplayClock", "RemoveClock", "CreateWindow",
     "GotoWindow", "Hangup", "DialPhone", "QuickDial", "Tone", "Pulse", "FlashHook", "FixedPause",
     "WaitTone", "User0", "User1", "User2", "User3", "User4", "User5", "User6", "User7", "User8",
     "User9", "OrigPair", "OrigColors", "InitializeColor", "InitializePair", "SetColorPair",
     "SetForeground", "SetBackground", "ChangeCharPitch", "ChangeLinePitch", "ChangeResHorz",
     "ChangeResVert", "DefineChar", "EnterDoublewideMode", "EnterDraftQuality", "EnterItalicsMode",
     "EnterLeftwardMode", "EnterMicroMode", "EnterNearLetterQuality", "EnterNormalQuality",
     "EnterShadowMode", "EnterSubscriptMode", "EnterSuperscriptMode", "EnterUpwardMode",
     "ExitDoublewideMode", "ExitItalicsMode", "ExitLeftwardMode", "ExitMicroMode", "ExitShadowMode",
     "ExitSubscriptMode", "ExitSuperscriptMode", "ExitUpwardMode", "MicroColumnAddress", "MicroDown",
     "MicroLeft", "MicroRight", "MicroRowAddress", "MicroUp", "OrderOfPins", "ParmDownMicro",
     "ParmLeftMicro", "ParmRightMicro", "ParmUpMicro", "SelectCharSet", "SetBottomMargin",
     "SetBottomMarginParm", "SetLeftMarginParm", "SetRightMarginParm", "SetTopMargin",
     "SetTopMarginParm", "StartBitImage", "StartCharSetDef", "StopBitImage", "StopCharSetDef",
     "SubscriptCharacters", "SuperscriptCharacters", "TheseCauseCr", "ZeroMotion", "CharSetNames",
     "KeyMouse", "MouseInfo", "ReqMousePos", "GetMouse", "SetAForeground", "SetABackground",
     "PkeyPlab", "DeviceType", "CodeSetInit", "Set0DesSeq", "Set1DesSeq", "Set2DesSeq", "Set3DesSeq",
     "SetLrMargin", "SetTbMargin", "BitImageRepeat", "BitImageNewline", "BitImageCarriageReturn",
     "ColorNames", "DefineBitImageRegion", "EndBitImageRegion", "SetColorBand", "SetPageLength",
     "DisplayPcChar", "EnterPcCharsetMode", "ExitPcCharsetMode", "EnterScancodeMode",
     "ExitScancodeMode", "PcTermOptions", "ScancodeEscape", "AltScancodeEsc",
     "EnterHorizontalHlMode", "EnterLeftHlMode", "EnterLowHlMode", "EnterRightHlMode",
     "EnterTopHlMode", "EnterVerticalHlMode", "SetAAttributes", "SetPglenInch", "TermcapInit2",
     "TermcapReset", "LinefeedIfNotLf", "BackspaceIfNotBs", "OtherNonFunctionKeys", "ArrowKeyMap",
     "AcsUlcorner", "AcsLlcorner", "AcsUrcorner", "AcsLrcorner", "AcsLtee", "AcsRtee", "AcsBtee",
     "AcsTtee", "AcsHline", "AcsVline", "AcsPlus", "MemoryLock", "MemoryUnlock", "BoxChars1" };

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::term::Terminfo

prefix_ senf::term::Terminfo::InvalidTerminfoException::InvalidTerminfoException(std::string const & term)
    : senf::Exception("Unreadable terminfo file")
{
    if (!term.empty())
        append( ": " + term);
}

prefix_ senf::term::Terminfo::Terminfo()
{}

prefix_ senf::term::Terminfo::Terminfo(std::string const & term)
{
    load(term);
}

prefix_ void senf::term::Terminfo::load(std::string const & term)
{
    std::string filename (findTerminfo(term));
    if (filename.empty()) throw InvalidTerminfoException(term);
    std::ifstream is (filename.c_str());
    if (!is) throw InvalidTerminfoException(filename);
    try {
        load(is);
    } catch (InvalidTerminfoException & ex) {
        ex << ": " << filename;
        throw;
    }
}

prefix_ bool senf::term::Terminfo::getFlag(properties::Boolean p)
    const
{
    if (BoolVec::size_type(p) >= booleans_.size())
        return false;
    return booleans_[p];
}

prefix_ senf::term::Terminfo::number_t senf::term::Terminfo::getNumber(properties::Numeric p)
    const
{
    if (NumberVec::size_type(p) >= numbers_.size())
        return NoValue;
    return numbers_[p];
}

prefix_ senf::term::Terminfo::string_t senf::term::Terminfo::getString(properties::String p)
    const
{
    if (StringVec::size_type(p) >= strings_.size())
        return 0;
    return strings_[p];
}

prefix_ bool senf::term::Terminfo::hasProperty(properties::Boolean p)
    const
{
    return getFlag(p);
}

prefix_ bool senf::term::Terminfo::hasProperty(properties::Numeric p)
    const
{
    return getNumber(p) != NoValue;
}

prefix_ bool senf::term::Terminfo::hasProperty(properties::String p)
    const
{
    return getString(p) != 0;
}

namespace {

    struct Stack
    {
        std::vector<senf::term::Terminfo::number_t> stack;

        void push(senf::term::Terminfo::number_t v)
            {
                stack.push_back(v);
            }

        senf::term::Terminfo::number_t pop()
            {
                if (stack.empty())
                    return 0;
                else {
                    senf::term::Terminfo::number_t v (stack.back());
                    stack.pop_back();
                    return v;
                }
            }

        senf::term::Terminfo::number_t popNonzero()
            {
                senf::term::Terminfo::number_t v (pop());
                return v ? v : 1;
            }
    };

}

// The following code is taken directly from utio. As far as I understand it is buggy
// and/or only partially implements the string format language. But seems to be enough for
// all the common terminal types ...
prefix_ std::string senf::term::Terminfo::formatString(properties::String p,
                                                       number_t arg1, number_t arg2,
                                                       number_t arg3, number_t arg4,
                                                       number_t arg5, number_t arg6,
                                                       number_t arg7, number_t arg8,
                                                       number_t arg9)
    const
{
    char const * fmt_p (getString(p));
    if (! fmt_p)
        return "";

    std::string const prgstr (fmt_p);
    Stack stack;
    bool bCondValue (false);
    std::string result;

    for (std::string::const_iterator i (prgstr.begin()); i != prgstr.end(); ++i) {
    if (*i != '%') {
        result += *i;
        continue;
    }
    int width = 0, base = 0;
    switch (*++i) {
        case '%': result += *i; break;
        case 'i': ++arg1; ++arg2; break;
        case 'c': result += char(stack.pop());  break;
        case 'x': base = 16; continue;
        case '0': if (!base) base = 8;
        case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8':
        case '9': if (!base) base = 10;
            width = width * base + (*i - '0');
            continue;
        case '\\': base = 0;
        case '{': continue;
        case '\'': if (*(i - 1) == '%') {
            if (*(i + 1) != '\\')
                width = *++i;
            continue;
        }
        case '}': stack.push(width); break;
        // Binary operands are in infix (reversed) order
        case '+': stack.push(stack.pop() + stack.pop()); break;
        case '-': stack.push(-stack.pop() + stack.pop()); break;
        case '*': stack.push(stack.pop() * stack.pop()); break;
        case '/': stack.push(stack.pop() / stack.popNonzero()); break;
        case 'm': stack.push(stack.pop() % stack.popNonzero()); break;
        case '|': stack.push(stack.pop() | stack.pop()); break;
        case '&': stack.push(stack.pop() & stack.pop()); break;
        case '^': stack.push(stack.pop() ^ stack.pop()); break;
        case '>': stack.push(stack.pop() < stack.pop()); break;
        case '<': stack.push(stack.pop() > stack.pop()); break;
        case '=': stack.push(stack.pop() == stack.pop()); break;
        case 'A': stack.push(stack.pop() && stack.pop()); break;
        case 'O': stack.push(stack.pop() || stack.pop()); break;
        case '!': stack.push(!stack.pop()); break;
        case '~': stack.push(~stack.pop()); break;
        case 't': bCondValue = stack.pop();
        case 'e': if ((bCondValue = !bCondValue)) // this also supports elsif
            --(i = prgstr.begin() + std::min (prgstr.find ("%e", i-prgstr.begin()),
                                              prgstr.find ("%;", i-prgstr.begin())));
        case '?':
        case ';': break;
        case 'p':
            switch (*++i) {
            case '1': stack.push(arg1); break;
            case '2': stack.push(arg2); break;
            case '3': stack.push(arg3); break;
            case '4': stack.push(arg4); break;
            case '5': stack.push(arg5); break;
            case '6': stack.push(arg6); break;
            case '7': stack.push(arg7); break;
            case '8': stack.push(arg8); break;
            case '9': stack.push(arg9); break;
            }
            break;
        case 'd': {
            number_t n = stack.pop();
            const std::string::size_type iSize = result.size();
            do {
                result += std::string::value_type('0' + (n % 10));
            } while ((n /= 10) || --width > 0);
            reverse (result.begin() + iSize, result.end());
            break; }
        }
    }

    return result;
}

 prefix_ void senf::term::Terminfo::dump(std::ostream & os)
     const
 {
     os << "Terminfo entry: " << name_ << "\n";
     os << "Booleans: " << booleans_.size() << "\n";
     os << "Numbers: " << numbers_.size() << "\n";
     os << "Strings: " << strings_.size() << "\n";
     os << "String pool size: " << stringPool_.size() << "\n";

     {
         os << "Flags:\n";
         unsigned n (0);
         BoolVec::const_iterator i (booleans_.begin());
         BoolVec::const_iterator const i_end (booleans_.end());
         for (; i != i_end; ++i, ++n)
             if (*i && n < sizeof(properties::BooleanNames)/sizeof(properties::BooleanNames[0]))
                 os << "    " << properties::BooleanNames[n] << "\n";
     }

     {
         os << "Numbers:\n";
         unsigned n (0);
         NumberVec::const_iterator i (numbers_.begin());
         NumberVec::const_iterator const i_end (numbers_.end());
         for (; i != i_end; ++i, ++n)
             if (*i != NoValue
                 && n < sizeof(properties::NumericNames)/sizeof(properties::NumericNames[0]))
                 os << "    " << properties::NumericNames[n] << " = " << *i << "\n";
     }

     {
         os << "Strings:\n";
         unsigned n (0);
         StringVec::const_iterator i (strings_.begin());
         StringVec::const_iterator const i_end (strings_.end());
         for (; i != i_end; ++i, ++n)
             if (*i && n < sizeof(properties::StringNames)/sizeof(properties::StringNames[0])) {
                 os << "    " << std::setw(32) << properties::StringNames[n] << " = ";
                 hexdump(*i, *i + strlen(*i), os, 32);
             }
     }

}

prefix_ std::string senf::term::Terminfo::findTerminfo(std::string const & name)
{
    if (name.empty()) return "";
    boost::filesystem::path subdir (name.substr(0,1)); subdir /= name;
    boost::filesystem::path tientry;

    {
        char const * tivar (::getenv("TERMINFO"));
        if (tivar) {
            tientry = boost::filesystem::path(tivar) / subdir;
            if (boost::filesystem::exists(tientry)) return tientry.string();
        }
    }

    tientry = boost::filesystem::path("/etc/terminfo") / subdir;
    if (boost::filesystem::exists(tientry)) return tientry.string();

    tientry = boost::filesystem::path("/lib/terminfo")  / subdir;
    if (boost::filesystem::exists(tientry)) return tientry.string();

    tientry = boost::filesystem::path("/usr/share/terminfo") / subdir;
    if (boost::filesystem::exists(tientry)) return tientry.string();

    return "";
}

namespace {

    boost::uint16_t const TerminfoMagic = 0x011A;

    struct TerminfoHeader {
        boost::uint16_t magic;
        boost::uint16_t namesSz;
        boost::uint16_t nBooleans;
        boost::uint16_t nNumbers;
        boost::uint16_t nStrings;
        boost::uint16_t stringPoolSz;

        void letoh() {
            magic = le16toh(magic);
            namesSz = le16toh(namesSz);
            nBooleans = le16toh(nBooleans);
            nNumbers = le16toh(nNumbers);
            nStrings = le16toh(nStrings);
            stringPoolSz = le16toh(stringPoolSz);
        }
    };

}

prefix_ void senf::term::Terminfo::load(std::istream & is)
{
    TerminfoHeader h;
    is.read(static_cast<char*>(static_cast<void*>(&h)), sizeof(h));
    h.letoh();
    if (!is || h.magic != TerminfoMagic) throw InvalidTerminfoException(
            "invalid magic number (") << h.magic << "!=" << TerminfoMagic << ")";

    name_.resize(h.namesSz);
    is.read(&(name_[0]), name_.size());
    if (!is) throw InvalidTerminfoException();
    if (name_.size() & 1)
        is.ignore(1u);
    {
        std::string::size_type n (name_.find('\0'));
        if (n != std::string::npos)
            name_.erase(n);
    }

    booleans_.resize(h.nBooleans);
    for (BoolVec::iterator i (booleans_.begin()); i != booleans_.end(); ++i) {
        char v;
        is.read(&v, sizeof(v));
        if (!is) throw InvalidTerminfoException();
        *i = v;
    }
    if (booleans_.size() & 1)
        is.ignore(1u);

    numbers_.resize(h.nNumbers);
    for (NumberVec::iterator i (numbers_.begin()); i != numbers_.end(); ++i) {
        number_t v;
        is.read(static_cast<char*>(static_cast<void*>(&v)), sizeof(v));
        if (!is) throw InvalidTerminfoException();
        *i = v;
    }

    typedef std::vector<number_t> OffsetVec;
    OffsetVec offsets;
    offsets.resize (h.nStrings);
    for (OffsetVec::iterator i (offsets.begin()); i != offsets.end(); ++i) {
        number_t v;
        is.read(static_cast<char*>(static_cast<void*>(&v)), sizeof(v));
        if (!is) throw InvalidTerminfoException();
        *i = le16toh(v);
    }

    stringPool_.resize(h.stringPoolSz);
    is.read(&(stringPool_[0]), stringPool_.size());
    if (!is) throw InvalidTerminfoException();

    strings_.resize(offsets.size());
    StringVec::iterator j (strings_.begin());
    for (OffsetVec::iterator i (offsets.begin()); i != offsets.end(); ++i, ++j)
        if (*i != NoValue && *i >= 0 && unsigned(*i) < stringPool_.size())
            *j = &(stringPool_[0]) + *i;
        else
            *j = 0;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::term::KeyParser

char const * const senf::term::KeyParser::KeyNames[] = {
    "Esc", "Backspace", "Backtab", "Begin", "CATab", "CTab", "Cancel", "Center", "Clear",
    "ClearToEOL", "ClearToEOS", "Close", "Command", "Copy", "Create", "Delete", "DeleteLine",
    "Down", "DownLeft", "DownRight", "End", "Enter", "Exit", "F0", "F1", "F2", "F3", "F4", "F5",
    "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "F16", "F17", "F18", "F19",
    "F20", "F21", "F22", "F23", "F24", "F25", "F26", "F27", "F28", "F29", "F30", "F31", "F32",
    "F33", "F34", "F35", "F36", "F37", "F38", "F39", "F40", "F41", "F42", "F43", "F44", "F45",
    "F46", "F47", "F48", "F49", "F50", "F51", "F52", "F53", "F54", "F55", "F56", "F57", "F58",
    "F59", "F60", "F61", "F62", "F63", "Find", "Help", "Home", "Insert", "InsertLine", "Left",
    "Mark", "Message", "Mouse", "Move", "Next", "Open", "Options", "PageDown", "PageUp", "Previous",
    "Print", "Redo", "Reference", "Refresh", "Replace", "Restart", "Resume", "Right", "Save",
    "Select", "ShiftBegin", "ShiftCancel", "ShiftCommand", "ShiftCopy", "ShiftCreate",
    "ShiftDelete", "ShiftDeleteLine", "ShiftEnd", "ShiftClearToEOL", "ShiftExit", "ShiftFind",
    "ShiftHelp", "ShiftHome", "ShiftInsert", "ShiftLeft", "ShiftMessage", "ShiftMove", "ShiftNext",
    "ShiftOptions", "ShiftPrevious", "ShiftPrint", "ShiftRedo", "ShiftReplace", "ShiftResume",
    "ShiftRight", "ShiftSave", "ShiftSuspend", "ShiftTab", "ShiftUndo", "Suspend", "Undo", "Up",
    "UpLeft", "UpRight" };

prefix_ senf::term::KeyParser::KeyParser()
{}

prefix_ senf::term::KeyParser::KeyParser(Terminfo const & ti)
{
    load(ti);
}

prefix_ void senf::term::KeyParser::load(Terminfo const & ti)
{
    static Terminfo::properties::String keyStrings [] = {
        Terminfo::properties::KeyCommand, Terminfo::properties::KeyBackspace,
        Terminfo::properties::KeyBtab, Terminfo::properties::KeyBeg, Terminfo::properties::KeyCatab,
        Terminfo::properties::KeyCtab, Terminfo::properties::KeyCancel, Terminfo::properties::KeyB2,
        Terminfo::properties::KeyClear, Terminfo::properties::KeyEol, Terminfo::properties::KeyEos,
        Terminfo::properties::KeyClose, Terminfo::properties::KeyCommand,
        Terminfo::properties::KeyCopy, Terminfo::properties::KeyCreate, Terminfo::properties::KeyDc,
        Terminfo::properties::KeyDl, Terminfo::properties::KeyDown, Terminfo::properties::KeyC1,
        Terminfo::properties::KeyC3, Terminfo::properties::KeyEnd, Terminfo::properties::KeyEnter,
        Terminfo::properties::KeyExit, Terminfo::properties::KeyF0, Terminfo::properties::KeyF1,
        Terminfo::properties::KeyF2, Terminfo::properties::KeyF3, Terminfo::properties::KeyF4,
        Terminfo::properties::KeyF5, Terminfo::properties::KeyF6, Terminfo::properties::KeyF7,
        Terminfo::properties::KeyF8, Terminfo::properties::KeyF9, Terminfo::properties::KeyF10,
        Terminfo::properties::KeyF11, Terminfo::properties::KeyF12, Terminfo::properties::KeyF13,
        Terminfo::properties::KeyF14, Terminfo::properties::KeyF15, Terminfo::properties::KeyF16,
        Terminfo::properties::KeyF17, Terminfo::properties::KeyF18, Terminfo::properties::KeyF19,
        Terminfo::properties::KeyF20, Terminfo::properties::KeyF21, Terminfo::properties::KeyF22,
        Terminfo::properties::KeyF23, Terminfo::properties::KeyF24, Terminfo::properties::KeyF25,
        Terminfo::properties::KeyF26, Terminfo::properties::KeyF27, Terminfo::properties::KeyF28,
        Terminfo::properties::KeyF29, Terminfo::properties::KeyF30, Terminfo::properties::KeyF31,
        Terminfo::properties::KeyF32, Terminfo::properties::KeyF33, Terminfo::properties::KeyF34,
        Terminfo::properties::KeyF35, Terminfo::properties::KeyF36, Terminfo::properties::KeyF37,
        Terminfo::properties::KeyF38, Terminfo::properties::KeyF39, Terminfo::properties::KeyF40,
        Terminfo::properties::KeyF41, Terminfo::properties::KeyF42, Terminfo::properties::KeyF43,
        Terminfo::properties::KeyF44, Terminfo::properties::KeyF45, Terminfo::properties::KeyF46,
        Terminfo::properties::KeyF47, Terminfo::properties::KeyF48, Terminfo::properties::KeyF49,
        Terminfo::properties::KeyF50, Terminfo::properties::KeyF51, Terminfo::properties::KeyF52,
        Terminfo::properties::KeyF53, Terminfo::properties::KeyF54, Terminfo::properties::KeyF55,
        Terminfo::properties::KeyF56, Terminfo::properties::KeyF57, Terminfo::properties::KeyF58,
        Terminfo::properties::KeyF59, Terminfo::properties::KeyF60, Terminfo::properties::KeyF61,
        Terminfo::properties::KeyF62, Terminfo::properties::KeyF63, Terminfo::properties::KeyFind,
        Terminfo::properties::KeyHelp, Terminfo::properties::KeyHome, Terminfo::properties::KeyIc,
        Terminfo::properties::KeyIl, Terminfo::properties::KeyLeft, Terminfo::properties::KeyMark,
        Terminfo::properties::KeyMessage, Terminfo::properties::KeyMouse,
        Terminfo::properties::KeyMove, Terminfo::properties::KeyNext, Terminfo::properties::KeyOpen,
        Terminfo::properties::KeyOptions, Terminfo::properties::KeyNpage,
        Terminfo::properties::KeyPpage, Terminfo::properties::KeyPrevious,
        Terminfo::properties::KeyPrint, Terminfo::properties::KeyRedo,
        Terminfo::properties::KeyReference, Terminfo::properties::KeyRefresh,
        Terminfo::properties::KeyReplace, Terminfo::properties::KeyRestart,
        Terminfo::properties::KeyResume, Terminfo::properties::KeyRight,
        Terminfo::properties::KeySave, Terminfo::properties::KeySelect,
        Terminfo::properties::KeySbeg, Terminfo::properties::KeyScancel,
        Terminfo::properties::KeyScommand, Terminfo::properties::KeyScopy,
        Terminfo::properties::KeyScreate, Terminfo::properties::KeySdc,
        Terminfo::properties::KeySdl, Terminfo::properties::KeySend, Terminfo::properties::KeySeol,
        Terminfo::properties::KeySexit, Terminfo::properties::KeySfind,
        Terminfo::properties::KeyShelp, Terminfo::properties::KeyShome,
        Terminfo::properties::KeySic, Terminfo::properties::KeySleft,
        Terminfo::properties::KeySmessage, Terminfo::properties::KeySmove,
        Terminfo::properties::KeySnext, Terminfo::properties::KeySoptions,
        Terminfo::properties::KeySprevious, Terminfo::properties::KeySprint,
        Terminfo::properties::KeySredo, Terminfo::properties::KeySreplace,
        Terminfo::properties::KeySrsume, Terminfo::properties::KeySright,
        Terminfo::properties::KeySsave, Terminfo::properties::KeySsuspend,
        Terminfo::properties::KeyStab, Terminfo::properties::KeySundo,
        Terminfo::properties::KeySuspend, Terminfo::properties::KeyUndo,
        Terminfo::properties::KeyUp, Terminfo::properties::KeyA1, Terminfo::properties::KeyA3 };

    table_.clear();
    for (unsigned i (0); i < sizeof(keyStrings)/sizeof(keyStrings[0]); ++i) {
        char const * key (ti.getString(keyStrings[i]));
        if (key)
            table_.insert(std::make_pair(key, KeyCode(i+First)));
    }
}

prefix_ std::pair<senf::term::KeyParser::keycode_t, std::string::size_type>
senf::term::KeyParser::lookup(std::string const & key)
    const
{
    if (key.empty())
        return std::make_pair(KeyCode(0), 0);

    // There are several cases:
    // a) 'key' is an incomplete key sequence. In this case, 'key' will precede all completions in
    //    the key table. The first possible completion is found by 'upper_bound'
    // b) 'key' is a complete key sequence. This is the key sequence *preceding* the 'upper_bound'
    // c) 'key' is a complete key sequence with additional trailing characters. In this case, 'key'
    //    will follow the correct entry in the key table. Again, the correct key sequence is
    //    the one preceding the 'upper_bound'

    Keytable::const_iterator i (table_.upper_bound(key));
    if (i != table_.end() && i->first.substr(0, key.size()) == key)
        return std::make_pair(Incomplete, key.size());
    if (i == table_.begin())
        return std::make_pair(keycode_t(key[0]), 1);
    --i;
    if (key.substr(0, i->first.size()) == i->first)
        return std::make_pair(i->second, i->first.size());
    return std::make_pair(keycode_t(key[0]), 1);
}

prefix_ std::string senf::term::KeyParser::describe(keycode_t key)
{
    if (key < keycode_t(' '))
        return "^" + std::string(1, '@' + key);
    if (key < 256)
        return std::string(1, char(key));
    if (key >= keycode_t(First) && key < keycode_t(First + sizeof(KeyNames) / sizeof(KeyNames[0])))
        return std::string(KeyNames[key-First]);
    else
        return "<" + boost::lexical_cast<std::string>(unsigned(key)) + ">";
}

prefix_ void senf::term::KeyParser::dump(std::ostream & os)
    const
{
    os << "Keytable:\n";
    for (Keytable::const_iterator i (table_.begin()); i != table_.end(); ++i) {
        unsigned index (i->second - First);
        if (index < sizeof(KeyNames)/sizeof(KeyNames[0])) {
            os << "    " << std::setw(32) << KeyNames[index] << ": ";
            hexdump(i->first.begin(), i->first.end(), os);
        }
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "Terminfo.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
