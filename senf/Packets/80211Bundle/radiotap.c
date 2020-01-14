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
