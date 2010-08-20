// $Id$
//
// Copyright (C) 2010
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

# include <senf/autoconf.hh>
#
# ifdef BYTEORDER_LITTLE_ENDIAN
#     ifndef HAVE_LE16TOH
#         define le16toh(x) (x)
#     endif
#     ifndef HAVE_LE32TOH
#         define le32toh(x) (x)
#     endif
# else
#     if !defined(HAVE_LE16TOH) || !defined(HAVE_LE32TOH)
#         include <byteswap.h>
#     endif
#     ifndef HAVE_LE16TOH
#         define le16toh(x) bswap_16(x)
#     endif
#     ifndef HAVE_LE32TOH
#         define le32toh(x) bswap_32(x)
#     endif
# endif
#
# include "radiotap/radiotap.c"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
