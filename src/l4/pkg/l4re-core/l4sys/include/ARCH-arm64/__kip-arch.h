/*
 * (c) 2013 Adam Lackorzynski <adam@os.inf.tu-dresden.de>,
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */
#pragma once

/**
 * \internal
 * Do not expose as any interface yet as this assumes equality over all CPUs
 * which will not be true along the way.
 */
struct l4_kip_platform_info_arch
{
  struct
  {
    l4_uint64_t MIDR, MPIDR, REVIDR;
    l4_uint64_t ID_PFR[2], ID_DFR0, ID_AFR0, ID_MMFR[4], ID_ISAR[6], ID_MVFR[3];
    l4_uint64_t ID_AA64DFR[2], ID_AA64ISAR[3], ID_AA64MMFR[3], ID_AA64PFR[2];
  } cpuinfo;
};
