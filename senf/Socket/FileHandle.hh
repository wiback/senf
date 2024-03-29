//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief FileHandle public header
 */

/** \defgroup handle_group The Handle Hierarchy

    \diaimage FhHierarchy.dia

    The senf::FileHandle class is the base of a hierarchy of socket handle classes (realized as
    templates). These classes provide an interface to the complete socket API. While going down the
    inheritance hierarchy, the interface will be more and more complete.

    The most complete interface is provided by senf::ProtocolClientSocketHandle and
    senf::ProtocolServerSocketHandle. The template Arguments specifies the Protocol class of the
    underlying socket type. These are the \e only classes having public constructors and are
    therefore the only classes, which may be created by the library user. You will normally use
    these classes by naming a specific socket typedef (e.g. senf::TCPv4ClientSocketHandle).

    However, to aid writing flexible and generic code, the socket library provides the
    senf::ClientSocketHandle and senf::ServerSocketHandle class templates. These templates implement
    a family of closely related classes based on the specification of the socket policy. This policy
    specification may be \e incomplete (see below). Instances of
    senf::ClientSocketHandle/senf::ServerSocketHandle can be assigned and converted to different
    ClientSocketHandle/ServerSocketHandle types as long as the policy specifications are compatible.

    \attention It is very important, to (almost) always pass the socket handle <em>by
    value</em>. The socket handle is a very lightweight class and designed to be used like an
    ordinary built-in type. This is very important in combination with the policy interface.

    \note The FileHandle hierarchy below the SocketHandle template is \e not meant to be user
    extensible. To add new socket types, you should introduce new protocol and/or policy classes,
    the SocketHandle classes should not be changed.
 */

#ifndef HH_SENF_Socket_FileHandle_
#define HH_SENF_Socket_FileHandle_ 1

// Custom includes
#include <senf/Utils/Cpp11Support/smart_ptr.hh>
#include <senf/Utils/safe_bool.hh>
#include <senf/Scheduler/ClockService.hh>

//#include "FileHandle.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "FileHandle.ih"

namespace senf {

    /** \brief access FileHandle extra data information
     */
    class FileHandleAccess
    {
    protected:
        static void * extraPtr(FileHandle const & fh);
        static void extraPtr(FileHandle fh, void * ptr);
    };

    /// \addtogroup handle_group
    //\{

    /** \brief Basic file handle wrapper

        senf::FileHandle provides a simple wrapper for arbitrary file handles. It exposes only a
        minimal interface which does \e not include reading or writing (since some filehandles are
        not readable or writable or only using special function calls like sendto).

        The FileHandle class provides handle/body handling and uses automatic reference
        counting. The senf::FileHandle instance is very lightweight and should be used like a
        built-in type.

        \attention You should mostly pass around senf::FileHandle objects by \e value and not by
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
        will have to call the protected FileHandle constructor passing a new senf::FileBody
        instance. This instance may either be a simple senf::FileBody or a class derived from
        senf::FileBody.
     */
    class FileHandle
        : public safe_bool<FileHandle>
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        FileHandle();
        ~FileHandle();

        // my default constructor
        // default copy constructor
        // default copy assignment
        // default destructor

        // no conversion constructors

        //\}
        //-////////////////////////////////////////////////////////////////////////

        void close();                ///< Close filehandle
                                     /**< \throws senf::SystemException */
        void terminate();            ///< Close filehandle ignoring error conditions

        bool readable() const;       ///< Check, whether a read on the handle would not block
                                     ///< (ignoring blocking state)
        bool waitReadable(senf::ClockService::clock_type timeout = senf::ClockService::clock_type(-1)) const;
                                     ///< Wait, until read on the handle would not block (ignoring
                                     ///< blocking state)
                                     /**< \param[in] timeout max time to wait, default is to wait
                                              forever.
                                          \returns \c true, if handle became readable or \c false on
                                              timeout. */
        bool writeable() const;      ///< Check, whether a write on the handle would not block
                                     ///< (ignoring blocking state)
        bool waitWriteable(senf::ClockService::clock_type timeout = senf::ClockService::clock_type(-1)) const;
                                     ///< Wait, until a write on the handle would not block
                                     ///< (ignoring blocking state)
                                     /**< \param[in] timeout max time to wait, default is to wait
                                              forever.
                                          \returns \c true, if handle became writable or \c false on
                                              timeout. */
        bool oobReadable() const;    ///< Check, whether a read of prioritized data on the handle
                                     ///< would not block (ignoring blocking state)
        bool waitOOBReadable(senf::ClockService::clock_type timeout = senf::ClockService::clock_type(-1)) const;
                                     ///< Wait, until read of prioritized data on the handle does
                                     ///< not block (ignoring blocking state)
                                     /**< \param[in] timeout max time to wait, default is to wait
                                              forever.
                                          \returns \c true, if handle became readable for
                                              out-of-band data or \c false on timeout. */

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
                                    /**< This is called when using a FileHandle instance in a boolean
                                       context

                                       See the performance comments for the eof() member */

        int fd() const;             ///< Return the raw FileHandle

        int refcount() const;
        bool is_shared() const;

        static FileHandle cast_static(FileHandle handle);  /**< \internal */
        static FileHandle cast_dynamic(FileHandle handle); /**< \internal */

    protected:
        explicit FileHandle(std::unique_ptr<FileBody> body);
                                    ///< create new FileHandle instance
                                    /**< The FileHandle instance will take over ownership over the
                                       given FileBody instance which must have been allocated using
                                       \c new. To configure the FileHandle behavior, A derived class
                                       may provide any class derived from FileBody here. */

        explicit FileHandle(FileBody::ptr body);

        FileBody & body();          ///< Access body
        FileBody const & body() const; ///< Access body in const context
        static FileBody & body(FileHandle & handle); ///< Access body of another FileHandle instance
        static FileBody const & body(FileHandle const & handle); ///< Access body of another
                                    ///< FileHandle instance in const context

        void fd(int fd);            ///< Set raw filehandle

        void * extraPtr() const;    ///< get extra data pointer
        void extraPtr(void * ptr);  ///< set extra data pointer

    private:
        FileBody::ptr body_;

        friend class FileBody;
        friend class FileHandleAccess;
    };

    /** \brief Adapt FileHandle to senf::scheduler
        \related senf::FileHandle

        \internal

        This function will be called by the Scheduler to retrieve the file descriptor of the
        FileHandle.
     */
    int retrieve_filehandle(FileHandle handle);

    //\}

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "FileHandle.cci"
//#include "FileHandle.ct"
//#include "FileHandle.cti"
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
