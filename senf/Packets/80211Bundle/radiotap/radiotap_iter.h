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


#ifndef __RADIOTAP_ITER_H
#define __RADIOTAP_ITER_H

#include <stdint.h>
#include "radiotap.h"

/* Radiotap header iteration
 *   implemented in radiotap.c
 */

struct radiotap_override {
	uint8_t field;
	uint8_t align:4, size:4;
};

struct radiotap_align_size {
	uint8_t align:4, size:4;
};

struct ieee80211_radiotap_namespace {
	const struct radiotap_align_size *align_size;
	int n_bits;
	uint32_t oui;
	uint8_t subns;
};

struct ieee80211_radiotap_vendor_namespaces {
	const struct ieee80211_radiotap_namespace *ns;
	int n_ns;
};

/**
 * struct ieee80211_radiotap_iterator - tracks walk thru present radiotap args
 * @this_arg_index: index of current arg, valid after each successful call
 *	to ieee80211_radiotap_iterator_next()
 * @this_arg: pointer to current radiotap arg; it is valid after each
 *	call to ieee80211_radiotap_iterator_next() but also after
 *	ieee80211_radiotap_iterator_init() where it will point to
 *	the beginning of the actual data portion
 * @this_arg_size: length of the current arg, for convenience
 * @current_namespace: pointer to the current namespace definition
 *	(or internally %NULL if the current namespace is unknown)
 * @is_radiotap_ns: indicates whether the current namespace is the default
 *	radiotap namespace or not
 *
 * @overrides: override standard radiotap fields
 * @n_overrides: number of overrides
 *
 * @_rtheader: pointer to the radiotap header we are walking through
 * @_max_length: length of radiotap header in cpu byte ordering
 * @_arg_index: next argument index
 * @_arg: next argument pointer
 * @_next_bitmap: internal pointer to next present u32
 * @_bitmap_shifter: internal shifter for curr u32 bitmap, b0 set == arg present
 * @_vns: vendor namespace definitions
 * @_next_ns_data: beginning of the next namespace's data
 * @_reset_on_ext: internal; reset the arg index to 0 when going to the
 *	next bitmap word
 *
 * Describes the radiotap parser state. Fields prefixed with an underscore
 * must not be used by users of the parser, only by the parser internally.
 */

struct ieee80211_radiotap_iterator {
	struct ieee80211_radiotap_header *_rtheader;
	const struct ieee80211_radiotap_vendor_namespaces *_vns;
	const struct ieee80211_radiotap_namespace *current_namespace;

	unsigned char *_arg, *_next_ns_data;
	uint32_t *_next_bitmap;

	unsigned char *this_arg;
	const struct radiotap_override *overrides;	/* Only for RADIOTAP_SUPPORT_OVERRIDES */
	int n_overrides;				/* Only for RADIOTAP_SUPPORT_OVERRIDES */
	int this_arg_index;
	int this_arg_size;

	int is_radiotap_ns;

	int _max_length;
	int _arg_index;
	uint32_t _bitmap_shifter;
	int _reset_on_ext;
};

extern int ieee80211_radiotap_iterator_init(
	struct ieee80211_radiotap_iterator *iterator,
	struct ieee80211_radiotap_header *radiotap_header,
	int max_length, const struct ieee80211_radiotap_vendor_namespaces *vns);

extern int ieee80211_radiotap_iterator_next(
	struct ieee80211_radiotap_iterator *iterator);

#endif /* __RADIOTAP_ITER_H */
