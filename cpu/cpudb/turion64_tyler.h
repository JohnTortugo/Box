/////////////////////////////////////////////////////////////////////////
// $Id: turion64_tyler.h 11033 2012-02-13 23:29:01Z sshwarts $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_TURION64_TYLER_CPUID_DEFINITIONS_H
#define BX_TURION64_TYLER_CPUID_DEFINITIONS_H

#if BX_SUPPORT_X86_64

#include "cpu/cpuid.h"

class turion64_tyler_t : public bx_cpuid_t {
public:
  turion64_tyler_t(BX_CPU_C *cpu);
  virtual ~turion64_tyler_t() {}

  // return CPU name
  virtual const char *get_name(void) const { return "turion64_tyler"; }

  virtual Bit64u get_isa_extensions_bitmask(void) const;
  virtual Bit32u get_cpu_extensions_bitmask(void) const;

  virtual void get_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const;

  virtual void dump_cpuid(void) const;

private:
  void get_std_cpuid_leaf_0(cpuid_function_t *leaf) const;
  void get_std_cpuid_leaf_1(cpuid_function_t *leaf) const;

  void get_ext_cpuid_leaf_0(cpuid_function_t *leaf) const;
  void get_ext_cpuid_leaf_1(cpuid_function_t *leaf) const;
  void get_ext_cpuid_brand_string_leaf(Bit32u function, cpuid_function_t *leaf) const;
  void get_ext_cpuid_leaf_5(cpuid_function_t *leaf) const;
  void get_ext_cpuid_leaf_6(cpuid_function_t *leaf) const;
  void get_ext_cpuid_leaf_7(cpuid_function_t *leaf) const;
  void get_ext_cpuid_leaf_8(cpuid_function_t *leaf) const;
#if BX_SUPPORT_SVM
  void get_ext_cpuid_leaf_A(cpuid_function_t *leaf) const;
#endif
};

extern bx_cpuid_t *create_turion64_tyler_cpuid(BX_CPU_C *cpu);

#endif // BX_SUPPORT_X86_64

#endif
