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
#include "Utils/SafeBool.hh"

//#include "FileHandle.mpp"
///////////////////////////////hh.p////////////////////////////////////////
#include "FileHandle.ih"

namespace senf {

    
    /** \brief
     */
    class FileHandle
	: public SafeBool<FileHandle>
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

        bool readable() const;
        void waitReadable() const;
        bool writeable() const;
        void waitWriteable() const;

        bool blocking() const;
        void blocking(bool status);

        bool eof() const;
        bool valid() const;

	bool boolean_test() const;

        int fd() const;

        static FileHandle cast_static(FileHandle handle);
        static FileHandle cast_dynamic(FileHandle handle);

    protected:
        explicit FileHandle(std::auto_ptr<FileBody> body);

        FileBody & body();
        FileBody const & body() const;
        static FileBody & body(FileHandle & handle);
        static FileBody const & body(FileHandle const & handle);

        void fd(int fd);

    private:
        FileBody::ptr body_;
    };

    int retrieve_filehandle(FileHandle handle);

}

///////////////////////////////hh.e////////////////////////////////////////
#include "FileHandle.cci"
//#include "FileHandle.ct"
//#include "FileHandle.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
