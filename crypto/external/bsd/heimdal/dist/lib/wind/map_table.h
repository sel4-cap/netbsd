/*	$NetBSD: map_table.h,v 1.2.10.1 2023/08/11 13:40:02 martin Exp $	*/

/* ./map_table.h */
/* Automatically generated at 2022-11-15T13:59:51.761662 */

#ifndef MAP_TABLE_H
#define MAP_TABLE_H 1

#include "windlocl.h"

struct translation {
  uint32_t key;
  unsigned short val_len;
  unsigned short val_offset;
  wind_profile_flags flags;
};

extern const struct translation _wind_map_table[];

extern const size_t _wind_map_table_size;

extern const uint32_t _wind_map_table_val[];

#endif /* MAP_TABLE_H */
