//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief IOStreamTarget public header */

#ifndef HH_SENF_Utils_Logger_IOStreamTarget_
#define HH_SENF_Utils_Logger_IOStreamTarget_ 1

// Custom includes
#include "Target.hh"
#include "LogFormat.hh"

//#include "IOStreamTarget.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace log {

    /** \brief Write %log messages to arbitrary std::ostream

        This target will write %log messages to an arbitrary std::ostream in the format
        \verbatim <date> [<level>][<area>] <message> \endverbatim

        The \e area will be omitted if it is \c senf::log::DefaultArea.

        \warning The class keeps a reference to the passed stream.

        \ingroup targets
      */
    class IOStreamTarget
        : public Target, private detail::LogFormat
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        IOStreamTarget(std::string const & name, std::ostream & os);

        //\}
        //-////////////////////////////////////////////////////////////////////////

        using detail::LogFormat::showTime;
        using detail::LogFormat::showStream;
        using detail::LogFormat::showLevel;
        using detail::LogFormat::showArea;
        using detail::LogFormat::timeFormat;
        using detail::LogFormat::tag;

        std::ostream & stream() const;
        void stream(std::ostream & newos);

    protected:
        void v_write(time_type timestamp, std::string const & stream,
                     std::string const & area, unsigned level,
                     std::string const & message);

    private:
        std::ostream * stream_;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "IOStreamTarget.cci"
//#include "IOStreamTarget.ct"
//#include "IOStreamTarget.cti"
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
