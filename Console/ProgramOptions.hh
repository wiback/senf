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
    \brief ProgramOptions public header */

#ifndef HH_ProgramOptions_
#define HH_ProgramOptions_ 1

// Custom includes
#include "Config.hh"


//#include "ProgramOptions.mpp"
#include "ProgramOptions.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    class ProgramOptions
        : public detail::BundleMixin
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{
        
        ProgramOptions(int argc, char ** argv, DirectoryNode & root = root());

        ///@}
        ///////////////////////////////////////////////////////////////////////////
        
        template <class Container>
        ProgramOptions & nonOptions(Container & container);
        ProgramOptions & alias(char letter, std::string const & longOpt, bool withArg=false);

    private:
        detail::ProgramOptionsSource & config_;
    };

    void parseOptions(int argc, char ** argv, DirectoryNode & root = root());

    detail::ProgramOptionsSource::ptr OptionsConfig(int argc, char ** argv);
}}

///////////////////////////////hh.e////////////////////////////////////////
#include "ProgramOptions.cci"
//#include "ProgramOptions.ct"
#include "ProgramOptions.cti"
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
