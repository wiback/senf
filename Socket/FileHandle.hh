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
    \brief senf::FileHandle public header
 */

#ifndef HH_FileHandle_
#define HH_FileHandle_ 1

// Custom includes
#include <memory> // std::auto_ptr
#include "Utils/SafeBool.hh"

//#include "FileHandle.mpp"
///////////////////////////////hh.p////////////////////////////////////////
#include "FileHandle.ih"

namespace senf {

    
    /** \brief Basic file handle wrapper

	senf::FileHandle provides a simple wrapper for arbitrary file handles. It exposes only a
	minimal interface which does \e not include reading or writing (since some filehandles are
	not readable or writable or only using special function calls like sendto).
	
	The FileHandle class provides handle/body handling and uses automatic reference
	counting. The senf::FileHandle istance is very lightweight and should be used like a
	built-in type.

	\attention You should mostly pass around senf::FileHandle objects by \e value und not by
	reference.

	The FileHandle abstraction is only applicable to real filehandles. It is \e not possible to
	wrap any provider or consumer into a filehandle like interface using this wrapper. The
	wrapper will forward some calls directly to the underlying API without relying on virtual
	methods. This allows important members to be inlined.

	It is not possible to use the senf::FileHandle class directly since it does not have any
	public constructor. The FileHandle class is however the baseclass of all handle classes of
	the socket library.

	\section filehandle_new Writing senf::FileHandle derived classes

	To build a new FileHandle type you need to derive from senf::FileHandle. The derived class
	will have to call the protocted FileHandle constructor passing a new senf::FileBody
	instance. This instance may either be a simple senf::FileBody or a class derived from
	senf::FileBody.
	
	\todo Add public default constructor to allow declaration of (empty) senf::FileHandle
	variables.
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

        void close();                ///< Close filehandle
	                             /**< \throws senf::SystemException */
        void terminate();            ///< Close filehandle ignoring error conditions

        bool readable() const;       ///< Check, wether a read on the handle would not block
				     ///< (ignoring blocking state)
        void waitReadable() const;   ///< Wait, until read on the handle would not block (ignoring
				     ///< blocking state)
        bool writeable() const;      ///< Check, wether a write on the handle would not block
				     ///< (ignoring blocking state)
        void waitWriteable() const;  ///< Wait, until a write on the handle would not block
				     ///< (ignoring blocking state)

        bool blocking() const;       ///< Return current blocking state
        void blocking(bool status);  ///< Set blocking state

        bool eof() const;            ///< Check EOF condition
	                             /**< Depending on the socket type, this might never return \p
					true.
					
					This member is somewhat problematic performance wise if
					called frequently since it relies on virtual
					functions. However, since the eof() handling is extremely
					protocol dependent, a policy based implementation does not
					seam feasible. */
        bool valid() const;          ///< Check filehandle validity
	                             /**< Any operation besides valid() will fail on an invalid
					FileHandle */

	bool boolean_test() const;  ///< Short for valid() && ! eof()
	                            /**< This is called when using a FileHandle instance in a boolen
				       context 

				       See the performance comments for the eof() member */

        int fd() const;             ///< Return the raw FileHandle

        static FileHandle cast_static(FileHandle handle);  /**< \internal */
        static FileHandle cast_dynamic(FileHandle handle); /**< \internal */

    protected:
        explicit FileHandle(std::auto_ptr<FileBody> body);
				    ///< create new FileHandle instance
				    /**< The FileHandle instance will take over ownership over the
				       given FileBody instance which must have been allocated using
				       \c new. To configure the FileHandle behavior, A derived class
				       may provide any class derived from FileBody here. */

        FileBody & body();          ///< Access body
        FileBody const & body() const; ///< Access body in const context
        static FileBody & body(FileHandle & handle); ///< Access body of another FileHandle instance
        static FileBody const & body(FileHandle const & handle); ///< Access body of another
				    ///< FileHandle instance in const context  

        void fd(int fd);	    ///< Set raw filehandle

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
// fill-column: 100
// End:
