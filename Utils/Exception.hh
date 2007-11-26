// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

//#include "Exception.mpp"
///////////////////////////////hh.p////////////////////////////////////////

/** \defgroup exception System exceptions

    The senf::SystemException class and it's derived class template senf::ErrnoException are used to
    signal generic system failures based on \c errno codes. 

    senf::SystemException is a generic \c errno based exception which carries an error number and
    origin information. senf::ErrnoException is a derived class specialized for a specific error
    code. This simplifies managing error conditions:

    \code
    try {
        something.open(path);
        // ...
    }
    catch (senf::ErrnoException<ENOFILE> & e) {
        // Create new file
    }
    catch (senf::SystemException & e) {
        // Catch all other system exceptions
        std::cerr << "Error accessing '" << path << "': " << e.what() << std::endl;
    }
    \endcode

    This exception is normally thrown using the senf::throwErrno() helper:

    \code
    if ((fd = ::open(path.c_str(), O_RDWR)) < 0)
        senf::throwErrno("::open()");
    \endcode

    The senf::throwErrno() helper will throw the correct exception class based on some \c errno
    value.
 */

namespace senf {

    /** \brief Exception handling standard UNIX errors (errno)

        This exception is thrown to signal generic \c errno failures. 

        This exception cannot be thrown directly. Instead the derived class ErrnoException should be
        thrown via one of the senf::throwErrno helpers.

        The error message associated with the SystemException may be extended arbitrarily by using
        the exception like a stream:
        \code
        try {
            // This throw would normally be within some function called from here.
            senf::throwErrno("::open()");

            // Or you may want to use a more descriptive argument string:
            senf::throwErrno("::open(\"" + filename + "\")");

            // Or even use boost::format here
            senf::throwErrno((boost::format("::open(\"%s\")") % filename).str());
        }
        catch (SystemException & e) {
            // You can add further error information later by catching and re-throwing the exception
            e << " [while operating on user '" << user << "']";
            throw;
        }
        \endcode

        \see ErrnoException
        \ingroup exception
     */
    class SystemException : public std::exception, public std::stringstream
    {
    public:
        virtual char const * what() const throw(); ///< Return verbose error description

        int errorNumber() const;        ///< Error code (\c errno number)
        char const * description() const; ///< Error description (strerror() value)

        bool anyOf(int c0, int c1=0, int c2=0, int c3=0, int c4=0, int c5=0, 
                   int c6=0, int c7=0, int c8=0, int c9=0);
                                        ///< \c true, if errorNumber() is one of \a c0 ... \a c9

        virtual ~SystemException() throw();

    protected:
        SystemException(std::string const & where, int code); 
        SystemException(SystemException const & other);

    private:
        int const code_;                // This must be const to make the derived ErrnoException
                                        // class a valid derived class.
        mutable std::string buffer_;

        friend void throwErrno(std::string const &, int);
    };

    /** \brief Error specific system exception

        This template restricts the generic SystemException to a specific, compile-time constant
        error number \p Code. This allows a specific \c errno number to be cached explicitly.

        This exception is normally thrown via one of the senf::throwErrno() helpers. These helpers
        take the numeric \c errno value (either from the \c errno variable or from their
        argument) and will throw the corresponding ErrnoException:
        \code
        if ((fd = ::open(filename, O_RDWR)) < 0)
             senf::throwErrno("open()");
        \endcode

        \see SystemException

        \ingroup exception
     */
    template <int Code>
    class ErrnoException : public SystemException
    {
    public:
        static int const fixed_code = Code;

        explicit ErrnoException(std::string const & where);
                                        ///< ErrnoException with error location information
        
        ErrnoException(ErrnoException const & other);
    };

    
    /** \brief Throw ErrnoException based on current \c errno value
        \ingroup exception
     */
    void throwErrno();

    /** \brief Throw ErrnoException based on current \c errno value (with location info)
        \ingroup exception
     */
    void throwErrno(std::string const & where);

    /** \brief Throw ErrnoException based on given \c errno value
        \ingroup exception
     */
    void throwErrno(int code);

    /** \brief Throw ErrnoException based on given \c errno value (with location info)
        \ingroup exception
     */
    void throwErrno(std::string const & where, int code);

    enum NoThrow_t { nothrow };

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
