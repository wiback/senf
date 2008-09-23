// $Id$
//
// Copyright (C) 2006
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
    \brief Exception public header */

#ifndef HH_Exception_
#define HH_Exception_ 1

// Custom includes
#include <exception>
#include <string>
#include <iostream>
#include <sstream>
#include <boost/preprocessor/repeat.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/utility.hpp>
#include <boost/type_traits/is_convertible.hpp>

//#include "Exception.mpp"
///////////////////////////////hh.p////////////////////////////////////////

/** \defgroup exception Exception classes

    All exceptions in %senf are derived from senf::Exception. This class adds the possibility to
    extend the exception description while it is processed. The functionality is provided by a mixin
    class senf::ExceptionMixin:

    \code
    try {

        // Some code which might raise an arbitrary senf exception

    }
    catch (senf::ExceptionMixin & e) {
        e << "\handling user " << user;
        throw;
    }
    \endcode

    This will add the user information to any %senf exception thrown. The Exception is however not a
    stream. If you need to do more extensive formating, either use an intermediate string-stream or
    use <a href="http://www.boost.org/libs/format/doc/format.html">Boost.Format</a>:

    \code
    try { 
        // ...
    }
    catch (senf::ExceptionMixin & e) {
        e << boost::format("\n" "call id 0x%04x@%s") % id % address;
    }
    \endcode

    senf::SystemException is thrown for all operating system errors (failures which result in the
    operating system setting the errno value). It is also derived from senf::Exception and can
    therefore be extended as well.

    Defining your own exception classes derived from senf::Exception is very simple:

    \code
    struct FooException : public senf::Exception
    { FooException() : senf::Exception("Foo hit the fan") {} };
    \endcode

    If SENF is compiled in debug mode (SENF_DEBUG is defined), the exception message will
    automatically include a stack backtrace. For this to work, you need to add the
    <tt>-rdynamic</tt> option to all link commands. This feature depends on <tt>gcc</tt> and
    the GNU-libc.

    To apply these features (extensibility, backtrace) to a non-senf exception, the non-senf
    exception can be wrapped and re-thrown.
    \code
    void foo() {
        try {
            // ... code that might throw std::bad_cast or somelib::FooException
        }
        SENF_WRAP_EXC(std::bad_cast)
        SENF_WRAP_EXC(somelib::FooException)
    }
    \endcode 
    The re-thrown exception can then be caught as <tt>std::bad_cast</tt> or as senf::ExceptionMixin
    as needed. It is safe, to wrap an exception twice (the macro will detect this case).
    \code
    bar() {
    try {
        try {
            foo();
        }
        catch (senf::ExceptionMixin & ex) {
            ex << "\n" "add this info";
        }
    }
    catch (std::bad_cast const & ex) {
        std::cerr << ex.what() << std::endl;
    }
    \endcode
    The final error output will include
    \li a backtrace if compiled in debug mode
    \li the original error message from the <tt>std::bad_cast</tt> exception
    \li the additional error message "add this info"

    \todo Link against libcwd to add file-name/line-number information to the backtrace and remove
        the dependency on -rdynamic
    \todo Or better, use addr2line to obtain that information when showing the backtrace when
        catched within Daemon (<tt>addr2line -fsiCe argv[0]</tt>)
    \todo Add signal handlers for the bad signals which writes a backtrace to stderr and
        terminates. This should probably write out a raw backtrace without de-mangling or
        line-numbers since we don't want to mess with dynamic memory when the heap might be
        corrupted ... Another handler for e.g. SIGUSR2 is nice to show a debug backtrace on demand
 */

namespace senf {

    /** \brief Generic extensible exception mixin

        ExceptionMixin is a generic exception mixin which allows the exception to be later extended
        by catching and re-throwing it (See example in \ref exception).

        \ingroup exception
      */
    class ExceptionMixin
    {
    public:
        std::string const & message() const;

        void append(std::string text); ///< Extend exception description
                                        /**< Adds \a text to the description text. */

    protected:
        explicit ExceptionMixin(std::string const & description = ""); 
                                        ///< Initialize exception with string
                                        /**< \a description is the initial error description
                                             string. This should probably be a string constant
                                             describing the exception for most derived
                                             exceptions. */

    private:
#ifdef SENF_DEBUG
        void addBacktrace();
#endif
        std::string message_;
    };

    /** \brief Extensible exception base-class

        This base-class is an exception which already includes the ExceptionMixin. All SENF
        exceptions are derived from this class. Other user-exception may be defined by deriving from
        this class too.

        \see \ref exception

        \ingroup exception
     */
    class Exception
        : public ExceptionMixin, public std::exception
    {
    public:
        virtual ~Exception() throw();

        virtual char const * what() const throw();

    protected:
        explicit Exception(std::string const & description = "");
    };
    
    /** \brief Wrapper for standard non-senf exceptions

        This class wraps an exception of type \a BaseException and adds functionality from
        senf::ExceptionMixin.

        \ingroup exception
     */
    template <class BaseException>
    class WrapException
        : public ExceptionMixin, public BaseException
    {
    public:
        typedef BaseException Base;

        WrapException(BaseException const & base);
        virtual ~WrapException() throw();

        virtual char const * what() const throw();
    };

    /** \brief Wrap a non-senf exception

        This macro allows to wrap a non-senf exception adding functionality from ExceptionMixin
        using the WrapException template. For an example, see \ref exception.

        \ingroup exception
     */
#   define SENF_WRAP_EXC(Ex)                                                                      \
        catch (Ex const & base) {                                                                 \
            if (dynamic_cast<senf::ExceptionMixin const *>(&base))                                \
                throw;                                                                            \
            else                                                                                  \
                throw senf::WrapException<Ex>(base);                                              \
        }

    template <class Exc, class Arg>
    typename boost::enable_if< boost::is_convertible<Exc*,ExceptionMixin*>, Exc & >::type
    operator<<(Exc const & exc, Arg const & arg); ///< Extend exception description
                                        /**< Adds \a arg converted to string to the end of the
                                             exception description string. This operator allows to
                                             use Exception instances like streams. The conversion is
                                             performed using <code>boost::lexical_cast</code> and is
                                             therefor identical to a streaming operation. 
                                             \see \ref exception */


#   ifdef SENF_DEBUG
#       define _SENF_EXC_DEBUG_ARGS ,char const * file=0,int line=0
#       define _SENF_EXC_DEBUG_ARGS_ND ,char const *file,int line
#       define _SENF_EXC_DEBUG_ARGS_P ,file,line
#   else
#       define _SENF_EXC_DEBUG_ARGS
#       define _SENF_EXC_DEBUG_ARGS_ND
#       define _SENF_EXC_DEBUG_ARGS_P
#   endif

    /** \brief Exception handling standard UNIX errors (errno)

        This exception is thrown to signal generic \c errno failures. Normally the \c errno value is
        automatically taken from the \c errno variable but it may also be specified explicitly:

        \code
        // Standard usage: Take \c errno from environment
        throw senf::SystemException("::open()") 
            << " while opening configuration file: " << filename;

        // You may however explicitly specify the errno value
        throw senf::SystemException("::open()", ENOFILE)

        // Or leave the location information empty
        throw senf::SystemException(ENOFILE);
        throw senf::SystemException();
        \endcode

        From within SENF (<em>and only there because it depends on the \c SENF_DEBUG symbol</em>),
        SystemException should be thrown using wrapper macros which add additional information to
        the exception description:
        \code
        // Standard usage: Take \c errno from environment
        SENF_THROW_SYSTEM_EXCEPTION()
            << " while opening configuration file: " << filename;

        // You may however explicitly specify the errno value
        throw senf::SystemException("::open()", ENOFILE SENF_EXC_DEBUGINFO)
        \endcode

        \ingroup exception
     */
    class SystemException : public Exception
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        explicit SystemException(std::string const & descr = "" _SENF_EXC_DEBUG_ARGS);
        explicit SystemException(int code _SENF_EXC_DEBUG_ARGS);
        SystemException(std::string const & descr, int code _SENF_EXC_DEBUG_ARGS);

        virtual ~SystemException() throw();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        int errorNumber() const;        ///< Error code (\c errno number)
        char const * errorString() const; ///< Error string (\c strerror() value)

        bool anyOf(int c0, int c1=0, int c2=0, int c3=0, int c4=0, int c5=0, 
                   int c6=0, int c7=0, int c8=0, int c9=0);
                                        ///< \c true, if errorNumber() is one of \a c0 ... \a c9



    private:
        void init(std::string const & descr, int code _SENF_EXC_DEBUG_ARGS_ND);
        
        int code_;
        std::string what_;
    };

#   ifdef SENF_DEBUG
#       define SENF_EXC_DEBUGINFO ,__FILE__,__LINE__
#   else
#       define SENF_EXC_DEBUGINFO
#   endif

#   define SENF_THROW_SYSTEM_EXCEPTION(desc) throw SystemException(desc SENF_EXC_DEBUGINFO)

}

///////////////////////////////hh.e////////////////////////////////////////
#include "Exception.cci"
//#include "Exception.ct"
#include "Exception.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
 
