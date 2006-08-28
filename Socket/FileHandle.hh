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

#ifndef HH_FileHandle_
#define HH_FileHandle_ 1

// Custom includes
#include <memory> // std::auto_ptr
#include <boost/intrusive_ptr.hpp>
#include "Utils/intrusive_refcount.hh"

//#include "FileHandle.mpp"
///////////////////////////////hh.p////////////////////////////////////////
#include "FileHandle.ih"

namespace satcom {
namespace lib {
    
    /** \brief
     */
    class FileHandle
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // protected default constructor
        // default copy constructor
        // default copy assignment
        // default destructor

        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////
        
        void close();
        void terminate();
        void flush();

        bool readable() const;
        void waitReadable() const;
        bool writeable() const;
        void waitWritable() const;

        bool blocking() const;
        bool blocking(bool status);

        bool eof() const;
        bool valid() const;

        operator bool () const;
        bool operator!() const;

        int fd() const;

    protected:
        explicit FileHandle(std::auto_ptr<FileBody> body);

        FileBody & body();
        FileBody const & body() const;

        int fd(int fd);

    private:
        FileBody::ptr body_;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "FileHandle.cci"
//#include "FileHandle.ct"
//#include "FileHandle.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
