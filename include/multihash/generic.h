/*! @file generic.h
 * @brief generic definitions for multihashes
 */

#pragma once

#define mh_assert_static(isTrue) void mh_assert_static(char x[1 - (!(isTrue))])

#define UNUSED(x) (void)(x)
