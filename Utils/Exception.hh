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
#include <boost/preprocessor/repeat.hpp>
#include <boost/preprocessor/cat.hpp>

//#include "Exception.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Exception handling standard UNIX errors (errno)

        This exception is thrown to signal generic errno failures.

        \todo make where and err accessors and make the member variables private

        \idea Add a template class derived from SystemException which takes the error number as a
        numeric argument. This allows catching specific errno conditions: ErrnoException<EPIPE> etc.

        \idea Add a generic error thrower which takes the origin string and errno value as an
        argument and will throw a corresponding template class instance. This would just be a big
        switch statement containing all possible errno values, probably created using some macro
        meta-programming.
     */
    class SystemException : public std::exception
    {
    public:
        SystemException();              ///< SystemException without error location infor
                                        /**< The error code is taken from the current value of the
                                             global 'errno' variable  */

        explicit SystemException(int code); ///< SystemException without error location info
                                        /**< \param[in] code error number (the errno value) */

        explicit SystemException(char const * where); ///< SystemException with error location info
                                        /**< The error code is taken from the current value of the
                                             global 'errno' variable 
                                             \param[in] where description of error origin */

        SystemException(char const * where, int code); ///< SystemException with error location info 
                                        /**< \param[in] where description of error origin
                                             \param[in] code error number (the errno value) */

        virtual char const * what() const throw(); ///< Return verbose error description

        char const * where() const;     ///< Error origin
        int code() const;               ///< Error code (errno number)
        char const * description() const; ///< Error description (strerror() value)

        bool anyOf(int c0, int c1=0, int c2=0, int c3=0, int c4=0, int c5=0, 
                   int c6=0, int c7=0, int c8=0, int c9=0);

        virtual ~SystemException() throw();

    private:
        void init();

        char const * const where_;
        int const code_;
        std::string buffer_;
    };

    template <int Code>
    class ErrnoException : public SystemException
    {
    public:
        static int const fixed_code = Code;

        ErrnoException();
        explicit ErrnoException(char const * where);
    };

    void throwErrno();
    void throwErrno(char const * where);
    void throwErrno(int code);
    void throwErrno(char const * where, int code);

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
