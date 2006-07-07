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

#ifndef HH_scheduler_
#define HH_scheduler_ 1

// Custom includes
#include <boost/function.hpp>
#include <boost/utility.hpp>
#include "SatLib/Utils/Exception.hh"

//#include "scheduler.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    /** \brief Singleton class to manage the event loop

        This class manages a single select() type event loop. A
        customer of this class may register any number of file
        descriptiors with this class and pass callback functions to be
        called on input, output or error. This functions are specified
        using boost::function objects
      */
    class Scheduler
        : boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void (int fd)> callback;
        enum events { EV_READ=1, EV_WRITE=2, EV_HUP=4, EV_ERR=8, EV_ALL= };

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // private default constructor
        // no copy constructor
        // no copy assignment
        // default destructor
        // no conversion constructors

        static Scheduler & instance();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void add(int fd, callback const & cb, events eventMask = EV_ALL);
        void remove(int fd, events eventMask = EV_ALL);

    protected:

    private:
        Scheduler();

        struct EventSpec 
        {
            callback cb_read;
            callback cb_write;
            callback cb_hup;
            callback cb_err;
        };
        
        typedef std::map<int,EventSpec> FdTable;

        FdTable fdTable_;
        int epollFd_;
    };


            \param b begin iterator of byte range to create the Packet
                    from
            \param e corresponding end iterator
            \return smart pointer to new packet
            \throws TruncatedPacketException The data cannot be parsed
                    securely (the data might be trunctated or just
                    plain invalid)
         */
        template <class OtherPacket, class InputIterator>
        static typename ptr_t<OtherPacket>::ptr create(InputIterator b, InputIterator e);

#       define BOOST_PP_ITERATION_PARAMS_1 (4, (1, 9, "Packet.mpp", 5))
#       include BOOST_PP_ITERATE()        

        ///@}

        ///\name Interpreter chain
        ///@{
        
        /** \brief get next packet from the interpreter chain
            \return smart pointer to next packet or 0 if last packet */
        ptr next() const;
        /** \brief get previous packet from the interpreter chain
            \return smart pointer to previous packet or 0 if last packet */
        ptr prev() const;
        /** \brief first packet of the interpreter chain
            \return smart pointer to first packet */
        ptr head() const;
        /** \brief get last packet of the interpreter chain
            \return smart pointer to last packet */
        ptr last() const;
        
        /** \brief first packet of given type after the current packet
            \return smart pointer to first following packet of type \a
                OtherPacket or 0, if no such packet exists */
        template <class OtherPacket> typename ptr_t<OtherPacket>::ptr find_next() const;
        /** \brief first packet of given type before the current packet
            \return smart pointer to first preceding packet of type \a
                OtherPacket or 0, if no such packet exists */
        template <class OtherPacket> typename ptr_t<OtherPacket>::ptr find_prev() const;

        /** \brief first packet of given type after the current packet
            \return smart pointer to first following packet of type \a
            OtherPacket. \e Assert's, that a packet of this type exists */
        template <class OtherPacket> typename ptr_t<OtherPacket>::ptr get_next() const;
        /** \brief first packet of given type before the current packet
            \return smart pointer to first preceding packet of type \a
            OtherPacket. \e Assert's, that a packet of this type exists */
        template <class OtherPacket> typename ptr_t<OtherPacket>::ptr get_prev() const;

        /** \brief check, wether the packet is of the given type
            \return true, if packt is of type \a OtherPacket, false
                otherwise */
        template <class OtherPacket> bool is() const; 
        /** \brief cast packet pointer to the given type
            \return a properly cast smart pointer if packet is of type
                \a OtherPacket. Otherwise return 0 */
        template <class OtherPacket> typename ptr_t<OtherPacket>::ptr as(); 

        /** \brief replace current packet interpreter

            This method will \e replace the current packet facade in
            the interpreter list with a new interpreter given by \a
            OtherPacket. 

            \attention This invalidates the packet instance \e
            this</b>. You must ensure, not to use the Packet instance
            any further after this call

            This overload is used, if the \a OtherPacket constructor
            takes no further arguments beyond the data range. If the
            constructor needs further arguments, provide these
            arguments to the \a reinterpret call. The compiler will
            select the correct \a reinterpret overload to use.

            \return smart pointer to a \e new packet facade
            \throws TruncatedPacketException there is not enough data
                to savely interpret the packet as the given type. The
                original packet is \e not invalidated
         */
        template <class OtherPacket>
        typename ptr_t<OtherPacket>::ptr reinterpret();
        template <class OtherPacket, class A0>
        typename ptr_t<OtherPacket>::ptr reinterpret(A0 const & a0);

#       define BOOST_PP_ITERATION_PARAMS_1 (4, (2, 9, "Packet.mpp", 1))
#       include BOOST_PP_ITERATE()

        ///@}

        ///\name Raw packet data
        ///@{

        /** \brief begin interator of raw packet data
            
            This iterator allows access to the raw data interpreted by
            the packet facade. This \e includes any header possibly
            interpreted by the derived packet instance. To access the
            payload of the packet, use next()->begin().

            \return random access iterator to the begin of the raw
                data */
        iterator begin() const;
        /** \brief past-the-end iterator of raw packet data
            
            This iterator allows access to the raw data interpreted by
            the packet facade. This \e includes any header possibly
            interpreted by the derived packet instance. To access the
            payload of the packet, use next()->end().

            \return random access past-the-end iterator of the raw
                data */
        iterator end() const;
        /** \brief raw data size of packet
            \return size of the raw data interpreted by this
                packet in bytes. This is \e not necessarily the size of
                the complete packet, use head()->size() for this. */
        size_t size() const;

        // Modifying the raw packet data

        // FIXME: Make all data mutators protected?

        /** \brief insert single byte \a v

            \attention The change will \e not be validated by the
            derived packet instance. This method is mostly to be used
            by the derived class implementation and their helper
            classes. */
        void insert(iterator pos, byte v);
        /** \brief insert \a n copies of byte \a v
            \attention The change will \e not be validated by the
            derived packet instance. This method is mostly to be used
            by the derived class implementation and their helper
            classes. */
        void insert(iterator pos, size_type n, byte v);
        /** \brief insert a copy of the given range
            \attention The change will \e not be validated by the
            derived packet instance. This method is mostly to be used
            by the derived class implementation and their helper
            classes. */
        template <class InputIterator> 
        void insert(iterator pos, InputIterator f, InputIterator l);

        /** \brief erase single byte
            \attention The change will \e not be validated by the
            derived packet instance. This method is mostly to be used
            by the derived class implementation and their helper
            classes. */
        void erase(iterator pos);
        /** \brief erase range
            \attention The change will \e not be validated by the
            derived packet instance. This method is mostly to be used
            by the derived class implementation and their helper
            classes. */
        void erase(iterator first, iterator last);

        ///@}

    protected:
        ///\name Derived class interface
        ///@{

        /** \brief create new interpreter facade for an existing packet
            
            This constructor is called, when a new interpreter is to
            be added to the interpreter chain. The constructor is
            called indirectly from registerInterpreter() or
            reinterpret() via the derived classes template
            constructor.
         */
        template <class Operation>
        Packet(Operation const & arg);
        virtual ~Packet();
        
    private:
        /** \brief create next packet interpreter

            This method is called by next(), last() or find_next() to
            create any missing interpreters in the interpreter
            chain. This method must be overridden in the derived class
            to register the next packet interpreter in the interpreter
            chain with the packet framework.

            To register the new interpreter, use
            registerInterpreter() to create the new Packet
            instance. The new instance is automatically added to the
            interpreter chain after the current interpreter.

            See also satcom::pkf::PacketRegistryMixin on how to
            use a Registry to find the next interpreters implementing
            class.
         */
        virtual void v_nextInterpreter() const = 0;

        /** \brief finalize packet for sending

            This method is called by the packet framework to let the
            interpreter facade do some final calculations/packet
            cleanup before the packet is sent out or digested in some
            other way. This is the place to calcaulate checksums and
            such.

            This method is autmatically called for all interpreters on
            the interpreter chain.
         */
        virtual void v_finalize() = 0;

    protected:
        /** \brief add interpreter to interpreter chain

            This method is used by v_nextInterpreter() in the derived
            classes to add a new interpreter to the interpreter
            chain. This method will call \c OtherPacket's constructor
            with the correct arguments and insert the new interpreter
            into the interpreter list. This method is used, if no
            further arguments are to be passed to the \c OtherPacket
            constructor. If additional arguments are necessary, just
            add them after \c end. The compiler will then choose the
            correct overload to use.
         */
        template <class OtherPacket>
        typename ptr_t<OtherPacket>::ptr registerInterpreter(
            raw_container::iterator begin, raw_container::iterator end) const;
        template <class OtherPacket, class A0>
        typename ptr_t<OtherPacket>::ptr registerInterpreter(
            raw_container::iterator begin, raw_container::iterator end,
            A0 const & a0) const;

#       define BOOST_PP_ITERATION_PARAMS_1 (4, (2, 9, "Packet.mpp", 3))
#       include BOOST_PP_ITERATE()

        ///@}

    private:

        ///\name Implementation
        ///@{

        void add_ref() const;
        bool release();
        bool unlink();

	struct PacketOp_register;
	friend class PacketOp_register;
        void i_registerInterpreter(Packet * p) const;

	struct PacketOp_replace;
	friend class PacketOp_replace;
        void i_replaceInterpreter(Packet * p);

	struct PacketOp_set;
	friend class PacketOp_set;
        void i_setInterpreter(impl::PacketImpl * i);

    private:
        friend class impl::PacketImpl;
        template <class OtherPacket> friend class impl::PkReg_EntryImpl;

        impl::PacketImpl* impl_;
        size_type begin_;
        size_type end_;
        interpreter_list::iterator self_;
        mutable bool parsed_;
        mutable refcount_t refcount_;

        ///@}
    };

    /** \brief smart pointer handling
        \relates Packet */
    void intrusive_ptr_add_ref(Packet const *);
    /** \brief smart pointer handling
        \relates Packet */
    void intrusive_ptr_release(Packet *);

    struct SystemException : public std::exception
    {
        SystemException(errno_) : errno(errno_) {};
        virtual char const * what() const throw();
        int errno;
    }    
    
}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "scheduler.cci"
//#include "scheduler.ct"
//#include "scheduler.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
