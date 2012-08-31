/////////////////////////////////////////////////////////////////////////
// $Id: fetchdecode_avx.h 11313 2012-08-05 13:52:40Z sshwarts $
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

#ifndef BX_AVX_FETCHDECODE_TABLES_H
#define BX_AVX_FETCHDECODE_TABLES_H

#if BX_SUPPORT_AVX && BX_CPU_LEVEL >= 6

/* ************************************************************************ */

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_f30f2a[2] = {
  /* 0 */ { 0, BX_IA_VCVTSI2SS_VssEd },
  /* 1 */ { 0, BX_IA_VCVTSI2SS_VssEq }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_f20f2a[2] = {
  /* 0 */ { 0, BX_IA_VCVTSI2SD_VsdEd },
  /* 1 */ { 0, BX_IA_VCVTSI2SD_VsdEq }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_f30f2c[2] = {
  /* 0 */ { 0, BX_IA_VCVTTSS2SI_GdWss },
  /* 1 */ { 0, BX_IA_VCVTTSS2SI_GqWss }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_f20f2c[2] = {
  /* 0 */ { 0, BX_IA_VCVTTSD2SI_GdWsd },
  /* 1 */ { 0, BX_IA_VCVTTSD2SI_GqWsd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_f30f2d[2] = {
  /* 0 */ { 0, BX_IA_VCVTSS2SI_GdWss },
  /* 1 */ { 0, BX_IA_VCVTSS2SI_GqWss }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_f20f2d[2] = {
  /* 0 */ { 0, BX_IA_VCVTSD2SI_GdWsd },
  /* 1 */ { 0, BX_IA_VCVTSD2SI_GqWsd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f6e[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_V128_VMOVD_VdqEd },
  /* 1 */ { BxPrefixSSE66, BX_IA_V128_VMOVQ_VdqEq },
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_660f7e[2] = {
  /* 0 */ { 0, BX_IA_V128_VMOVD_EdVd },
  /* 1 */ { 0, BX_IA_V128_VMOVQ_EqVq },
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX128_VexW_0f3a16[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_V128_VPEXTRD_EdVdqIb },
  /* 1 */ { BxPrefixSSE66, BX_IA_V128_VPEXTRQ_EqVdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f3818[2] = {
  /* R */ { BxPrefixSSE66 | BxVexW0, BX_IA_VBROADCASTSS_VpsWss },
  /* M */ { BxPrefixSSE66 | BxVexW0, BX_IA_VBROADCASTSS_VpsMss }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f3819[2] = {
  /* R */ { BxPrefixSSE66 | BxVexW0, BX_IA_V256_VBROADCASTSD_VpdWsd },
  /* M */ { BxPrefixSSE66 | BxVexW0, BX_IA_V256_VBROADCASTSD_VpdMsd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX128_VexW_0f3a22[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_V128_VPINSRD_VdqEdIb },
  /* 1 */ { BxPrefixSSE66, BX_IA_V128_VPINSRQ_VdqEqIb }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f3845[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VPSRLVD_VdqHdqWdq },
  /* 1 */ { BxPrefixSSE66, BX_IA_VPSRLVQ_VdqHdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f3847[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VPSLLVD_VdqHdqWdq },
  /* 1 */ { BxPrefixSSE66, BX_IA_VPSLLVQ_VdqHdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f388c[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VMASKMOVD_VdqHdqMdq },
  /* 1 */ { BxPrefixSSE66, BX_IA_VMASKMOVQ_VdqHdqMdq }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f388e[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VMASKMOVD_MdqHdqVdq },
  /* 1 */ { BxPrefixSSE66, BX_IA_VMASKMOVQ_MdqHdqVdq }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f3890[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VGATHERDD_VdqHdq },
  /* 1 */ { BxPrefixSSE66, BX_IA_VGATHERDQ_VdqHdq }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f3891[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VGATHERQD_VdqHdq },
  /* 1 */ { BxPrefixSSE66, BX_IA_VGATHERQQ_VdqHdq }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f3892[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VGATHERDPS_VpsHps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VGATHERDPD_VpdHpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f3893[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VGATHERQPS_VpsHps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VGATHERQPD_VpdHpd }
};

// FMA
static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f3896[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMADDSUB132PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMADDSUB132PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f3897[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMSUBADD132PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMSUBADD132PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f3898[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMADD132PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMADD132PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f3899[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMADD132SS_VpsHssWss },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMADD132SD_VpdHsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f389a[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMSUB132PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMSUB132PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f389b[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMSUB132SS_VpsHssWss },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMSUB132SD_VpdHsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f389c[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFNMADD132PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFNMADD132PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f389d[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFNMADD132SS_VpsHssWss },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFNMADD132SD_VpdHsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f389e[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFNMSUB132PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFNMSUB132PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f389f[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFNMSUB132SS_VpsHssWss },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFNMSUB132SD_VpdHsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38a6[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMADDSUB213PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMADDSUB213PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38a7[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMSUBADD213PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMSUBADD213PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38a8[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMADD213PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMADD213PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38a9[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMADD213SS_VpsHssWss },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMADD213SD_VpdHsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38aa[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMSUB213PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMSUB213PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38ab[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMSUB213SS_VpsHssWss },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMSUB213SD_VpdHsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38ac[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFNMADD213PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFNMADD213PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38ad[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFNMADD213SS_VpsHssWss },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFNMADD213SD_VpdHsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38ae[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFNMSUB213PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFNMSUB213PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38af[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFNMSUB213SS_VpsHssWss },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFNMSUB213SD_VpdHsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38b6[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMADDSUB231PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMADDSUB231PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38b7[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMSUBADD231PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMSUBADD231PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38b8[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMADD231PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMADD231PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38b9[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMADD231SS_VpsHssWss },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMADD231SD_VpdHsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38ba[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMSUB231PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMSUB231PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38bb[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFMSUB231SS_VpsHssWss },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFMSUB231SD_VpdHsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38bc[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFNMADD231PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFNMADD231PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38bd[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFNMADD231SS_VpsHssWss },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFNMADD231SD_VpdHsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38be[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFNMSUB231PS_VpsHpsWps },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFNMSUB231PD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38bf[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VFNMSUB231SS_VpsHssWss },
  /* 1 */ { BxPrefixSSE66, BX_IA_VFNMSUB231SD_VpdHsdWsd }
};
// FMA

// BMI
static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38f2[2] = {
  /* 0 */ { BxPrefixSSE, BX_IA_ANDN_GdBdEd, BxOpcodeGroupSSE_ERR },
  /* 1 */ { BxPrefixSSE, BX_IA_ANDN_GqBqEq, BxOpcodeGroupSSE_ERR }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVXG17VEXW0[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { BxPrefixSSE, BX_IA_BLSR_BdEd, BxOpcodeGroupSSE_ERR },
  /* 2 */ { BxPrefixSSE, BX_IA_BLSMSK_BdEd, BxOpcodeGroupSSE_ERR },
  /* 3 */ { BxPrefixSSE, BX_IA_BLSI_BdEd, BxOpcodeGroupSSE_ERR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVXG17VEXW1[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { BxPrefixSSE, BX_IA_BLSR_BqEq, BxOpcodeGroupSSE_ERR },
  /* 2 */ { BxPrefixSSE, BX_IA_BLSMSK_BqEq, BxOpcodeGroupSSE_ERR },
  /* 3 */ { BxPrefixSSE, BX_IA_BLSI_BqEq, BxOpcodeGroupSSE_ERR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38f3[2] = {
  /* 0 */ { BxGroup17, BX_IA_ERROR, BxOpcodeInfoAVXG17VEXW0 },
  /* 1 */ { BxGroup17, BX_IA_ERROR, BxOpcodeInfoAVXG17VEXW1 }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f38f5_VEX0[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_PEXT_GdEdBd },
  /* F2 */ { 0, BX_IA_PDEP_GdEdBd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f38f5_VEX1[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_PEXT_GqEqBq },
  /* F2 */ { 0, BX_IA_PDEP_GqEqBq }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38f5[2] = {
  /* 0 */ { BxPrefixSSE, BX_IA_BZHI_GdEdBd, BxOpcodeGroupAVX_0f38f5_VEX0 },
  /* 1 */ { BxPrefixSSE, BX_IA_BZHI_GqEqBq, BxOpcodeGroupAVX_0f38f5_VEX1 }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38f6[2] = {
  /* 0 */ { BxPrefixSSEF2, BX_IA_MULX_GdBdEd },
  /* 1 */ { BxPrefixSSEF2, BX_IA_MULX_GqBqEq }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f38f7_VEX0[3] = {
  /* 66 */ { 0, BX_IA_SHLX_GdEdBd },
  /* F3 */ { 0, BX_IA_SARX_GdEdBd },
  /* F2 */ { 0, BX_IA_SHRX_GdEdBd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f38f7_VEX1[3] = {
  /* 66 */ { 0, BX_IA_SHLX_GqEqBq },
  /* F3 */ { 0, BX_IA_SARX_GqEqBq },
  /* F2 */ { 0, BX_IA_SHRX_GqEqBq }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f38f7[2] = {
  /* 0 */ { BxPrefixSSE, BX_IA_BEXTR_GdEdBd, BxOpcodeGroupAVX_0f38f7_VEX0 },
  /* 1 */ { BxPrefixSSE, BX_IA_BEXTR_GqEqBq, BxOpcodeGroupAVX_0f38f7_VEX1 }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f3af0[2] = {
  /* 0 */ { BxPrefixSSEF2, BX_IA_RORX_GdEdIb, BxOpcodeGroupSSE_ERR },
  /* 1 */ { BxPrefixSSEF2, BX_IA_RORX_GqEqIb, BxOpcodeGroupSSE_ERR }
};
// BMI

/* ************************************************************************ */

/* ********** */
/* SSE Groups */
/* ********** */

static const BxOpcodeInfo_t BxOpcodeGroupAVX128_0f10[3] = {
  /* 66 */ { 0, BX_IA_VMOVUPD_VpdWpd },
  /* F3 */ { 0, BX_IA_V128_VMOVSS_VssHpsWss },
  /* F2 */ { 0, BX_IA_V128_VMOVSD_VsdHpdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX256_0f10[3] = {
  /* 66 */ { 0, BX_IA_VMOVUPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX128_0f11[3] = {
  /* 66 */ { 0, BX_IA_VMOVUPD_WpdVpd },
  /* F3 */ { 0, BX_IA_V128_VMOVSS_WssHpsVss },
  /* F2 */ { 0, BX_IA_V128_VMOVSD_WsdHpdVsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX256_0f11[3] = {
  /* 66 */ { 0, BX_IA_VMOVUPD_WpdVpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX128_0f12[3] = {
  /* 66 */ { 0, BX_IA_V128_VMOVLPD_VpdHpdMq },
  /* F3 */ { 0, BX_IA_VMOVSLDUP_VpsWps },
  /* F2 */ { 0, BX_IA_VMOVDDUP_VpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX256_0f12[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_VMOVSLDUP_VpsWps },
  /* F2 */ { 0, BX_IA_VMOVDDUP_VpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX128_0f13M[3] = {
  /* 66 */ { 0, BX_IA_V128_VMOVLPD_MqVsd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f14[3] = {
  /* 66 */ { 0, BX_IA_VUNPCKLPD_VpdHpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f15[3] = {
  /* 66 */ { 0, BX_IA_VUNPCKHPD_VpdHpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX128_0f16[3] = {
  /* 66 */ { 0, BX_IA_V128_VMOVHPD_VpdHpdMq },
  /* F3 */ { 0, BX_IA_VMOVSHDUP_VpsWps },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX128_0f17M[3] = {
  /* 66 */ { 0, BX_IA_V128_VMOVHPD_MqVsd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f28[3] = {
  /* 66 */ { 0, BX_IA_VMOVAPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f29[3] = {
  /* 66 */ { 0, BX_IA_VMOVAPD_WpdVpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f2a[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_f30f2a },
  /* F2 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_f20f2a }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f2bM[3] = {
  /* 66 */ { 0, BX_IA_VMOVNTPD_MpdVpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f2c[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_f30f2c },
  /* F2 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_f20f2c }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f2d[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_f30f2d },
  /* F2 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_f20f2d }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f2e[3] = {
  /* 66 */ { 0, BX_IA_VUCOMISD_VsdWsd },            	
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f2f[3] = {
  /* 66 */ { 0, BX_IA_VCOMISD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f50R[3] = {
  /* 66 */ { 0, BX_IA_VMOVMSKPD_GdVRpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f51[3] = {
  /* 66 */ { 0, BX_IA_VSQRTPD_VpdWpd },
  /* F3 */ { 0, BX_IA_VSQRTSS_VssHpsWss },
  /* F2 */ { 0, BX_IA_VSQRTSD_VsdHpdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f52[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_VRSQRTSS_VssHpsWss },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f53[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_VRCPSS_VssHpsWss },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f54[3] = {
  /* 66 */ { 0, BX_IA_VANDPD_VpdHpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f55[3] = {
  /* 66 */ { 0, BX_IA_VANDNPD_VpdHpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f56[3] = {
  /* 66 */ { 0, BX_IA_VORPD_VpdHpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f57[3] = {
  /* 66 */ { 0, BX_IA_VXORPD_VpdHpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f58[3] = {
  /* 66 */ { 0, BX_IA_VADDPD_VpdHpdWpd },
  /* F3 */ { 0, BX_IA_VADDSS_VssHpsWss },
  /* F2 */ { 0, BX_IA_VADDSD_VsdHpdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f59[3] = {
  /* 66 */ { 0, BX_IA_VMULPD_VpdHpdWpd },
  /* F3 */ { 0, BX_IA_VMULSS_VssHpsWss },
  /* F2 */ { 0, BX_IA_VMULSD_VsdHpdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f5a[3] = {
  /* 66 */ { 0, BX_IA_VCVTPD2PS_VpsWpd },
  /* F3 */ { 0, BX_IA_VCVTSS2SD_VsdWss },
  /* F2 */ { 0, BX_IA_VCVTSD2SS_VssWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f5b[3] = {
  /* 66 */ { 0, BX_IA_VCVTPS2DQ_VdqWps },
  /* F3 */ { 0, BX_IA_VCVTTPS2DQ_VdqWps },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f5c[3] = {
  /* 66 */ { 0, BX_IA_VSUBPD_VpdHpdWpd },
  /* F3 */ { 0, BX_IA_VSUBSS_VssHpsWss },
  /* F2 */ { 0, BX_IA_VSUBSD_VsdHpdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f5d[3] = {
  /* 66 */ { 0, BX_IA_VMINPD_VpdHpdWpd },
  /* F3 */ { 0, BX_IA_VMINSS_VssHpsWss },
  /* F2 */ { 0, BX_IA_VMINSD_VsdHpdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f5e[3] = {
  /* 66 */ { 0, BX_IA_VDIVPD_VpdHpdWpd },
  /* F3 */ { 0, BX_IA_VDIVSS_VssHpsWss },
  /* F2 */ { 0, BX_IA_VDIVSD_VsdHpdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f5f[3] = {
  /* 66 */ { 0, BX_IA_VMAXPD_VpdHpdWpd },
  /* F3 */ { 0, BX_IA_VMAXSS_VssHpsWss },
  /* F2 */ { 0, BX_IA_VMAXSD_VsdHpdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f6f[3] = {
  /* 66 */ { 0, BX_IA_VMOVDQA_VdqWdq },
  /* F3 */ { 0, BX_IA_VMOVDQU_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX128_0f70[3] = {
  /* 66 */ { 0, BX_IA_V128_VPSHUFD_VdqWdqIb  },
  /* F3 */ { 0, BX_IA_V128_VPSHUFHW_VdqWdqIb },
  /* F2 */ { 0, BX_IA_V128_VPSHUFLW_VdqWdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX256_0f70[3] = {
  /* 66 */ { 0, BX_IA_V256_VPSHUFD_VdqWdqIb  },
  /* F3 */ { 0, BX_IA_V256_VPSHUFHW_VdqWdqIb },
  /* F2 */ { 0, BX_IA_V256_VPSHUFLW_VdqWdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f7c[3] = {
  /* 66 */ { 0, BX_IA_VHADDPD_VpdHpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_VHADDPS_VpsHpsWps }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f7d[3] = {
  /* 66 */ { 0, BX_IA_VHSUBPD_VpdHpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_VHSUBPS_VpsHpsWps }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f7e[3] = {
  /* 66 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_660f7e },
  /* F3 */ { 0, BX_IA_VMOVQ_VqWq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f7f[3] = {
  /* 66 */ { 0, BX_IA_VMOVDQA_WdqVdq },
  /* F3 */ { 0, BX_IA_VMOVDQU_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0fc2[3] = {
  /* 66 */ { 0, BX_IA_VCMPPD_VpdHpdWpdIb },
  /* F3 */ { 0, BX_IA_VCMPSS_VssHpsWssIb },
  /* F2 */ { 0, BX_IA_VCMPSD_VsdHpdWsdIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0fc6[3] = {
  /* 66 */ { 0, BX_IA_VSHUFPD_VpdHpdWpdIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0fd0[3] = {
  /* 66 */ { 0, BX_IA_VADDSUBPD_VpdHpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_VADDSUBPS_VpsHpsWps }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX128_0fe6[3] = {
  /* 66 */ { 0, BX_IA_VCVTTPD2DQ_VqWpd },
  /* F3 */ { 0, BX_IA_V128_VCVTDQ2PD_VpdWq },
  /* F2 */ { 0, BX_IA_VCVTPD2DQ_VqWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX256_0fe6[3] = {
  /* 66 */ { 0, BX_IA_VCVTTPD2DQ_VqWpd },
  /* F3 */ { 0, BX_IA_V256_VCVTDQ2PD_VpdWq },
  /* F2 */ { 0, BX_IA_VCVTPD2DQ_VqWpd }
};

/* ******** */
/* Group 12 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoAVX128G12R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V128_VPSRLW_UdqIb },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V128_VPSRAW_UdqIb },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V128_VPSLLW_UdqIb },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX256G12R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V256_VPSRLW_UdqIb },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V256_VPSRAW_UdqIb },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V256_VPSLLW_UdqIb },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******** */
/* Group 13 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoAVX128G13R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V128_VPSRLD_UdqIb },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V128_VPSRAD_UdqIb },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V128_VPSLLD_UdqIb },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX256G13R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V256_VPSRLD_UdqIb },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V256_VPSRAD_UdqIb },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V256_VPSLLD_UdqIb },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******** */
/* Group 14 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoAVX128G14R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V128_VPSRLQ_UdqIb },
  /* 3 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V128_VPSRLDQ_UdqIb },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V128_VPSLLQ_UdqIb },
  /* 7 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V128_VPSLLDQ_UdqIb }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX256G14R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V256_VPSRLQ_UdqIb },
  /* 3 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V256_VPSRLDQ_UdqIb },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V256_VPSLLQ_UdqIb },
  /* 7 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V256_VPSLLDQ_UdqIb }
};

/* ******** */
/* Group 15 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoAVXG15M[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxPrefixSSE, BX_IA_VLDMXCSR, BxOpcodeGroupSSE_ERR },
  /* 3 */ { BxPrefixSSE, BX_IA_VSTMXCSR, BxOpcodeGroupSSE_ERR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ************************************************************************ */

static const BxOpcodeInfo_t BxOpcodeTableAVX[256*3*2] = {
  // 256 entries for VEX-encoded 0x0F opcodes (VEX.L=0)
  /* 00 /0 */ { 0, BX_IA_ERROR },
  /* 01 /0 */ { 0, BX_IA_ERROR },
  /* 02 /0 */ { 0, BX_IA_ERROR },
  /* 03 /0 */ { 0, BX_IA_ERROR },
  /* 04 /0 */ { 0, BX_IA_ERROR },
  /* 05 /0 */ { 0, BX_IA_ERROR },
  /* 06 /0 */ { 0, BX_IA_ERROR },
  /* 07 /0 */ { 0, BX_IA_ERROR },
  /* 08 /0 */ { 0, BX_IA_ERROR },
  /* 09 /0 */ { 0, BX_IA_ERROR },
  /* 0A /0 */ { 0, BX_IA_ERROR },
  /* 0B /0 */ { 0, BX_IA_ERROR },
  /* 0C /0 */ { 0, BX_IA_ERROR },
  /* 0D /0 */ { 0, BX_IA_ERROR },
  /* 0E /0 */ { 0, BX_IA_ERROR },
  /* 0F /0 */ { 0, BX_IA_ERROR },
  /* 10 /0 */ { BxPrefixSSE, BX_IA_VMOVUPS_VpsWps, BxOpcodeGroupAVX128_0f10 },
  /* 11 /0 */ { BxPrefixSSE, BX_IA_VMOVUPS_WpsVps, BxOpcodeGroupAVX128_0f11 },
  /* 12 /0 */ { BxPrefixSSE, BX_IA_V128_VMOVLPS_VpsHpsMq, BxOpcodeGroupAVX128_0f12 },
  /* 13 /0 */ { BxPrefixSSE, BX_IA_V128_VMOVLPS_MqVps, BxOpcodeGroupAVX128_0f13M },
  /* 14 /0 */ { BxPrefixSSE, BX_IA_VUNPCKLPS_VpsHpsWps, BxOpcodeGroupAVX_0f14 },
  /* 15 /0 */ { BxPrefixSSE, BX_IA_VUNPCKHPS_VpsHpsWps, BxOpcodeGroupAVX_0f15 },
  /* 16 /0 */ { BxPrefixSSE, BX_IA_V128_VMOVHPS_VpsHpsMq, BxOpcodeGroupAVX128_0f16 },
  /* 17 /0 */ { BxPrefixSSE, BX_IA_V128_VMOVHPS_MqVps, BxOpcodeGroupAVX128_0f17M },
  /* 18 /0 */ { 0, BX_IA_ERROR },
  /* 19 /0 */ { 0, BX_IA_ERROR },
  /* 1A /0 */ { 0, BX_IA_ERROR },
  /* 1B /0 */ { 0, BX_IA_ERROR },
  /* 1C /0 */ { 0, BX_IA_ERROR },
  /* 1D /0 */ { 0, BX_IA_ERROR },
  /* 1E /0 */ { 0, BX_IA_ERROR },
  /* 1F /0 */ { 0, BX_IA_ERROR },
  /* 20 /0 */ { 0, BX_IA_ERROR }, // WARNING: ModC0 always '1
  /* 21 /0 */ { 0, BX_IA_ERROR }, // WARNING: ModC0 always '1
  /* 22 /0 */ { 0, BX_IA_ERROR }, // WARNING: ModC0 always '1
  /* 23 /0 */ { 0, BX_IA_ERROR }, // WARNING: ModC0 always '1
  /* 24 /0 */ { 0, BX_IA_ERROR },
  /* 25 /0 */ { 0, BX_IA_ERROR },
  /* 26 /0 */ { 0, BX_IA_ERROR },
  /* 27 /0 */ { 0, BX_IA_ERROR },
  /* 28 /0 */ { BxPrefixSSE, BX_IA_VMOVAPS_VpsWps, BxOpcodeGroupAVX_0f28 },
  /* 29 /0 */ { BxPrefixSSE, BX_IA_VMOVAPS_WpsVps, BxOpcodeGroupAVX_0f29 },
  /* 2A /0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f2a },
  /* 2B /0 */ { BxPrefixSSE, BX_IA_VMOVNTPS_MpsVps, BxOpcodeGroupAVX_0f2bM },
  /* 2C /0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f2c },
  /* 2D /0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f2d },
  /* 2E /0 */ { BxPrefixSSE, BX_IA_VUCOMISS_VssWss, BxOpcodeGroupAVX_0f2e },
  /* 2F /0 */ { BxPrefixSSE, BX_IA_VCOMISS_VpsWps, BxOpcodeGroupAVX_0f2f },
  /* 30 /0 */ { 0, BX_IA_ERROR },
  /* 31 /0 */ { 0, BX_IA_ERROR },
  /* 32 /0 */ { 0, BX_IA_ERROR },
  /* 33 /0 */ { 0, BX_IA_ERROR },
  /* 34 /0 */ { 0, BX_IA_ERROR },
  /* 35 /0 */ { 0, BX_IA_ERROR },
  /* 36 /0 */ { 0, BX_IA_ERROR },
  /* 37 /0 */ { 0, BX_IA_ERROR },
  /* 38 /0 */ { 0, BX_IA_ERROR }, // 3-Byte Escape
  /* 39 /0 */ { 0, BX_IA_ERROR },
  /* 3A /0 */ { 0, BX_IA_ERROR }, // 3-Byte Escape
  /* 3B /0 */ { 0, BX_IA_ERROR },
  /* 3C /0 */ { 0, BX_IA_ERROR },
  /* 3D /0 */ { 0, BX_IA_ERROR },
  /* 3E /0 */ { 0, BX_IA_ERROR },
  /* 3F /0 */ { 0, BX_IA_ERROR },
  /* 40 /0 */ { 0, BX_IA_ERROR },
  /* 41 /0 */ { 0, BX_IA_ERROR },
  /* 42 /0 */ { 0, BX_IA_ERROR },
  /* 43 /0 */ { 0, BX_IA_ERROR },
  /* 44 /0 */ { 0, BX_IA_ERROR },
  /* 45 /0 */ { 0, BX_IA_ERROR },
  /* 46 /0 */ { 0, BX_IA_ERROR },
  /* 47 /0 */ { 0, BX_IA_ERROR },
  /* 48 /0 */ { 0, BX_IA_ERROR },
  /* 49 /0 */ { 0, BX_IA_ERROR },
  /* 4A /0 */ { 0, BX_IA_ERROR },
  /* 4B /0 */ { 0, BX_IA_ERROR },
  /* 4C /0 */ { 0, BX_IA_ERROR },
  /* 4D /0 */ { 0, BX_IA_ERROR },
  /* 4E /0 */ { 0, BX_IA_ERROR },
  /* 4F /0 */ { 0, BX_IA_ERROR },
  /* 50 /0 */ { BxPrefixSSE, BX_IA_VMOVMSKPS_GdVRps, BxOpcodeGroupAVX_0f50R },
  /* 51 /0 */ { BxPrefixSSE, BX_IA_VSQRTPS_VpsWps, BxOpcodeGroupAVX_0f51 },
  /* 52 /0 */ { BxPrefixSSE, BX_IA_VRSQRTPS_VpsWps, BxOpcodeGroupAVX_0f52 },
  /* 53 /0 */ { BxPrefixSSE, BX_IA_VRCPPS_VpsWps, BxOpcodeGroupAVX_0f53 },
  /* 54 /0 */ { BxPrefixSSE, BX_IA_VANDPS_VpsHpsWps, BxOpcodeGroupAVX_0f54 },
  /* 55 /0 */ { BxPrefixSSE, BX_IA_VANDNPS_VpsHpsWps, BxOpcodeGroupAVX_0f55 },
  /* 56 /0 */ { BxPrefixSSE, BX_IA_VORPS_VpsHpsWps, BxOpcodeGroupAVX_0f56 },
  /* 57 /0 */ { BxPrefixSSE, BX_IA_VXORPS_VpsHpsWps, BxOpcodeGroupAVX_0f57 },
  /* 58 /0 */ { BxPrefixSSE, BX_IA_VADDPS_VpsHpsWps, BxOpcodeGroupAVX_0f58 },
  /* 59 /0 */ { BxPrefixSSE, BX_IA_VMULPS_VpsHpsWps, BxOpcodeGroupAVX_0f59 },
  /* 5A /0 */ { BxPrefixSSE, BX_IA_V128_VCVTPS2PD_VpdWps, BxOpcodeGroupAVX_0f5a },
  /* 5B /0 */ { BxPrefixSSE, BX_IA_VCVTDQ2PS_VpsWdq, BxOpcodeGroupAVX_0f5b },
  /* 5C /0 */ { BxPrefixSSE, BX_IA_VSUBPS_VpsHpsWps, BxOpcodeGroupAVX_0f5c },
  /* 5D /0 */ { BxPrefixSSE, BX_IA_VMINPS_VpsHpsWps, BxOpcodeGroupAVX_0f5d },
  /* 5E /0 */ { BxPrefixSSE, BX_IA_VDIVPS_VpsHpsWps, BxOpcodeGroupAVX_0f5e },
  /* 5F /0 */ { BxPrefixSSE, BX_IA_VMAXPS_VpsHpsWps, BxOpcodeGroupAVX_0f5f },
  /* 60 /0 */ { BxPrefixSSE66, BX_IA_V128_VPUNPCKLBW_VdqHdqWdq },
  /* 61 /0 */ { BxPrefixSSE66, BX_IA_V128_VPUNPCKLWD_VdqHdqWdq },
  /* 62 /0 */ { BxPrefixSSE66, BX_IA_V128_VPUNPCKLDQ_VdqHdqWdq },
  /* 63 /0 */ { BxPrefixSSE66, BX_IA_V128_VPACKSSWB_VdqHdqWdq },
  /* 64 /0 */ { BxPrefixSSE66, BX_IA_V128_VPCMPGTB_VdqHdqWdq },
  /* 65 /0 */ { BxPrefixSSE66, BX_IA_V128_VPCMPGTW_VdqHdqWdq },
  /* 66 /0 */ { BxPrefixSSE66, BX_IA_V128_VPCMPGTD_VdqHdqWdq },
  /* 67 /0 */ { BxPrefixSSE66, BX_IA_V128_VPACKUSWB_VdqHdqWdq },
  /* 68 /0 */ { BxPrefixSSE66, BX_IA_V128_VPUNPCKHBW_VdqHdqWdq },
  /* 69 /0 */ { BxPrefixSSE66, BX_IA_V128_VPUNPCKHWD_VdqHdqWdq },
  /* 6A /0 */ { BxPrefixSSE66, BX_IA_V128_VPUNPCKHDQ_VdqHdqWdq },
  /* 6B /0 */ { BxPrefixSSE66, BX_IA_V128_VPACKSSDW_VdqHdqWdq },
  /* 6C /0 */ { BxPrefixSSE66, BX_IA_V128_VPUNPCKLQDQ_VdqHdqWdq },
  /* 6D /0 */ { BxPrefixSSE66, BX_IA_V128_VPUNPCKHQDQ_VdqHdqWdq },
  /* 6E /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f6e },
  /* 6F /0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f6f },
  /* 70 /0 */ { BxPrefixSSE | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeGroupAVX128_0f70 },
  /* 71 /0 */ { BxGroup12, BX_IA_ERROR, BxOpcodeInfoAVX128G12R },
  /* 72 /0 */ { BxGroup13, BX_IA_ERROR, BxOpcodeInfoAVX128G13R },
  /* 73 /0 */ { BxGroup14, BX_IA_ERROR, BxOpcodeInfoAVX128G14R },
  /* 74 /0 */ { BxPrefixSSE66, BX_IA_V128_VPCMPEQB_VdqHdqWdq },
  /* 75 /0 */ { BxPrefixSSE66, BX_IA_V128_VPCMPEQW_VdqHdqWdq },
  /* 76 /0 */ { BxPrefixSSE66, BX_IA_V128_VPCMPEQD_VdqHdqWdq },
  /* 77 /0 */ { BxPrefixSSE, BX_IA_VZEROUPPER, BxOpcodeGroupSSE_ERR },
  /* 78 /0 */ { 0, BX_IA_ERROR },
  /* 79 /0 */ { 0, BX_IA_ERROR },
  /* 7A /0 */ { 0, BX_IA_ERROR },
  /* 7B /0 */ { 0, BX_IA_ERROR },
  /* 7C /0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f7c },
  /* 7D /0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f7d },
  /* 7E /0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f7e },
  /* 7F /0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f7f },
  /* 80 /0 */ { 0, BX_IA_ERROR },
  /* 81 /0 */ { 0, BX_IA_ERROR },
  /* 82 /0 */ { 0, BX_IA_ERROR },
  /* 83 /0 */ { 0, BX_IA_ERROR },
  /* 84 /0 */ { 0, BX_IA_ERROR },
  /* 85 /0 */ { 0, BX_IA_ERROR },
  /* 86 /0 */ { 0, BX_IA_ERROR },
  /* 87 /0 */ { 0, BX_IA_ERROR },
  /* 88 /0 */ { 0, BX_IA_ERROR },
  /* 89 /0 */ { 0, BX_IA_ERROR },
  /* 8A /0 */ { 0, BX_IA_ERROR },
  /* 8B /0 */ { 0, BX_IA_ERROR },
  /* 8C /0 */ { 0, BX_IA_ERROR },
  /* 8D /0 */ { 0, BX_IA_ERROR },
  /* 8E /0 */ { 0, BX_IA_ERROR },
  /* 8F /0 */ { 0, BX_IA_ERROR },
  /* 90 /0 */ { 0, BX_IA_ERROR },
  /* 91 /0 */ { 0, BX_IA_ERROR },
  /* 92 /0 */ { 0, BX_IA_ERROR },
  /* 93 /0 */ { 0, BX_IA_ERROR },
  /* 94 /0 */ { 0, BX_IA_ERROR },
  /* 95 /0 */ { 0, BX_IA_ERROR },
  /* 96 /0 */ { 0, BX_IA_ERROR },
  /* 97 /0 */ { 0, BX_IA_ERROR },
  /* 98 /0 */ { 0, BX_IA_ERROR },
  /* 99 /0 */ { 0, BX_IA_ERROR },
  /* 9A /0 */ { 0, BX_IA_ERROR },
  /* 9B /0 */ { 0, BX_IA_ERROR },
  /* 9C /0 */ { 0, BX_IA_ERROR },
  /* 9D /0 */ { 0, BX_IA_ERROR },
  /* 9E /0 */ { 0, BX_IA_ERROR },
  /* 9F /0 */ { 0, BX_IA_ERROR },
  /* A0 /0 */ { 0, BX_IA_ERROR },
  /* A1 /0 */ { 0, BX_IA_ERROR },
  /* A2 /0 */ { 0, BX_IA_ERROR },
  /* A3 /0 */ { 0, BX_IA_ERROR },
  /* A4 /0 */ { 0, BX_IA_ERROR },
  /* A5 /0 */ { 0, BX_IA_ERROR },
  /* A6 /0 */ { 0, BX_IA_ERROR },
  /* A7 /0 */ { 0, BX_IA_ERROR },
  /* A8 /0 */ { 0, BX_IA_ERROR },
  /* A9 /0 */ { 0, BX_IA_ERROR },
  /* AA /0 */ { 0, BX_IA_ERROR },
  /* AB /0 */ { 0, BX_IA_ERROR },
  /* AC /0 */ { 0, BX_IA_ERROR },
  /* AD /0 */ { 0, BX_IA_ERROR },
  /* AE /0 */ { BxGroupN, BX_IA_ERROR, BxOpcodeInfoAVXG15M },
  /* AF /0 */ { 0, BX_IA_ERROR },
  /* B0 /0 */ { 0, BX_IA_ERROR },
  /* B1 /0 */ { 0, BX_IA_ERROR },
  /* B2 /0 */ { 0, BX_IA_ERROR },
  /* B3 /0 */ { 0, BX_IA_ERROR },
  /* B4 /0 */ { 0, BX_IA_ERROR },
  /* B5 /0 */ { 0, BX_IA_ERROR },
  /* B6 /0 */ { 0, BX_IA_ERROR },
  /* B7 /0 */ { 0, BX_IA_ERROR },
  /* B8 /0 */ { 0, BX_IA_ERROR },
  /* B9 /0 */ { 0, BX_IA_ERROR },
  /* BA /0 */ { 0, BX_IA_ERROR },
  /* BB /0 */ { 0, BX_IA_ERROR },
  /* BC /0 */ { 0, BX_IA_ERROR },
  /* BD /0 */ { 0, BX_IA_ERROR },
  /* BE /0 */ { 0, BX_IA_ERROR },
  /* BF /0 */ { 0, BX_IA_ERROR },
  /* C0 /0 */ { 0, BX_IA_ERROR },
  /* C1 /0 */ { 0, BX_IA_ERROR },
  /* C2 /0 */ { BxPrefixSSE | BxImmediate_Ib, BX_IA_VCMPPS_VpsHpsWpsIb, BxOpcodeGroupAVX_0fc2 },
  /* C3 /0 */ { 0, BX_IA_ERROR },
  /* C4 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V128_VPINSRW_VdqEwIb },
  /* C5 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V128_VPEXTRW_GdUdqIb },
  /* C6 /0 */ { BxPrefixSSE | BxImmediate_Ib, BX_IA_VSHUFPS_VpsHpsWpsIb, BxOpcodeGroupAVX_0fc6 },
  /* C7 /0 */ { 0, BX_IA_ERROR },
  /* C8 /0 */ { 0, BX_IA_ERROR },
  /* C9 /0 */ { 0, BX_IA_ERROR },
  /* CA /0 */ { 0, BX_IA_ERROR },
  /* CB /0 */ { 0, BX_IA_ERROR },
  /* CC /0 */ { 0, BX_IA_ERROR },
  /* CD /0 */ { 0, BX_IA_ERROR },
  /* CE /0 */ { 0, BX_IA_ERROR },
  /* CF /0 */ { 0, BX_IA_ERROR },
  /* D0 /0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0fd0 },
  /* D1 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSRLW_VdqHdqWdq },
  /* D2 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSRLD_VdqHdqWdq },
  /* D3 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSRLQ_VdqHdqWdq },
  /* D4 /0 */ { BxPrefixSSE66, BX_IA_V128_VPADDQ_VdqHdqWdq },
  /* D5 /0 */ { BxPrefixSSE66, BX_IA_V128_VPMULLW_VdqHdqWdq },
  /* D6 /0 */ { BxPrefixSSE66, BX_IA_VMOVQ_WqVq },
  /* D7 /0 */ { BxPrefixSSE66, BX_IA_V128_VPMOVMSKB_GdUdq },
  /* D8 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSUBUSB_VdqHdqWdq },
  /* D9 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSUBUSW_VdqHdqWdq },
  /* DA /0 */ { BxPrefixSSE66, BX_IA_V128_VPMINUB_VdqHdqWdq },
  /* DB /0 */ { BxPrefixSSE66, BX_IA_V128_VPAND_VdqHdqWdq },
  /* DC /0 */ { BxPrefixSSE66, BX_IA_V128_VPADDUSB_VdqHdqWdq },
  /* DD /0 */ { BxPrefixSSE66, BX_IA_V128_VPADDUSW_VdqHdqWdq },
  /* DE /0 */ { BxPrefixSSE66, BX_IA_V128_VPMAXUB_VdqHdqWdq },
  /* DF /0 */ { BxPrefixSSE66, BX_IA_V128_VPANDN_VdqHdqWdq },
  /* E0 /0 */ { BxPrefixSSE66, BX_IA_V128_VPAVGB_VdqWdq },
  /* E1 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSRAW_VdqHdqWdq },
  /* E2 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSRAD_VdqHdqWdq },
  /* E3 /0 */ { BxPrefixSSE66, BX_IA_V128_VPAVGW_VdqWdq },
  /* E4 /0 */ { BxPrefixSSE66, BX_IA_V128_VPMULHUW_VdqHdqWdq },
  /* E5 /0 */ { BxPrefixSSE66, BX_IA_V128_VPMULHW_VdqHdqWdq },
  /* E6 /0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX128_0fe6 },
  /* E7 /0 */ { BxPrefixSSE66, BX_IA_VMOVNTDQ_MdqVdq },
  /* E8 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSUBSB_VdqHdqWdq },
  /* E9 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSUBSW_VdqHdqWdq },
  /* EA /0 */ { BxPrefixSSE66, BX_IA_V128_VPMINSW_VdqHdqWdq },
  /* EB /0 */ { BxPrefixSSE66, BX_IA_V128_VPOR_VdqHdqWdq },
  /* EC /0 */ { BxPrefixSSE66, BX_IA_V128_VPADDSB_VdqHdqWdq },
  /* ED /0 */ { BxPrefixSSE66, BX_IA_V128_VPADDSW_VdqHdqWdq },
  /* EE /0 */ { BxPrefixSSE66, BX_IA_V128_VPMAXSW_VdqHdqWdq },
  /* EF /0 */ { BxPrefixSSE66, BX_IA_V128_VPXOR_VdqHdqWdq },
  /* F0 /0 */ { BxPrefixSSEF2, BX_IA_VLDDQU_VdqMdq },
  /* F1 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSLLW_VdqHdqWdq },
  /* F2 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSLLD_VdqHdqWdq },
  /* F3 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSLLQ_VdqHdqWdq },
  /* F4 /0 */ { BxPrefixSSE66, BX_IA_V128_VPMULUDQ_VdqHdqWdq },
  /* F5 /0 */ { BxPrefixSSE66, BX_IA_V128_VPMADDWD_VdqHdqWdq },
  /* F6 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSADBW_VdqHdqWdq },
  /* F7 /0 */ { BxPrefixSSE66, BX_IA_V128_VMASKMOVDQU_VdqUdq },
  /* F8 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSUBB_VdqHdqWdq },
  /* F9 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSUBW_VdqHdqWdq },
  /* FA /0 */ { BxPrefixSSE66, BX_IA_V128_VPSUBD_VdqHdqWdq },
  /* FB /0 */ { BxPrefixSSE66, BX_IA_V128_VPSUBQ_VdqHdqWdq },
  /* FC /0 */ { BxPrefixSSE66, BX_IA_V128_VPADDB_VdqHdqWdq },
  /* FD /0 */ { BxPrefixSSE66, BX_IA_V128_VPADDW_VdqHdqWdq },
  /* FE /0 */ { BxPrefixSSE66, BX_IA_V128_VPADDD_VdqHdqWdq },
  /* FF /0 */ { 0, BX_IA_ERROR },

  // 256 entries for VEX-encoded 0x0F 0x38 opcodes (VEX.L=0)
  /* 00 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSHUFB_VdqHdqWdq },
  /* 01 /0 */ { BxPrefixSSE66, BX_IA_V128_VPHADDW_VdqHdqWdq },
  /* 02 /0 */ { BxPrefixSSE66, BX_IA_V128_VPHADDD_VdqHdqWdq },
  /* 03 /0 */ { BxPrefixSSE66, BX_IA_V128_VPHADDSW_VdqHdqWdq },
  /* 04 /0 */ { BxPrefixSSE66, BX_IA_V128_VPMADDUBSW_VdqHdqWdq },
  /* 05 /0 */ { BxPrefixSSE66, BX_IA_V128_VPHSUBW_VdqHdqWdq },
  /* 06 /0 */ { BxPrefixSSE66, BX_IA_V128_VPHSUBD_VdqHdqWdq },
  /* 07 /0 */ { BxPrefixSSE66, BX_IA_V128_VPHSUBSW_VdqHdqWdq },
  /* 08 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSIGNB_VdqHdqWdq },
  /* 09 /0 */ { BxPrefixSSE66, BX_IA_V128_VPSIGNW_VdqHdqWdq },
  /* 0A /0 */ { BxPrefixSSE66, BX_IA_V128_VPSIGND_VdqHdqWdq },
  /* 0B /0 */ { BxPrefixSSE66, BX_IA_V128_VPMULHRSW_VdqHdqWdq },
  /* 0C /0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VPERMILPS_VpsHpsWps },
  /* 0D /0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VPERMILPD_VpdHpdWpd },
  /* 0E /0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VTESTPS_VpsWps },
  /* 0F /0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VTESTPD_VpdWpd },
  /* 10 /0 */ { 0, BX_IA_ERROR },
  /* 11 /0 */ { 0, BX_IA_ERROR },
  /* 12 /0 */ { 0, BX_IA_ERROR },
  /* 13 /0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VCVTPH2PS_VpsWps },
  /* 14 /0 */ { 0, BX_IA_ERROR },
  /* 15 /0 */ { 0, BX_IA_ERROR },
  /* 16 /0 */ { 0, BX_IA_ERROR },
  /* 17 /0 */ { BxPrefixSSE66, BX_IA_VPTEST_VdqWdq },
  /* 18 /0 */ { BxSplitMod11B, BX_IA_ERROR, BxOpcodeGroupAVX_0f3818 },
  /* 19 /0 */ { 0, BX_IA_ERROR },
  /* 1A /0 */ { 0, BX_IA_ERROR },
  /* 1B /0 */ { 0, BX_IA_ERROR },
  /* 1C /0 */ { BxPrefixSSE66, BX_IA_V128_VPABSB_VdqWdq },
  /* 1D /0 */ { BxPrefixSSE66, BX_IA_V128_VPABSW_VdqWdq },
  /* 1E /0 */ { BxPrefixSSE66, BX_IA_V128_VPABSD_VdqWdq },
  /* 1F /0 */ { 0, BX_IA_ERROR },
  /* 20 /0 */ { BxPrefixSSE66, BX_IA_VPMOVSXBW_VdqWq },
  /* 21 /0 */ { BxPrefixSSE66, BX_IA_VPMOVSXBD_VdqWd },
  /* 22 /0 */ { BxPrefixSSE66, BX_IA_VPMOVSXBQ_VdqWw },
  /* 23 /0 */ { BxPrefixSSE66, BX_IA_VPMOVSXWD_VdqWq },
  /* 24 /0 */ { BxPrefixSSE66, BX_IA_VPMOVSXWQ_VdqWd },
  /* 25 /0 */ { BxPrefixSSE66, BX_IA_VPMOVSXDQ_VdqWq },
  /* 26 /0 */ { 0, BX_IA_ERROR },
  /* 27 /0 */ { 0, BX_IA_ERROR },
  /* 28 /0 */ { BxPrefixSSE66, BX_IA_V128_VPMULDQ_VdqHdqWdq },
  /* 29 /0 */ { BxPrefixSSE66, BX_IA_V128_VPCMPEQQ_VdqHdqWdq },
  /* 2A /0 */ { BxPrefixSSE66, BX_IA_V128_VMOVNTDQA_VdqMdq },
  /* 2B /0 */ { BxPrefixSSE66, BX_IA_V128_VPACKUSDW_VdqHdqWdq },
  /* 2C /0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VMASKMOVPS_VpsHpsMps },
  /* 2D /0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VMASKMOVPD_VpdHpdMpd },
  /* 2E /0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VMASKMOVPS_MpsHpsVps },
  /* 2F /0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VMASKMOVPD_MpdHpdVpd },
  /* 30 /0 */ { BxPrefixSSE66, BX_IA_VPMOVZXBW_VdqWq },
  /* 31 /0 */ { BxPrefixSSE66, BX_IA_VPMOVZXBD_VdqWd },
  /* 32 /0 */ { BxPrefixSSE66, BX_IA_VPMOVZXBQ_VdqWw },
  /* 33 /0 */ { BxPrefixSSE66, BX_IA_VPMOVZXWD_VdqWq },
  /* 34 /0 */ { BxPrefixSSE66, BX_IA_VPMOVZXWQ_VdqWd },
  /* 35 /0 */ { BxPrefixSSE66, BX_IA_VPMOVZXDQ_VdqWq },
  /* 36 /0 */ { 0, BX_IA_ERROR },
  /* 37 /0 */ { BxPrefixSSE66, BX_IA_V128_VPCMPGTQ_VdqHdqWdq },
  /* 38 /0 */ { BxPrefixSSE66, BX_IA_V128_VPMINSB_VdqHdqWdq },
  /* 39 /0 */ { BxPrefixSSE66, BX_IA_V128_VPMINSD_VdqHdqWdq },
  /* 3A /0 */ { BxPrefixSSE66, BX_IA_V128_VPMINUW_VdqHdqWdq },
  /* 3B /0 */ { BxPrefixSSE66, BX_IA_V128_VPMINUD_VdqHdqWdq },
  /* 3C /0 */ { BxPrefixSSE66, BX_IA_V128_VPMAXSB_VdqHdqWdq },
  /* 3D /0 */ { BxPrefixSSE66, BX_IA_V128_VPMAXSD_VdqHdqWdq },
  /* 3E /0 */ { BxPrefixSSE66, BX_IA_V128_VPMAXUW_VdqHdqWdq },
  /* 3F /0 */ { BxPrefixSSE66, BX_IA_V128_VPMAXUD_VdqHdqWdq },
  /* 40 /0 */ { BxPrefixSSE66, BX_IA_V128_VPMULLD_VdqHdqWdq },
  /* 41 /0 */ { BxPrefixSSE66, BX_IA_V128_VPHMINPOSUW_VdqWdq },
  /* 42 /0 */ { 0, BX_IA_ERROR },
  /* 43 /0 */ { 0, BX_IA_ERROR },
  /* 44 /0 */ { 0, BX_IA_ERROR },
  /* 45 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3845 },
  /* 46 /0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VPSRAVD_VdqHdqWdq },
  /* 47 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3847 },
  /* 48 /0 */ { 0, BX_IA_ERROR },
  /* 49 /0 */ { 0, BX_IA_ERROR },
  /* 4A /0 */ { 0, BX_IA_ERROR },
  /* 4B /0 */ { 0, BX_IA_ERROR },
  /* 4C /0 */ { 0, BX_IA_ERROR },
  /* 4D /0 */ { 0, BX_IA_ERROR },
  /* 4E /0 */ { 0, BX_IA_ERROR },
  /* 4F /0 */ { 0, BX_IA_ERROR },
  /* 50 /0 */ { 0, BX_IA_ERROR },
  /* 51 /0 */ { 0, BX_IA_ERROR },
  /* 52 /0 */ { 0, BX_IA_ERROR },
  /* 53 /0 */ { 0, BX_IA_ERROR },
  /* 54 /0 */ { 0, BX_IA_ERROR },
  /* 55 /0 */ { 0, BX_IA_ERROR },
  /* 56 /0 */ { 0, BX_IA_ERROR },
  /* 57 /0 */ { 0, BX_IA_ERROR },
  /* 58 /0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VPBROADCASTD_VdqWd },
  /* 59 /0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VPBROADCASTQ_VdqWq },
  /* 5A /0 */ { 0, BX_IA_ERROR },
  /* 5B /0 */ { 0, BX_IA_ERROR },
  /* 5C /0 */ { 0, BX_IA_ERROR },
  /* 5D /0 */ { 0, BX_IA_ERROR },
  /* 5E /0 */ { 0, BX_IA_ERROR },
  /* 5F /0 */ { 0, BX_IA_ERROR },
  /* 60 /0 */ { 0, BX_IA_ERROR },
  /* 61 /0 */ { 0, BX_IA_ERROR },
  /* 62 /0 */ { 0, BX_IA_ERROR },
  /* 63 /0 */ { 0, BX_IA_ERROR },
  /* 64 /0 */ { 0, BX_IA_ERROR },
  /* 65 /0 */ { 0, BX_IA_ERROR },
  /* 66 /0 */ { 0, BX_IA_ERROR },
  /* 67 /0 */ { 0, BX_IA_ERROR },
  /* 68 /0 */ { 0, BX_IA_ERROR },
  /* 69 /0 */ { 0, BX_IA_ERROR },
  /* 6A /0 */ { 0, BX_IA_ERROR },
  /* 6B /0 */ { 0, BX_IA_ERROR },
  /* 6C /0 */ { 0, BX_IA_ERROR },
  /* 6D /0 */ { 0, BX_IA_ERROR },
  /* 6E /0 */ { 0, BX_IA_ERROR },
  /* 6F /0 */ { 0, BX_IA_ERROR },
  /* 70 /0 */ { 0, BX_IA_ERROR },
  /* 71 /0 */ { 0, BX_IA_ERROR },
  /* 72 /0 */ { 0, BX_IA_ERROR },
  /* 73 /0 */ { 0, BX_IA_ERROR },
  /* 74 /0 */ { 0, BX_IA_ERROR },
  /* 75 /0 */ { 0, BX_IA_ERROR },
  /* 76 /0 */ { 0, BX_IA_ERROR },
  /* 77 /0 */ { 0, BX_IA_ERROR },
  /* 78 /0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VPBROADCASTB_VdqWb },
  /* 79 /0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VPBROADCASTW_VdqWw },
  /* 7A /0 */ { 0, BX_IA_ERROR },
  /* 7B /0 */ { 0, BX_IA_ERROR },
  /* 7C /0 */ { 0, BX_IA_ERROR },
  /* 7D /0 */ { 0, BX_IA_ERROR },
  /* 7E /0 */ { 0, BX_IA_ERROR },
  /* 7F /0 */ { 0, BX_IA_ERROR },
  /* 80 /0 */ { 0, BX_IA_ERROR },
  /* 81 /0 */ { 0, BX_IA_ERROR },
  /* 82 /0 */ { 0, BX_IA_ERROR },
  /* 83 /0 */ { 0, BX_IA_ERROR },
  /* 84 /0 */ { 0, BX_IA_ERROR },
  /* 85 /0 */ { 0, BX_IA_ERROR },
  /* 86 /0 */ { 0, BX_IA_ERROR },
  /* 87 /0 */ { 0, BX_IA_ERROR },
  /* 88 /0 */ { 0, BX_IA_ERROR },
  /* 89 /0 */ { 0, BX_IA_ERROR },
  /* 8A /0 */ { 0, BX_IA_ERROR },
  /* 8B /0 */ { 0, BX_IA_ERROR },
  /* 8C /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f388c },
  /* 8D /0 */ { 0, BX_IA_ERROR },
  /* 8E /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f388e },
  /* 8F /0 */ { 0, BX_IA_ERROR },
  /* 90 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3890 },
  /* 91 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3891 },
  /* 92 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3892 },
  /* 93 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3893 },
  /* 94 /0 */ { 0, BX_IA_ERROR },
  /* 95 /0 */ { 0, BX_IA_ERROR },
  /* 96 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3896 },
  /* 97 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3897 },
  /* 98 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3898 },
  /* 99 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3899 },
  /* 9A /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f389a },
  /* 9B /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f389b },
  /* 9C /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f389c },
  /* 9D /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f389d },
  /* 9E /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f389e },
  /* 9F /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f389f },
  /* A0 /0 */ { 0, BX_IA_ERROR },
  /* A1 /0 */ { 0, BX_IA_ERROR },
  /* A2 /0 */ { 0, BX_IA_ERROR },
  /* A3 /0 */ { 0, BX_IA_ERROR },
  /* A4 /0 */ { 0, BX_IA_ERROR },
  /* A5 /0 */ { 0, BX_IA_ERROR },
  /* A6 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38a6 },
  /* A7 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38a7 },
  /* A8 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38a8 },
  /* A9 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38a9 },
  /* AA /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38aa },
  /* AB /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38ab },
  /* AC /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38ac },
  /* AD /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38ad },
  /* AE /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38ae },
  /* AF /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38af },
  /* B0 /0 */ { 0, BX_IA_ERROR },
  /* B1 /0 */ { 0, BX_IA_ERROR },
  /* B2 /0 */ { 0, BX_IA_ERROR },
  /* B3 /0 */ { 0, BX_IA_ERROR },
  /* B4 /0 */ { 0, BX_IA_ERROR },
  /* B5 /0 */ { 0, BX_IA_ERROR },
  /* B6 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38b6 },
  /* B7 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38b7 },
  /* B8 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38b8 },
  /* B9 /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38b9 },
  /* BA /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38ba },
  /* BB /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38bb },
  /* BC /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38bc },
  /* BD /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38bd },
  /* BE /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38be },
  /* BF /0 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38bf },
  /* C0 /0 */ { 0, BX_IA_ERROR },
  /* C1 /0 */ { 0, BX_IA_ERROR },
  /* C2 /0 */ { 0, BX_IA_ERROR },
  /* C3 /0 */ { 0, BX_IA_ERROR },
  /* C4 /0 */ { 0, BX_IA_ERROR },
  /* C5 /0 */ { 0, BX_IA_ERROR },
  /* C6 /0 */ { 0, BX_IA_ERROR },
  /* C7 /0 */ { 0, BX_IA_ERROR },
  /* C8 /0 */ { 0, BX_IA_ERROR },
  /* C9 /0 */ { 0, BX_IA_ERROR },
  /* CA /0 */ { 0, BX_IA_ERROR },
  /* CB /0 */ { 0, BX_IA_ERROR },
  /* CC /0 */ { 0, BX_IA_ERROR },
  /* CD /0 */ { 0, BX_IA_ERROR },
  /* CE /0 */ { 0, BX_IA_ERROR },
  /* CF /0 */ { 0, BX_IA_ERROR },
  /* D0 /0 */ { 0, BX_IA_ERROR },
  /* D1 /0 */ { 0, BX_IA_ERROR },
  /* D2 /0 */ { 0, BX_IA_ERROR },
  /* D3 /0 */ { 0, BX_IA_ERROR },
  /* D4 /0 */ { 0, BX_IA_ERROR },
  /* D5 /0 */ { 0, BX_IA_ERROR },
  /* D6 /0 */ { 0, BX_IA_ERROR },
  /* D7 /0 */ { 0, BX_IA_ERROR },
  /* D8 /0 */ { 0, BX_IA_ERROR },
  /* D9 /0 */ { 0, BX_IA_ERROR },
  /* DA /0 */ { 0, BX_IA_ERROR },
  /* DB /0 */ { BxPrefixSSE66, BX_IA_V128_VAESIMC_VdqWdq },
  /* DC /0 */ { BxPrefixSSE66, BX_IA_V128_VAESENC_VdqHdqWdq },
  /* DD /0 */ { BxPrefixSSE66, BX_IA_V128_VAESENCLAST_VdqHdqWdq },
  /* DE /0 */ { BxPrefixSSE66, BX_IA_V128_VAESDEC_VdqHdqWdq },
  /* DF /0 */ { BxPrefixSSE66, BX_IA_V128_VAESDECLAST_VdqHdqWdq },
  /* E0 /0 */ { 0, BX_IA_ERROR },
  /* E1 /0 */ { 0, BX_IA_ERROR },
  /* E2 /0 */ { 0, BX_IA_ERROR },
  /* E3 /0 */ { 0, BX_IA_ERROR },
  /* E4 /0 */ { 0, BX_IA_ERROR },
  /* E5 /0 */ { 0, BX_IA_ERROR },
  /* E6 /0 */ { 0, BX_IA_ERROR },
  /* E7 /0 */ { 0, BX_IA_ERROR },
  /* E8 /0 */ { 0, BX_IA_ERROR },
  /* E9 /0 */ { 0, BX_IA_ERROR },
  /* EA /0 */ { 0, BX_IA_ERROR },
  /* EB /0 */ { 0, BX_IA_ERROR },
  /* EC /0 */ { 0, BX_IA_ERROR },
  /* ED /0 */ { 0, BX_IA_ERROR },
  /* EE /0 */ { 0, BX_IA_ERROR },
  /* EF /0 */ { 0, BX_IA_ERROR },
  /* F0 /0 */ { 0, BX_IA_ERROR },
  /* F1 /0 */ { 0, BX_IA_ERROR },
  /* F2 /0 */ { BxSplitVexW64, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38f2 },
  /* F3 /0 */ { BxSplitVexW64, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38f3 },
  /* F4 /0 */ { 0, BX_IA_ERROR },
  /* F5 /0 */ { BxSplitVexW64, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38f5 },
  /* F6 /0 */ { BxSplitVexW64, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38f6 },
  /* F7 /0 */ { BxSplitVexW64, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38f7 },
  /* F8 /0 */ { 0, BX_IA_ERROR },
  /* F9 /0 */ { 0, BX_IA_ERROR },
  /* FA /0 */ { 0, BX_IA_ERROR },
  /* FB /0 */ { 0, BX_IA_ERROR },
  /* FC /0 */ { 0, BX_IA_ERROR },
  /* FD /0 */ { 0, BX_IA_ERROR },
  /* FE /0 */ { 0, BX_IA_ERROR },
  /* FF /0 */ { 0, BX_IA_ERROR },

  // 256 entries for VEX-encoded 0x0F 0x3A opcodes (VEX.L=0)
  /* 00 /0 */ { 0, BX_IA_ERROR },
  /* 01 /0 */ { 0, BX_IA_ERROR },
  /* 02 /0 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_VPBLENDD_VdqHdqWdqIb },
  /* 03 /0 */ { 0, BX_IA_ERROR },
  /* 04 /0 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_VPERMILPS_VpsWpsIb },
  /* 05 /0 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_VPERMILPD_VpdWpdIb },
  /* 06 /0 */ { 0, BX_IA_ERROR },
  /* 07 /0 */ { 0, BX_IA_ERROR },
  /* 08 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VROUNDPS_VpsWpsIb },
  /* 09 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VROUNDPD_VpdWpdIb },
  /* 0A /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VROUNDSS_VssHpsWssIb },
  /* 0B /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VROUNDSD_VsdHpdWsdIb },
  /* 0C /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VBLENDPS_VpsHpsWpsIb },
  /* 0D /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VBLENDPD_VpdHpdWpdIb },
  /* 0E /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V128_VPBLENDW_VdqHdqWdqIb },
  /* 0F /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V128_VPALIGNR_VdqHdqWdqIb },
  /* 10 /0 */ { 0, BX_IA_ERROR },
  /* 11 /0 */ { 0, BX_IA_ERROR },
  /* 12 /0 */ { 0, BX_IA_ERROR },
  /* 13 /0 */ { 0, BX_IA_ERROR },
  /* 14 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V128_VPEXTRB_EbdVdqIb },
  /* 15 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V128_VPEXTRW_EwdVdqIb },
  /* 16 /0 */ { BxSplitVexW | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeInfoAVX128_VexW_0f3a16 },
  /* 17 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V128_VEXTRACTPS_EdVpsIb },
  /* 18 /0 */ { 0, BX_IA_ERROR },
  /* 19 /0 */ { 0, BX_IA_ERROR },
  /* 1A /0 */ { 0, BX_IA_ERROR },
  /* 1B /0 */ { 0, BX_IA_ERROR },
  /* 1C /0 */ { 0, BX_IA_ERROR },
  /* 1D /0 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_VCVTPS2PH_WpsVpsIb },
  /* 1E /0 */ { 0, BX_IA_ERROR },
  /* 1F /0 */ { 0, BX_IA_ERROR },
  /* 20 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V128_VPINSRB_VdqEbIb },
  /* 21 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V128_VINSERTPS_VpsWssIb },
  /* 22 /0 */ { BxSplitVexW | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeInfoAVX128_VexW_0f3a22 },
  /* 23 /0 */ { 0, BX_IA_ERROR },
  /* 24 /0 */ { 0, BX_IA_ERROR },
  /* 25 /0 */ { 0, BX_IA_ERROR },
  /* 26 /0 */ { 0, BX_IA_ERROR },
  /* 27 /0 */ { 0, BX_IA_ERROR },
  /* 28 /0 */ { 0, BX_IA_ERROR },
  /* 29 /0 */ { 0, BX_IA_ERROR },
  /* 2A /0 */ { 0, BX_IA_ERROR },
  /* 2B /0 */ { 0, BX_IA_ERROR },
  /* 2C /0 */ { 0, BX_IA_ERROR },
  /* 2D /0 */ { 0, BX_IA_ERROR },
  /* 2E /0 */ { 0, BX_IA_ERROR },
  /* 2F /0 */ { 0, BX_IA_ERROR },
  /* 30 /0 */ { 0, BX_IA_ERROR },
  /* 31 /0 */ { 0, BX_IA_ERROR },
  /* 32 /0 */ { 0, BX_IA_ERROR },
  /* 33 /0 */ { 0, BX_IA_ERROR },
  /* 34 /0 */ { 0, BX_IA_ERROR },
  /* 35 /0 */ { 0, BX_IA_ERROR },
  /* 36 /0 */ { 0, BX_IA_ERROR },
  /* 37 /0 */ { 0, BX_IA_ERROR },
  /* 38 /0 */ { 0, BX_IA_ERROR },
  /* 39 /0 */ { 0, BX_IA_ERROR },
  /* 3A /0 */ { 0, BX_IA_ERROR },
  /* 3B /0 */ { 0, BX_IA_ERROR },
  /* 3C /0 */ { 0, BX_IA_ERROR },
  /* 3D /0 */ { 0, BX_IA_ERROR },
  /* 3E /0 */ { 0, BX_IA_ERROR },
  /* 3F /0 */ { 0, BX_IA_ERROR },
  /* 40 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VDPPS_VpsHpsWpsIb },
  /* 41 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VDPPD_VpdHpdWpdIb },
  /* 42 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V128_VMPSADBW_VdqWdqIb },
  /* 43 /0 */ { 0, BX_IA_ERROR },
  /* 44 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V128_VPCLMULQDQ_VdqHdqWdqIb },
  /* 45 /0 */ { 0, BX_IA_ERROR },
  /* 46 /0 */ { 0, BX_IA_ERROR },
  /* 47 /0 */ { 0, BX_IA_ERROR },
  /* 48 /0 */ { BxPrefixSSE66 | BxImmediate_Ib5, BX_IA_VPERMIL2PS_VdqHdqWdqIb },
  /* 49 /0 */ { BxPrefixSSE66 | BxImmediate_Ib5, BX_IA_VPERMIL2PD_VdqHdqWdqIb },
  /* 4A /0 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib4, BX_IA_VBLENDVPS_VpsHpsWpsIb },
  /* 4B /0 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib4, BX_IA_VBLENDVPD_VpdHpdWpdIb },
  /* 4C /0 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib4, BX_IA_V128_VPBLENDVB_VdqHdqWdqIb },
  /* 4D /0 */ { 0, BX_IA_ERROR },
  /* 4E /0 */ { 0, BX_IA_ERROR },
  /* 4F /0 */ { 0, BX_IA_ERROR },
  /* 50 /0 */ { 0, BX_IA_ERROR },
  /* 51 /0 */ { 0, BX_IA_ERROR },
  /* 52 /0 */ { 0, BX_IA_ERROR },
  /* 53 /0 */ { 0, BX_IA_ERROR },
  /* 54 /0 */ { 0, BX_IA_ERROR },
  /* 55 /0 */ { 0, BX_IA_ERROR },
  /* 56 /0 */ { 0, BX_IA_ERROR },
  /* 57 /0 */ { 0, BX_IA_ERROR },
  /* 58 /0 */ { 0, BX_IA_ERROR },
  /* 59 /0 */ { 0, BX_IA_ERROR },
  /* 5A /0 */ { 0, BX_IA_ERROR },
  /* 5B /0 */ { 0, BX_IA_ERROR },
  /* 5C /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMADDSUBPS_VpsHpsWpsVIb },
  /* 5D /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMADDSUBPD_VpdHpdWpdVIb },
  /* 5E /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMSUBADDPS_VpsHpsWpsVIb },
  /* 5F /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMSUBADDPD_VpdHpdWpdVIb },
  /* 60 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V128_VPCMPESTRM_VdqWdqIb },
  /* 61 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V128_VPCMPESTRI_VdqWdqIb },
  /* 62 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V128_VPCMPISTRM_VdqWdqIb },
  /* 63 /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V128_VPCMPISTRI_VdqWdqIb },
  /* 64 /0 */ { 0, BX_IA_ERROR },
  /* 65 /0 */ { 0, BX_IA_ERROR },
  /* 66 /0 */ { 0, BX_IA_ERROR },
  /* 67 /0 */ { 0, BX_IA_ERROR },
  /* 68 /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMADDPS_VpsHpsWpsVIb },
  /* 69 /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMADDPD_VpdHpdWpdVIb },
  /* 6A /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMADDSS_VssHssWssVIb },
  /* 6B /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMADDSD_VsdHsdWsdVIb },
  /* 6C /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMSUBPS_VpsHpsWpsVIb },
  /* 6D /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMSUBPD_VpdHpdWpdVIb },
  /* 6E /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMSUBSS_VssHssWssVIb },
  /* 6F /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMSUBSD_VsdHsdWsdVIb },
  /* 70 /0 */ { 0, BX_IA_ERROR },
  /* 71 /0 */ { 0, BX_IA_ERROR },
  /* 72 /0 */ { 0, BX_IA_ERROR },
  /* 73 /0 */ { 0, BX_IA_ERROR },
  /* 74 /0 */ { 0, BX_IA_ERROR },
  /* 75 /0 */ { 0, BX_IA_ERROR },
  /* 76 /0 */ { 0, BX_IA_ERROR },
  /* 77 /0 */ { 0, BX_IA_ERROR },
  /* 78 /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFNMADDPS_VpsHpsWpsVIb },
  /* 79 /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFNMADDPD_VpdHpdWpdVIb },
  /* 7A /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFNMADDSS_VssHssWssVIb },
  /* 7B /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFNMADDSD_VsdHsdWsdVIb },
  /* 7C /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFNMSUBPS_VpsHpsWpsVIb },
  /* 7D /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFNMSUBPD_VpdHpdWpdVIb },
  /* 7E /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFNMSUBSS_VssHssWssVIb },
  /* 7F /0 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFNMSUBSD_VsdHsdWsdVIb },
  /* 80 /0 */ { 0, BX_IA_ERROR },
  /* 81 /0 */ { 0, BX_IA_ERROR },
  /* 82 /0 */ { 0, BX_IA_ERROR },
  /* 83 /0 */ { 0, BX_IA_ERROR },
  /* 84 /0 */ { 0, BX_IA_ERROR },
  /* 85 /0 */ { 0, BX_IA_ERROR },
  /* 86 /0 */ { 0, BX_IA_ERROR },
  /* 87 /0 */ { 0, BX_IA_ERROR },
  /* 88 /0 */ { 0, BX_IA_ERROR },
  /* 89 /0 */ { 0, BX_IA_ERROR },
  /* 8A /0 */ { 0, BX_IA_ERROR },
  /* 8B /0 */ { 0, BX_IA_ERROR },
  /* 8C /0 */ { 0, BX_IA_ERROR },
  /* 8D /0 */ { 0, BX_IA_ERROR },
  /* 8E /0 */ { 0, BX_IA_ERROR },
  /* 8F /0 */ { 0, BX_IA_ERROR },
  /* 90 /0 */ { 0, BX_IA_ERROR },
  /* 91 /0 */ { 0, BX_IA_ERROR },
  /* 92 /0 */ { 0, BX_IA_ERROR },
  /* 93 /0 */ { 0, BX_IA_ERROR },
  /* 94 /0 */ { 0, BX_IA_ERROR },
  /* 95 /0 */ { 0, BX_IA_ERROR },
  /* 96 /0 */ { 0, BX_IA_ERROR },
  /* 97 /0 */ { 0, BX_IA_ERROR },
  /* 98 /0 */ { 0, BX_IA_ERROR },
  /* 99 /0 */ { 0, BX_IA_ERROR },
  /* 9A /0 */ { 0, BX_IA_ERROR },
  /* 9B /0 */ { 0, BX_IA_ERROR },
  /* 9C /0 */ { 0, BX_IA_ERROR },
  /* 9D /0 */ { 0, BX_IA_ERROR },
  /* 9E /0 */ { 0, BX_IA_ERROR },
  /* 9F /0 */ { 0, BX_IA_ERROR },
  /* A0 /0 */ { 0, BX_IA_ERROR },
  /* A1 /0 */ { 0, BX_IA_ERROR },
  /* A2 /0 */ { 0, BX_IA_ERROR },
  /* A3 /0 */ { 0, BX_IA_ERROR },
  /* A4 /0 */ { 0, BX_IA_ERROR },
  /* A5 /0 */ { 0, BX_IA_ERROR },
  /* A6 /0 */ { 0, BX_IA_ERROR },
  /* A7 /0 */ { 0, BX_IA_ERROR },
  /* A8 /0 */ { 0, BX_IA_ERROR },
  /* A9 /0 */ { 0, BX_IA_ERROR },
  /* AA /0 */ { 0, BX_IA_ERROR },
  /* AB /0 */ { 0, BX_IA_ERROR },
  /* AC /0 */ { 0, BX_IA_ERROR },
  /* AD /0 */ { 0, BX_IA_ERROR },
  /* AE /0 */ { 0, BX_IA_ERROR },
  /* AF /0 */ { 0, BX_IA_ERROR },
  /* B0 /0 */ { 0, BX_IA_ERROR },
  /* B1 /0 */ { 0, BX_IA_ERROR },
  /* B2 /0 */ { 0, BX_IA_ERROR },
  /* B3 /0 */ { 0, BX_IA_ERROR },
  /* B4 /0 */ { 0, BX_IA_ERROR },
  /* B5 /0 */ { 0, BX_IA_ERROR },
  /* B6 /0 */ { 0, BX_IA_ERROR },
  /* B7 /0 */ { 0, BX_IA_ERROR },
  /* B8 /0 */ { 0, BX_IA_ERROR },
  /* B9 /0 */ { 0, BX_IA_ERROR },
  /* BA /0 */ { 0, BX_IA_ERROR },
  /* BB /0 */ { 0, BX_IA_ERROR },
  /* BC /0 */ { 0, BX_IA_ERROR },
  /* BD /0 */ { 0, BX_IA_ERROR },
  /* BE /0 */ { 0, BX_IA_ERROR },
  /* BF /0 */ { 0, BX_IA_ERROR },
  /* C0 /0 */ { 0, BX_IA_ERROR },
  /* C1 /0 */ { 0, BX_IA_ERROR },
  /* C2 /0 */ { 0, BX_IA_ERROR },
  /* C3 /0 */ { 0, BX_IA_ERROR },
  /* C4 /0 */ { 0, BX_IA_ERROR },
  /* C5 /0 */ { 0, BX_IA_ERROR },
  /* C6 /0 */ { 0, BX_IA_ERROR },
  /* C7 /0 */ { 0, BX_IA_ERROR },
  /* C8 /0 */ { 0, BX_IA_ERROR },
  /* C9 /0 */ { 0, BX_IA_ERROR },
  /* CA /0 */ { 0, BX_IA_ERROR },
  /* CB /0 */ { 0, BX_IA_ERROR },
  /* CC /0 */ { 0, BX_IA_ERROR },
  /* CD /0 */ { 0, BX_IA_ERROR },
  /* CE /0 */ { 0, BX_IA_ERROR },
  /* CF /0 */ { 0, BX_IA_ERROR },
  /* D0 /0 */ { 0, BX_IA_ERROR },
  /* D1 /0 */ { 0, BX_IA_ERROR },
  /* D2 /0 */ { 0, BX_IA_ERROR },
  /* D3 /0 */ { 0, BX_IA_ERROR },
  /* D4 /0 */ { 0, BX_IA_ERROR },
  /* D5 /0 */ { 0, BX_IA_ERROR },
  /* D6 /0 */ { 0, BX_IA_ERROR },
  /* D7 /0 */ { 0, BX_IA_ERROR },
  /* D8 /0 */ { 0, BX_IA_ERROR },
  /* D9 /0 */ { 0, BX_IA_ERROR },
  /* DA /0 */ { 0, BX_IA_ERROR },
  /* DB /0 */ { 0, BX_IA_ERROR },
  /* DC /0 */ { 0, BX_IA_ERROR },
  /* DD /0 */ { 0, BX_IA_ERROR },
  /* DE /0 */ { 0, BX_IA_ERROR },
  /* DF /0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V128_VAESKEYGENASSIST_VdqWdqIb },
  /* E0 /0 */ { 0, BX_IA_ERROR },
  /* E1 /0 */ { 0, BX_IA_ERROR },
  /* E2 /0 */ { 0, BX_IA_ERROR },
  /* E3 /0 */ { 0, BX_IA_ERROR },
  /* E4 /0 */ { 0, BX_IA_ERROR },
  /* E5 /0 */ { 0, BX_IA_ERROR },
  /* E6 /0 */ { 0, BX_IA_ERROR },
  /* E7 /0 */ { 0, BX_IA_ERROR },
  /* E8 /0 */ { 0, BX_IA_ERROR },
  /* E9 /0 */ { 0, BX_IA_ERROR },
  /* EA /0 */ { 0, BX_IA_ERROR },
  /* EB /0 */ { 0, BX_IA_ERROR },
  /* EC /0 */ { 0, BX_IA_ERROR },
  /* ED /0 */ { 0, BX_IA_ERROR },
  /* EE /0 */ { 0, BX_IA_ERROR },
  /* EF /0 */ { 0, BX_IA_ERROR },
  /* F0 /0 */ { BxSplitVexW64 | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3af0 },
  /* F1 /0 */ { 0, BX_IA_ERROR },
  /* F2 /0 */ { 0, BX_IA_ERROR },
  /* F3 /0 */ { 0, BX_IA_ERROR },
  /* F4 /0 */ { 0, BX_IA_ERROR },
  /* F5 /0 */ { 0, BX_IA_ERROR },
  /* F6 /0 */ { 0, BX_IA_ERROR },
  /* F7 /0 */ { 0, BX_IA_ERROR },
  /* F8 /0 */ { 0, BX_IA_ERROR },
  /* F9 /0 */ { 0, BX_IA_ERROR },
  /* FA /0 */ { 0, BX_IA_ERROR },
  /* FB /0 */ { 0, BX_IA_ERROR },
  /* FC /0 */ { 0, BX_IA_ERROR },
  /* FD /0 */ { 0, BX_IA_ERROR },
  /* FE /0 */ { 0, BX_IA_ERROR },
  /* FF /0 */ { 0, BX_IA_ERROR },

  // 256 entries for VEX-encoded 0x0F opcodes (VEX.L=1)
  /* 00 /1 */ { 0, BX_IA_ERROR },
  /* 01 /1 */ { 0, BX_IA_ERROR },
  /* 02 /1 */ { 0, BX_IA_ERROR },
  /* 03 /1 */ { 0, BX_IA_ERROR },
  /* 04 /1 */ { 0, BX_IA_ERROR },
  /* 05 /1 */ { 0, BX_IA_ERROR },
  /* 06 /1 */ { 0, BX_IA_ERROR },
  /* 07 /1 */ { 0, BX_IA_ERROR },
  /* 08 /1 */ { 0, BX_IA_ERROR },
  /* 09 /1 */ { 0, BX_IA_ERROR },
  /* 0A /1 */ { 0, BX_IA_ERROR },
  /* 0B /1 */ { 0, BX_IA_ERROR },
  /* 0C /1 */ { 0, BX_IA_ERROR },
  /* 0D /1 */ { 0, BX_IA_ERROR },
  /* 0E /1 */ { 0, BX_IA_ERROR },
  /* 0F /1 */ { 0, BX_IA_ERROR },
  /* 10 /1 */ { BxPrefixSSE, BX_IA_VMOVUPS_VpsWps, BxOpcodeGroupAVX256_0f10 },
  /* 11 /1 */ { BxPrefixSSE, BX_IA_VMOVUPS_WpsVps, BxOpcodeGroupAVX256_0f11 },
  /* 12 /1 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX256_0f12 },
  /* 13 /1 */ { 0, BX_IA_ERROR },
  /* 14 /1 */ { BxPrefixSSE, BX_IA_VUNPCKLPS_VpsHpsWps, BxOpcodeGroupAVX_0f14 },
  /* 15 /1 */ { BxPrefixSSE, BX_IA_VUNPCKHPS_VpsHpsWps, BxOpcodeGroupAVX_0f15 },
  /* 16 /1 */ { BxPrefixSSEF3, BX_IA_VMOVSHDUP_VpsWps },
  /* 17 /1 */ { 0, BX_IA_ERROR },
  /* 18 /1 */ { 0, BX_IA_ERROR },
  /* 19 /1 */ { 0, BX_IA_ERROR },
  /* 1A /1 */ { 0, BX_IA_ERROR },
  /* 1B /1 */ { 0, BX_IA_ERROR },
  /* 1C /1 */ { 0, BX_IA_ERROR },
  /* 1D /1 */ { 0, BX_IA_ERROR },
  /* 1E /1 */ { 0, BX_IA_ERROR },
  /* 1F /1 */ { 0, BX_IA_ERROR },
  /* 20 /1 */ { 0, BX_IA_ERROR }, // WARNING: ModC0 always '1
  /* 21 /1 */ { 0, BX_IA_ERROR }, // WARNING: ModC0 always '1
  /* 22 /1 */ { 0, BX_IA_ERROR }, // WARNING: ModC0 always '1
  /* 23 /1 */ { 0, BX_IA_ERROR }, // WARNING: ModC0 always '1
  /* 24 /1 */ { 0, BX_IA_ERROR },
  /* 25 /1 */ { 0, BX_IA_ERROR },
  /* 26 /1 */ { 0, BX_IA_ERROR },
  /* 27 /1 */ { 0, BX_IA_ERROR },
  /* 28 /1 */ { BxPrefixSSE, BX_IA_VMOVAPS_VpsWps, BxOpcodeGroupAVX_0f28 },
  /* 29 /1 */ { BxPrefixSSE, BX_IA_VMOVAPS_WpsVps, BxOpcodeGroupAVX_0f29 },
  /* 2A /1 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f2a },
  /* 2B /1 */ { BxPrefixSSE, BX_IA_VMOVNTPS_MpsVps, BxOpcodeGroupAVX_0f2bM },
  /* 2C /1 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f2c },
  /* 2D /1 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f2d },
  /* 2E /1 */ { BxPrefixSSE, BX_IA_VUCOMISS_VssWss, BxOpcodeGroupAVX_0f2e },
  /* 2F /1 */ { BxPrefixSSE, BX_IA_VCOMISS_VpsWps, BxOpcodeGroupAVX_0f2f },
  /* 30 /1 */ { 0, BX_IA_ERROR },
  /* 31 /1 */ { 0, BX_IA_ERROR },
  /* 32 /1 */ { 0, BX_IA_ERROR },
  /* 33 /1 */ { 0, BX_IA_ERROR },
  /* 34 /1 */ { 0, BX_IA_ERROR },
  /* 35 /1 */ { 0, BX_IA_ERROR },
  /* 36 /1 */ { 0, BX_IA_ERROR },
  /* 37 /1 */ { 0, BX_IA_ERROR },
  /* 38 /1 */ { 0, BX_IA_ERROR }, // 3-Byte Escape
  /* 39 /1 */ { 0, BX_IA_ERROR },
  /* 3A /1 */ { 0, BX_IA_ERROR }, // 3-Byte Escape
  /* 3B /1 */ { 0, BX_IA_ERROR },
  /* 3C /1 */ { 0, BX_IA_ERROR },
  /* 3D /1 */ { 0, BX_IA_ERROR },
  /* 3E /1 */ { 0, BX_IA_ERROR },
  /* 3F /1 */ { 0, BX_IA_ERROR },
  /* 40 /1 */ { 0, BX_IA_ERROR },
  /* 41 /1 */ { 0, BX_IA_ERROR },
  /* 42 /1 */ { 0, BX_IA_ERROR },
  /* 43 /1 */ { 0, BX_IA_ERROR },
  /* 44 /1 */ { 0, BX_IA_ERROR },
  /* 45 /1 */ { 0, BX_IA_ERROR },
  /* 46 /1 */ { 0, BX_IA_ERROR },
  /* 47 /1 */ { 0, BX_IA_ERROR },
  /* 48 /1 */ { 0, BX_IA_ERROR },
  /* 49 /1 */ { 0, BX_IA_ERROR },
  /* 4A /1 */ { 0, BX_IA_ERROR },
  /* 4B /1 */ { 0, BX_IA_ERROR },
  /* 4C /1 */ { 0, BX_IA_ERROR },
  /* 4D /1 */ { 0, BX_IA_ERROR },
  /* 4E /1 */ { 0, BX_IA_ERROR },
  /* 4F /1 */ { 0, BX_IA_ERROR },
  /* 50 /1 */ { BxPrefixSSE, BX_IA_VMOVMSKPS_GdVRps, BxOpcodeGroupAVX_0f50R },
  /* 51 /1 */ { BxPrefixSSE, BX_IA_VSQRTPS_VpsWps, BxOpcodeGroupAVX_0f51 },
  /* 52 /1 */ { BxPrefixSSE, BX_IA_VRSQRTPS_VpsWps, BxOpcodeGroupAVX_0f52 },
  /* 53 /1 */ { BxPrefixSSE, BX_IA_VRCPPS_VpsWps, BxOpcodeGroupAVX_0f53 },
  /* 54 /1 */ { BxPrefixSSE, BX_IA_VANDPS_VpsHpsWps, BxOpcodeGroupAVX_0f54 },
  /* 55 /1 */ { BxPrefixSSE, BX_IA_VANDNPS_VpsHpsWps, BxOpcodeGroupAVX_0f55 },
  /* 56 /1 */ { BxPrefixSSE, BX_IA_VORPS_VpsHpsWps, BxOpcodeGroupAVX_0f56 },
  /* 57 /1 */ { BxPrefixSSE, BX_IA_VXORPS_VpsHpsWps, BxOpcodeGroupAVX_0f57 },
  /* 58 /1 */ { BxPrefixSSE, BX_IA_VADDPS_VpsHpsWps, BxOpcodeGroupAVX_0f58 },
  /* 59 /1 */ { BxPrefixSSE, BX_IA_VMULPS_VpsHpsWps, BxOpcodeGroupAVX_0f59 },
  /* 5A /1 */ { BxPrefixSSE, BX_IA_V256_VCVTPS2PD_VpdWps, BxOpcodeGroupAVX_0f5a },
  /* 5B /1 */ { BxPrefixSSE, BX_IA_VCVTDQ2PS_VpsWdq, BxOpcodeGroupAVX_0f5b },
  /* 5C /1 */ { BxPrefixSSE, BX_IA_VSUBPS_VpsHpsWps, BxOpcodeGroupAVX_0f5c },
  /* 5D /1 */ { BxPrefixSSE, BX_IA_VMINPS_VpsHpsWps, BxOpcodeGroupAVX_0f5d },
  /* 5E /1 */ { BxPrefixSSE, BX_IA_VDIVPS_VpsHpsWps, BxOpcodeGroupAVX_0f5e },
  /* 5F /1 */ { BxPrefixSSE, BX_IA_VMAXPS_VpsHpsWps, BxOpcodeGroupAVX_0f5f },
  /* 60 /1 */ { BxPrefixSSE66, BX_IA_V256_VPUNPCKLBW_VdqHdqWdq },
  /* 61 /1 */ { BxPrefixSSE66, BX_IA_V256_VPUNPCKLWD_VdqHdqWdq },
  /* 62 /1 */ { BxPrefixSSE66, BX_IA_V256_VPUNPCKLDQ_VdqHdqWdq },
  /* 63 /1 */ { BxPrefixSSE66, BX_IA_V256_VPACKSSWB_VdqHdqWdq },
  /* 64 /1 */ { BxPrefixSSE66, BX_IA_V256_VPCMPGTB_VdqHdqWdq },
  /* 65 /1 */ { BxPrefixSSE66, BX_IA_V256_VPCMPGTW_VdqHdqWdq },
  /* 66 /1 */ { BxPrefixSSE66, BX_IA_V256_VPCMPGTD_VdqHdqWdq },
  /* 67 /1 */ { BxPrefixSSE66, BX_IA_V256_VPACKUSWB_VdqHdqWdq },
  /* 68 /1 */ { BxPrefixSSE66, BX_IA_V256_VPUNPCKHBW_VdqHdqWdq },
  /* 69 /1 */ { BxPrefixSSE66, BX_IA_V256_VPUNPCKHWD_VdqHdqWdq },
  /* 6A /1 */ { BxPrefixSSE66, BX_IA_V256_VPUNPCKHDQ_VdqHdqWdq },
  /* 6B /1 */ { BxPrefixSSE66, BX_IA_V256_VPACKSSDW_VdqHdqWdq },
  /* 6C /1 */ { BxPrefixSSE66, BX_IA_V256_VPUNPCKLQDQ_VdqHdqWdq },
  /* 6D /1 */ { BxPrefixSSE66, BX_IA_V256_VPUNPCKHQDQ_VdqHdqWdq },
  /* 6E /1 */ { 0, BX_IA_ERROR },
  /* 6F /1 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f6f },
  /* 70 /1 */ { BxPrefixSSE | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeGroupAVX256_0f70 },
  /* 71 /1 */ { BxGroup12, BX_IA_ERROR, BxOpcodeInfoAVX256G12R },
  /* 72 /1 */ { BxGroup13, BX_IA_ERROR, BxOpcodeInfoAVX256G13R },
  /* 73 /1 */ { BxGroup14, BX_IA_ERROR, BxOpcodeInfoAVX256G14R },
  /* 74 /1 */ { BxPrefixSSE66, BX_IA_V256_VPCMPEQB_VdqHdqWdq },
  /* 75 /1 */ { BxPrefixSSE66, BX_IA_V256_VPCMPEQW_VdqHdqWdq },
  /* 76 /1 */ { BxPrefixSSE66, BX_IA_V256_VPCMPEQD_VdqHdqWdq },
  /* 77 /1 */ { BxPrefixSSE, BX_IA_VZEROALL, BxOpcodeGroupSSE_ERR },
  /* 78 /1 */ { 0, BX_IA_ERROR },
  /* 79 /1 */ { 0, BX_IA_ERROR },
  /* 7A /1 */ { 0, BX_IA_ERROR },
  /* 7B /1 */ { 0, BX_IA_ERROR },
  /* 7C /1 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f7c },
  /* 7D /1 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f7d },
  /* 7E /1 */ { 0, BX_IA_ERROR },
  /* 7F /1 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f7f },
  /* 80 /1 */ { 0, BX_IA_ERROR },
  /* 81 /1 */ { 0, BX_IA_ERROR },
  /* 82 /1 */ { 0, BX_IA_ERROR },
  /* 83 /1 */ { 0, BX_IA_ERROR },
  /* 84 /1 */ { 0, BX_IA_ERROR },
  /* 85 /1 */ { 0, BX_IA_ERROR },
  /* 86 /1 */ { 0, BX_IA_ERROR },
  /* 87 /1 */ { 0, BX_IA_ERROR },
  /* 88 /1 */ { 0, BX_IA_ERROR },
  /* 89 /1 */ { 0, BX_IA_ERROR },
  /* 8A /1 */ { 0, BX_IA_ERROR },
  /* 8B /1 */ { 0, BX_IA_ERROR },
  /* 8C /1 */ { 0, BX_IA_ERROR },
  /* 8D /1 */ { 0, BX_IA_ERROR },
  /* 8E /1 */ { 0, BX_IA_ERROR },
  /* 8F /1 */ { 0, BX_IA_ERROR },
  /* 90 /1 */ { 0, BX_IA_ERROR },
  /* 91 /1 */ { 0, BX_IA_ERROR },
  /* 92 /1 */ { 0, BX_IA_ERROR },
  /* 93 /1 */ { 0, BX_IA_ERROR },
  /* 94 /1 */ { 0, BX_IA_ERROR },
  /* 95 /1 */ { 0, BX_IA_ERROR },
  /* 96 /1 */ { 0, BX_IA_ERROR },
  /* 97 /1 */ { 0, BX_IA_ERROR },
  /* 98 /1 */ { 0, BX_IA_ERROR },
  /* 99 /1 */ { 0, BX_IA_ERROR },
  /* 9A /1 */ { 0, BX_IA_ERROR },
  /* 9B /1 */ { 0, BX_IA_ERROR },
  /* 9C /1 */ { 0, BX_IA_ERROR },
  /* 9D /1 */ { 0, BX_IA_ERROR },
  /* 9E /1 */ { 0, BX_IA_ERROR },
  /* 9F /1 */ { 0, BX_IA_ERROR },
  /* A0 /1 */ { 0, BX_IA_ERROR },
  /* A1 /1 */ { 0, BX_IA_ERROR },
  /* A2 /1 */ { 0, BX_IA_ERROR },
  /* A3 /1 */ { 0, BX_IA_ERROR },
  /* A4 /1 */ { 0, BX_IA_ERROR },
  /* A5 /1 */ { 0, BX_IA_ERROR },
  /* A6 /1 */ { 0, BX_IA_ERROR },
  /* A7 /1 */ { 0, BX_IA_ERROR },
  /* A8 /1 */ { 0, BX_IA_ERROR },
  /* A9 /1 */ { 0, BX_IA_ERROR },
  /* AA /1 */ { 0, BX_IA_ERROR },
  /* AB /1 */ { 0, BX_IA_ERROR },
  /* AC /1 */ { 0, BX_IA_ERROR },
  /* AD /1 */ { 0, BX_IA_ERROR },
  /* AE /1 */ { 0, BX_IA_ERROR },
  /* AF /1 */ { 0, BX_IA_ERROR },
  /* B0 /1 */ { 0, BX_IA_ERROR },
  /* B1 /1 */ { 0, BX_IA_ERROR },
  /* B2 /1 */ { 0, BX_IA_ERROR },
  /* B3 /1 */ { 0, BX_IA_ERROR },
  /* B4 /1 */ { 0, BX_IA_ERROR },
  /* B5 /1 */ { 0, BX_IA_ERROR },
  /* B6 /1 */ { 0, BX_IA_ERROR },
  /* B7 /1 */ { 0, BX_IA_ERROR },
  /* B8 /1 */ { 0, BX_IA_ERROR },
  /* B9 /1 */ { 0, BX_IA_ERROR },
  /* BA /1 */ { 0, BX_IA_ERROR },
  /* BB /1 */ { 0, BX_IA_ERROR },
  /* BC /1 */ { 0, BX_IA_ERROR },
  /* BD /1 */ { 0, BX_IA_ERROR },
  /* BE /1 */ { 0, BX_IA_ERROR },
  /* BF /1 */ { 0, BX_IA_ERROR },
  /* C0 /1 */ { 0, BX_IA_ERROR },
  /* C1 /1 */ { 0, BX_IA_ERROR },
  /* C2 /1 */ { BxPrefixSSE | BxImmediate_Ib, BX_IA_VCMPPS_VpsHpsWpsIb, BxOpcodeGroupAVX_0fc2 },
  /* C3 /1 */ { 0, BX_IA_ERROR },
  /* C4 /1 */ { 0, BX_IA_ERROR },
  /* C5 /1 */ { 0, BX_IA_ERROR },
  /* C6 /1 */ { BxPrefixSSE | BxImmediate_Ib, BX_IA_VSHUFPS_VpsHpsWpsIb, BxOpcodeGroupAVX_0fc6 },
  /* C7 /1 */ { 0, BX_IA_ERROR },
  /* C8 /1 */ { 0, BX_IA_ERROR },
  /* C9 /1 */ { 0, BX_IA_ERROR },
  /* CA /1 */ { 0, BX_IA_ERROR },
  /* CB /1 */ { 0, BX_IA_ERROR },
  /* CC /1 */ { 0, BX_IA_ERROR },
  /* CD /1 */ { 0, BX_IA_ERROR },
  /* CE /1 */ { 0, BX_IA_ERROR },
  /* CF /1 */ { 0, BX_IA_ERROR },
  /* D0 /1 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0fd0 },
  /* D1 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSRLW_VdqHdqWdq },
  /* D2 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSRLD_VdqHdqWdq },
  /* D3 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSRLQ_VdqHdqWdq },
  /* D4 /1 */ { BxPrefixSSE66, BX_IA_V256_VPADDQ_VdqHdqWdq },
  /* D5 /1 */ { BxPrefixSSE66, BX_IA_V256_VPMULLW_VdqHdqWdq },
  /* D6 /1 */ { 0, BX_IA_ERROR },
  /* D7 /1 */ { BxPrefixSSE66, BX_IA_V256_VPMOVMSKB_GdUdq },
  /* D8 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSUBUSB_VdqHdqWdq },
  /* D9 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSUBUSW_VdqHdqWdq },
  /* DA /1 */ { BxPrefixSSE66, BX_IA_V256_VPMINUB_VdqHdqWdq },
  /* DB /1 */ { BxPrefixSSE66, BX_IA_V256_VPAND_VdqHdqWdq },
  /* DC /1 */ { BxPrefixSSE66, BX_IA_V256_VPADDUSB_VdqHdqWdq },
  /* DD /1 */ { BxPrefixSSE66, BX_IA_V256_VPADDUSW_VdqHdqWdq },
  /* DE /1 */ { BxPrefixSSE66, BX_IA_V256_VPMAXUB_VdqHdqWdq },
  /* DF /1 */ { BxPrefixSSE66, BX_IA_V256_VPANDN_VdqHdqWdq },
  /* E0 /1 */ { BxPrefixSSE66, BX_IA_V256_VPAVGB_VdqWdq },
  /* E1 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSRAW_VdqHdqWdq },
  /* E2 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSRAD_VdqHdqWdq },
  /* E3 /1 */ { BxPrefixSSE66, BX_IA_V256_VPAVGW_VdqWdq },
  /* E4 /1 */ { BxPrefixSSE66, BX_IA_V256_VPMULHUW_VdqHdqWdq },
  /* E5 /1 */ { BxPrefixSSE66, BX_IA_V256_VPMULHW_VdqHdqWdq },
  /* E6 /1 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX256_0fe6 },
  /* E7 /1 */ { BxPrefixSSE66, BX_IA_VMOVNTDQ_MdqVdq },
  /* E8 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSUBSB_VdqHdqWdq },
  /* E9 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSUBSW_VdqHdqWdq },
  /* EA /1 */ { BxPrefixSSE66, BX_IA_V256_VPMINSW_VdqHdqWdq },
  /* EB /1 */ { BxPrefixSSE66, BX_IA_V256_VPOR_VdqHdqWdq },
  /* EC /1 */ { BxPrefixSSE66, BX_IA_V256_VPADDSB_VdqHdqWdq },
  /* ED /1 */ { BxPrefixSSE66, BX_IA_V256_VPADDSW_VdqHdqWdq },
  /* EE /1 */ { BxPrefixSSE66, BX_IA_V256_VPMAXSW_VdqHdqWdq },
  /* EF /1 */ { BxPrefixSSE66, BX_IA_V256_VPXOR_VdqHdqWdq },
  /* F0 /1 */ { BxPrefixSSEF2, BX_IA_VLDDQU_VdqMdq },
  /* F1 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSLLW_VdqHdqWdq },
  /* F2 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSLLD_VdqHdqWdq },
  /* F3 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSLLQ_VdqHdqWdq },
  /* F4 /1 */ { BxPrefixSSE66, BX_IA_V256_VPMULUDQ_VdqHdqWdq },
  /* F5 /1 */ { BxPrefixSSE66, BX_IA_V256_VPMADDWD_VdqHdqWdq },
  /* F6 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSADBW_VdqHdqWdq },
  /* F7 /1 */ { 0, BX_IA_ERROR },
  /* F8 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSUBB_VdqHdqWdq },
  /* F9 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSUBW_VdqHdqWdq },
  /* FA /1 */ { BxPrefixSSE66, BX_IA_V256_VPSUBD_VdqHdqWdq },
  /* FB /1 */ { BxPrefixSSE66, BX_IA_V256_VPSUBQ_VdqHdqWdq },
  /* FC /1 */ { BxPrefixSSE66, BX_IA_V256_VPADDB_VdqHdqWdq },
  /* FD /1 */ { BxPrefixSSE66, BX_IA_V256_VPADDW_VdqHdqWdq },
  /* FE /1 */ { BxPrefixSSE66, BX_IA_V256_VPADDD_VdqHdqWdq },
  /* FF /1 */ { 0, BX_IA_ERROR },

  // 256 entries for VEX-encoded 0x0F 0x38 opcodes (VEX.L=1)
  /* 00 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSHUFB_VdqHdqWdq },
  /* 01 /1 */ { BxPrefixSSE66, BX_IA_V256_VPHADDW_VdqHdqWdq },
  /* 02 /1 */ { BxPrefixSSE66, BX_IA_V256_VPHADDD_VdqHdqWdq },
  /* 03 /1 */ { BxPrefixSSE66, BX_IA_V256_VPHADDSW_VdqHdqWdq },
  /* 04 /1 */ { BxPrefixSSE66, BX_IA_V256_VPMADDUBSW_VdqHdqWdq },
  /* 05 /1 */ { BxPrefixSSE66, BX_IA_V256_VPHSUBW_VdqHdqWdq },
  /* 06 /1 */ { BxPrefixSSE66, BX_IA_V256_VPHSUBD_VdqHdqWdq },
  /* 07 /1 */ { BxPrefixSSE66, BX_IA_V256_VPHSUBSW_VdqHdqWdq },
  /* 08 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSIGNB_VdqHdqWdq },
  /* 09 /1 */ { BxPrefixSSE66, BX_IA_V256_VPSIGNW_VdqHdqWdq },
  /* 0A /1 */ { BxPrefixSSE66, BX_IA_V256_VPSIGND_VdqHdqWdq },
  /* 0B /1 */ { BxPrefixSSE66, BX_IA_V256_VPMULHRSW_VdqHdqWdq },
  /* 0C /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VPERMILPS_VpsHpsWps },
  /* 0D /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VPERMILPD_VpdHpdWpd },
  /* 0E /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VTESTPS_VpsWps },
  /* 0F /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VTESTPD_VpdWpd },
  /* 10 /1 */ { 0, BX_IA_ERROR },
  /* 11 /1 */ { 0, BX_IA_ERROR },
  /* 12 /1 */ { 0, BX_IA_ERROR },
  /* 13 /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VCVTPH2PS_VpsWps },
  /* 14 /1 */ { 0, BX_IA_ERROR },
  /* 15 /1 */ { 0, BX_IA_ERROR },
  /* 16 /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_V256_VPERMPS_VpsHpsWps },
  /* 17 /1 */ { BxPrefixSSE66, BX_IA_VPTEST_VdqWdq },
  /* 18 /1 */ { BxSplitMod11B, BX_IA_ERROR, BxOpcodeGroupAVX_0f3818 },
  /* 19 /1 */ { BxSplitMod11B, BX_IA_ERROR, BxOpcodeGroupAVX_0f3819 },
  /* 1A /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_V256_VBROADCASTF128_VdqMdq },
  /* 1B /1 */ { 0, BX_IA_ERROR },
  /* 1C /1 */ { BxPrefixSSE66, BX_IA_V256_VPABSB_VdqWdq },
  /* 1D /1 */ { BxPrefixSSE66, BX_IA_V256_VPABSW_VdqWdq },
  /* 1E /1 */ { BxPrefixSSE66, BX_IA_V256_VPABSD_VdqWdq },
  /* 1F /1 */ { 0, BX_IA_ERROR },
  /* 20 /1 */ { BxPrefixSSE66, BX_IA_VPMOVSXBW256_VdqWdq },
  /* 21 /1 */ { BxPrefixSSE66, BX_IA_VPMOVSXBD256_VdqWq },
  /* 22 /1 */ { BxPrefixSSE66, BX_IA_VPMOVSXBQ256_VdqWd },
  /* 23 /1 */ { BxPrefixSSE66, BX_IA_VPMOVSXWD256_VdqWdq },
  /* 24 /1 */ { BxPrefixSSE66, BX_IA_VPMOVSXWQ256_VdqWq },
  /* 25 /1 */ { BxPrefixSSE66, BX_IA_VPMOVSXDQ256_VdqWdq },
  /* 26 /1 */ { 0, BX_IA_ERROR },
  /* 27 /1 */ { 0, BX_IA_ERROR },
  /* 28 /1 */ { BxPrefixSSE66, BX_IA_V256_VPMULDQ_VdqHdqWdq },
  /* 29 /1 */ { BxPrefixSSE66, BX_IA_V256_VPCMPEQQ_VdqHdqWdq },
  /* 2A /1 */ { BxPrefixSSE66, BX_IA_V256_VMOVNTDQA_VdqMdq },
  /* 2B /1 */ { BxPrefixSSE66, BX_IA_V256_VPACKUSDW_VdqHdqWdq },
  /* 2C /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VMASKMOVPS_VpsHpsMps },
  /* 2D /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VMASKMOVPD_VpdHpdMpd },
  /* 2E /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VMASKMOVPS_MpsHpsVps },
  /* 2F /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VMASKMOVPD_MpdHpdVpd },
  /* 30 /1 */ { BxPrefixSSE66, BX_IA_VPMOVZXBW256_VdqWdq },
  /* 31 /1 */ { BxPrefixSSE66, BX_IA_VPMOVZXBD256_VdqWq },
  /* 32 /1 */ { BxPrefixSSE66, BX_IA_VPMOVZXBQ256_VdqWd },
  /* 33 /1 */ { BxPrefixSSE66, BX_IA_VPMOVZXWD256_VdqWdq },
  /* 34 /1 */ { BxPrefixSSE66, BX_IA_VPMOVZXWQ256_VdqWq },
  /* 35 /1 */ { BxPrefixSSE66, BX_IA_VPMOVZXDQ256_VdqWdq },
  /* 36 /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_V256_VPERMD_VdqHdqWdq },
  /* 37 /1 */ { BxPrefixSSE66, BX_IA_V256_VPCMPGTQ_VdqHdqWdq },
  /* 38 /1 */ { BxPrefixSSE66, BX_IA_V256_VPMINSB_VdqHdqWdq },
  /* 39 /1 */ { BxPrefixSSE66, BX_IA_V256_VPMINSD_VdqHdqWdq },
  /* 3A /1 */ { BxPrefixSSE66, BX_IA_V256_VPMINUW_VdqHdqWdq },
  /* 3B /1 */ { BxPrefixSSE66, BX_IA_V256_VPMINUD_VdqHdqWdq },
  /* 3C /1 */ { BxPrefixSSE66, BX_IA_V256_VPMAXSB_VdqHdqWdq },
  /* 3D /1 */ { BxPrefixSSE66, BX_IA_V256_VPMAXSD_VdqHdqWdq },
  /* 3E /1 */ { BxPrefixSSE66, BX_IA_V256_VPMAXUW_VdqHdqWdq },
  /* 3F /1 */ { BxPrefixSSE66, BX_IA_V256_VPMAXUD_VdqHdqWdq },
  /* 40 /1 */ { BxPrefixSSE66, BX_IA_V256_VPMULLD_VdqHdqWdq },
  /* 41 /1 */ { 0, BX_IA_ERROR },
  /* 42 /1 */ { 0, BX_IA_ERROR },
  /* 43 /1 */ { 0, BX_IA_ERROR },
  /* 44 /1 */ { 0, BX_IA_ERROR },
  /* 45 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3845 },
  /* 46 /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VPSRAVD_VdqHdqWdq },
  /* 47 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3847 },
  /* 48 /1 */ { 0, BX_IA_ERROR },
  /* 49 /1 */ { 0, BX_IA_ERROR },
  /* 4A /1 */ { 0, BX_IA_ERROR },
  /* 4B /1 */ { 0, BX_IA_ERROR },
  /* 4C /1 */ { 0, BX_IA_ERROR },
  /* 4D /1 */ { 0, BX_IA_ERROR },
  /* 4E /1 */ { 0, BX_IA_ERROR },
  /* 4F /1 */ { 0, BX_IA_ERROR },
  /* 50 /1 */ { 0, BX_IA_ERROR },
  /* 51 /1 */ { 0, BX_IA_ERROR },
  /* 52 /1 */ { 0, BX_IA_ERROR },
  /* 53 /1 */ { 0, BX_IA_ERROR },
  /* 54 /1 */ { 0, BX_IA_ERROR },
  /* 55 /1 */ { 0, BX_IA_ERROR },
  /* 56 /1 */ { 0, BX_IA_ERROR },
  /* 57 /1 */ { 0, BX_IA_ERROR },
  /* 58 /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VPBROADCASTD_VdqWd },
  /* 59 /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VPBROADCASTQ_VdqWq },
  /* 5A /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_V256_VBROADCASTI128_VdqMdq },
  /* 5B /1 */ { 0, BX_IA_ERROR },
  /* 5C /1 */ { 0, BX_IA_ERROR },
  /* 5D /1 */ { 0, BX_IA_ERROR },
  /* 5E /1 */ { 0, BX_IA_ERROR },
  /* 5F /1 */ { 0, BX_IA_ERROR },
  /* 60 /1 */ { 0, BX_IA_ERROR },
  /* 61 /1 */ { 0, BX_IA_ERROR },
  /* 62 /1 */ { 0, BX_IA_ERROR },
  /* 63 /1 */ { 0, BX_IA_ERROR },
  /* 64 /1 */ { 0, BX_IA_ERROR },
  /* 65 /1 */ { 0, BX_IA_ERROR },
  /* 66 /1 */ { 0, BX_IA_ERROR },
  /* 67 /1 */ { 0, BX_IA_ERROR },
  /* 68 /1 */ { 0, BX_IA_ERROR },
  /* 69 /1 */ { 0, BX_IA_ERROR },
  /* 6A /1 */ { 0, BX_IA_ERROR },
  /* 6B /1 */ { 0, BX_IA_ERROR },
  /* 6C /1 */ { 0, BX_IA_ERROR },
  /* 6D /1 */ { 0, BX_IA_ERROR },
  /* 6E /1 */ { 0, BX_IA_ERROR },
  /* 6F /1 */ { 0, BX_IA_ERROR },
  /* 70 /1 */ { 0, BX_IA_ERROR },
  /* 71 /1 */ { 0, BX_IA_ERROR },
  /* 72 /1 */ { 0, BX_IA_ERROR },
  /* 73 /1 */ { 0, BX_IA_ERROR },
  /* 74 /1 */ { 0, BX_IA_ERROR },
  /* 75 /1 */ { 0, BX_IA_ERROR },
  /* 76 /1 */ { 0, BX_IA_ERROR },
  /* 77 /1 */ { 0, BX_IA_ERROR },
  /* 78 /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VPBROADCASTB_VdqWb },
  /* 79 /1 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VPBROADCASTW_VdqWw },
  /* 7A /1 */ { 0, BX_IA_ERROR },
  /* 7B /1 */ { 0, BX_IA_ERROR },
  /* 7C /1 */ { 0, BX_IA_ERROR },
  /* 7D /1 */ { 0, BX_IA_ERROR },
  /* 7E /1 */ { 0, BX_IA_ERROR },
  /* 7F /1 */ { 0, BX_IA_ERROR },
  /* 80 /1 */ { 0, BX_IA_ERROR },
  /* 81 /1 */ { 0, BX_IA_ERROR },
  /* 82 /1 */ { 0, BX_IA_ERROR },
  /* 83 /1 */ { 0, BX_IA_ERROR },
  /* 84 /1 */ { 0, BX_IA_ERROR },
  /* 85 /1 */ { 0, BX_IA_ERROR },
  /* 86 /1 */ { 0, BX_IA_ERROR },
  /* 87 /1 */ { 0, BX_IA_ERROR },
  /* 88 /1 */ { 0, BX_IA_ERROR },
  /* 89 /1 */ { 0, BX_IA_ERROR },
  /* 8A /1 */ { 0, BX_IA_ERROR },
  /* 8B /1 */ { 0, BX_IA_ERROR },
  /* 8C /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f388c },
  /* 8D /1 */ { 0, BX_IA_ERROR },
  /* 8E /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f388e },
  /* 8F /1 */ { 0, BX_IA_ERROR },
  /* 90 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3890 },
  /* 91 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3891 },
  /* 92 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3892 },
  /* 93 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3893 },
  /* 94 /1 */ { 0, BX_IA_ERROR },
  /* 95 /1 */ { 0, BX_IA_ERROR },
  /* 96 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3896 },
  /* 97 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3897 },
  /* 98 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3898 },
  /* 99 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3899 },
  /* 9A /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f389a },
  /* 9B /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f389b },
  /* 9C /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f389c },
  /* 9D /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f389d },
  /* 9E /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f389e },
  /* 9F /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f389f },
  /* A0 /1 */ { 0, BX_IA_ERROR },
  /* A1 /1 */ { 0, BX_IA_ERROR },
  /* A2 /1 */ { 0, BX_IA_ERROR },
  /* A3 /1 */ { 0, BX_IA_ERROR },
  /* A4 /1 */ { 0, BX_IA_ERROR },
  /* A5 /1 */ { 0, BX_IA_ERROR },
  /* A6 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38a6 },
  /* A7 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38a7 },
  /* A8 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38a8 },
  /* A9 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38a9 },
  /* AA /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38aa },
  /* AB /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38ab },
  /* AC /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38ac },
  /* AD /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38ad },
  /* AE /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38ae },
  /* AF /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38af },
  /* B0 /1 */ { 0, BX_IA_ERROR },
  /* B1 /1 */ { 0, BX_IA_ERROR },
  /* B2 /1 */ { 0, BX_IA_ERROR },
  /* B3 /1 */ { 0, BX_IA_ERROR },
  /* B4 /1 */ { 0, BX_IA_ERROR },
  /* B5 /1 */ { 0, BX_IA_ERROR },
  /* B6 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38b6 },
  /* B7 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38b7 },
  /* B8 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38b8 },
  /* B9 /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38b9 },
  /* BA /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38ba },
  /* BB /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38bb },
  /* BC /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38bc },
  /* BD /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38bd },
  /* BE /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38be },
  /* BF /1 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f38bf },
  /* C0 /1 */ { 0, BX_IA_ERROR },
  /* C1 /1 */ { 0, BX_IA_ERROR },
  /* C2 /1 */ { 0, BX_IA_ERROR },
  /* C3 /1 */ { 0, BX_IA_ERROR },
  /* C4 /1 */ { 0, BX_IA_ERROR },
  /* C5 /1 */ { 0, BX_IA_ERROR },
  /* C6 /1 */ { 0, BX_IA_ERROR },
  /* C7 /1 */ { 0, BX_IA_ERROR },
  /* C8 /1 */ { 0, BX_IA_ERROR },
  /* C9 /1 */ { 0, BX_IA_ERROR },
  /* CA /1 */ { 0, BX_IA_ERROR },
  /* CB /1 */ { 0, BX_IA_ERROR },
  /* CC /1 */ { 0, BX_IA_ERROR },
  /* CD /1 */ { 0, BX_IA_ERROR },
  /* CE /1 */ { 0, BX_IA_ERROR },
  /* CF /1 */ { 0, BX_IA_ERROR },
  /* D0 /1 */ { 0, BX_IA_ERROR },
  /* D1 /1 */ { 0, BX_IA_ERROR },
  /* D2 /1 */ { 0, BX_IA_ERROR },
  /* D3 /1 */ { 0, BX_IA_ERROR },
  /* D4 /1 */ { 0, BX_IA_ERROR },
  /* D5 /1 */ { 0, BX_IA_ERROR },
  /* D6 /1 */ { 0, BX_IA_ERROR },
  /* D7 /1 */ { 0, BX_IA_ERROR },
  /* D8 /1 */ { 0, BX_IA_ERROR },
  /* D9 /1 */ { 0, BX_IA_ERROR },
  /* DA /1 */ { 0, BX_IA_ERROR },
  /* DB /1 */ { 0, BX_IA_ERROR },
  /* DC /1 */ { 0, BX_IA_ERROR },
  /* DD /1 */ { 0, BX_IA_ERROR },
  /* DE /1 */ { 0, BX_IA_ERROR },
  /* DF /1 */ { 0, BX_IA_ERROR },
  /* E0 /1 */ { 0, BX_IA_ERROR },
  /* E1 /1 */ { 0, BX_IA_ERROR },
  /* E2 /1 */ { 0, BX_IA_ERROR },
  /* E3 /1 */ { 0, BX_IA_ERROR },
  /* E4 /1 */ { 0, BX_IA_ERROR },
  /* E5 /1 */ { 0, BX_IA_ERROR },
  /* E6 /1 */ { 0, BX_IA_ERROR },
  /* E7 /1 */ { 0, BX_IA_ERROR },
  /* E8 /1 */ { 0, BX_IA_ERROR },
  /* E9 /1 */ { 0, BX_IA_ERROR },
  /* EA /1 */ { 0, BX_IA_ERROR },
  /* EB /1 */ { 0, BX_IA_ERROR },
  /* EC /1 */ { 0, BX_IA_ERROR },
  /* ED /1 */ { 0, BX_IA_ERROR },
  /* EE /1 */ { 0, BX_IA_ERROR },
  /* EF /1 */ { 0, BX_IA_ERROR },
  /* F0 /1 */ { 0, BX_IA_ERROR },
  /* F1 /1 */ { 0, BX_IA_ERROR },
  /* F2 /1 */ { 0, BX_IA_ERROR },
  /* F3 /1 */ { 0, BX_IA_ERROR },
  /* F4 /1 */ { 0, BX_IA_ERROR },
  /* F5 /1 */ { 0, BX_IA_ERROR },
  /* F6 /1 */ { 0, BX_IA_ERROR },
  /* F7 /1 */ { 0, BX_IA_ERROR },
  /* F8 /1 */ { 0, BX_IA_ERROR },
  /* F9 /1 */ { 0, BX_IA_ERROR },
  /* FA /1 */ { 0, BX_IA_ERROR },
  /* FB /1 */ { 0, BX_IA_ERROR },
  /* FC /1 */ { 0, BX_IA_ERROR },
  /* FD /1 */ { 0, BX_IA_ERROR },
  /* FE /1 */ { 0, BX_IA_ERROR },
  /* FF /1 */ { 0, BX_IA_ERROR },

  // 256 entries for VEX-encoded 0x0F 0x3A opcodes (VEX.L=1)
  /* 00 /1 */ { BxPrefixSSE66 | BxVexW1 | BxImmediate_Ib, BX_IA_V256_VPERMQ_VdqWdqIb },
  /* 01 /1 */ { BxPrefixSSE66 | BxVexW1 | BxImmediate_Ib, BX_IA_V256_VPERMPD_VpdWpdIb },
  /* 02 /1 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_VPBLENDD_VdqHdqWdqIb },
  /* 03 /1 */ { 0, BX_IA_ERROR },
  /* 04 /1 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_VPERMILPS_VpsWpsIb },
  /* 05 /1 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_VPERMILPD_VpdWpdIb },
  /* 06 /1 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_V256_VPERM2F128_VdqHdqWdqIb },
  /* 07 /1 */ { 0, BX_IA_ERROR },
  /* 08 /1 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VROUNDPS_VpsWpsIb },
  /* 09 /1 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VROUNDPD_VpdWpdIb },
  /* 0A /1 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VROUNDSS_VssHpsWssIb },
  /* 0B /1 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VROUNDSD_VsdHpdWsdIb },
  /* 0C /1 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VBLENDPS_VpsHpsWpsIb },
  /* 0D /1 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VBLENDPD_VpdHpdWpdIb },
  /* 0E /1 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V256_VPBLENDW_VdqHdqWdqIb },
  /* 0F /1 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V256_VPALIGNR_VdqHdqWdqIb },
  /* 10 /1 */ { 0, BX_IA_ERROR },
  /* 11 /1 */ { 0, BX_IA_ERROR },
  /* 12 /1 */ { 0, BX_IA_ERROR },
  /* 13 /1 */ { 0, BX_IA_ERROR },
  /* 14 /1 */ { 0, BX_IA_ERROR },
  /* 15 /1 */ { 0, BX_IA_ERROR },
  /* 16 /1 */ { 0, BX_IA_ERROR },
  /* 17 /1 */ { 0, BX_IA_ERROR },
  /* 18 /1 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_V256_VINSERTF128_VdqHdqWdqIb },
  /* 19 /1 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_V256_VEXTRACTF128_WdqVdqIb },
  /* 1A /1 */ { 0, BX_IA_ERROR },
  /* 1B /1 */ { 0, BX_IA_ERROR },
  /* 1C /1 */ { 0, BX_IA_ERROR },
  /* 1D /1 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_VCVTPS2PH_WpsVpsIb },
  /* 1E /1 */ { 0, BX_IA_ERROR },
  /* 1F /1 */ { 0, BX_IA_ERROR },
  /* 20 /1 */ { 0, BX_IA_ERROR },
  /* 21 /1 */ { 0, BX_IA_ERROR },
  /* 22 /1 */ { 0, BX_IA_ERROR },
  /* 23 /1 */ { 0, BX_IA_ERROR },
  /* 24 /1 */ { 0, BX_IA_ERROR },
  /* 25 /1 */ { 0, BX_IA_ERROR },
  /* 26 /1 */ { 0, BX_IA_ERROR },
  /* 27 /1 */ { 0, BX_IA_ERROR },
  /* 28 /1 */ { 0, BX_IA_ERROR },
  /* 29 /1 */ { 0, BX_IA_ERROR },
  /* 2A /1 */ { 0, BX_IA_ERROR },
  /* 2B /1 */ { 0, BX_IA_ERROR },
  /* 2C /1 */ { 0, BX_IA_ERROR },
  /* 2D /1 */ { 0, BX_IA_ERROR },
  /* 2E /1 */ { 0, BX_IA_ERROR },
  /* 2F /1 */ { 0, BX_IA_ERROR },
  /* 30 /1 */ { 0, BX_IA_ERROR },
  /* 31 /1 */ { 0, BX_IA_ERROR },
  /* 32 /1 */ { 0, BX_IA_ERROR },
  /* 33 /1 */ { 0, BX_IA_ERROR },
  /* 34 /1 */ { 0, BX_IA_ERROR },
  /* 35 /1 */ { 0, BX_IA_ERROR },
  /* 36 /1 */ { 0, BX_IA_ERROR },
  /* 37 /1 */ { 0, BX_IA_ERROR },
  /* 38 /1 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_V256_VINSERTI128_VdqHdqWdqIb },
  /* 39 /1 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_V256_VEXTRACTI128_WdqVdqIb },
  /* 3A /1 */ { 0, BX_IA_ERROR },
  /* 3B /1 */ { 0, BX_IA_ERROR },
  /* 3C /1 */ { 0, BX_IA_ERROR },
  /* 3D /1 */ { 0, BX_IA_ERROR },
  /* 3E /1 */ { 0, BX_IA_ERROR },
  /* 3F /1 */ { 0, BX_IA_ERROR },
  /* 40 /1 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VDPPS_VpsHpsWpsIb },
  /* 41 /1 */ { 0, BX_IA_ERROR },
  /* 42 /1 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V256_VMPSADBW_VdqWdqIb },
  /* 43 /1 */ { 0, BX_IA_ERROR },
  /* 44 /1 */ { 0, BX_IA_ERROR },
  /* 45 /1 */ { 0, BX_IA_ERROR },
  /* 46 /1 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_V256_VPERM2I128_VdqHdqWdqIb },
  /* 47 /1 */ { 0, BX_IA_ERROR },
  /* 48 /1 */ { BxPrefixSSE66 | BxImmediate_Ib5, BX_IA_VPERMIL2PS_VdqHdqWdqIb },
  /* 49 /1 */ { BxPrefixSSE66 | BxImmediate_Ib5, BX_IA_VPERMIL2PD_VdqHdqWdqIb },
  /* 4A /1 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib4, BX_IA_VBLENDVPS_VpsHpsWpsIb },
  /* 4B /1 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib4, BX_IA_VBLENDVPD_VpdHpdWpdIb },
  /* 4C /1 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib4, BX_IA_V256_VPBLENDVB_VdqHdqWdqIb },
  /* 4D /1 */ { 0, BX_IA_ERROR },
  /* 4E /1 */ { 0, BX_IA_ERROR },
  /* 4F /1 */ { 0, BX_IA_ERROR },
  /* 50 /1 */ { 0, BX_IA_ERROR },
  /* 51 /1 */ { 0, BX_IA_ERROR },
  /* 52 /1 */ { 0, BX_IA_ERROR },
  /* 53 /1 */ { 0, BX_IA_ERROR },
  /* 54 /1 */ { 0, BX_IA_ERROR },
  /* 55 /1 */ { 0, BX_IA_ERROR },
  /* 56 /1 */ { 0, BX_IA_ERROR },
  /* 57 /1 */ { 0, BX_IA_ERROR },
  /* 58 /1 */ { 0, BX_IA_ERROR },
  /* 59 /1 */ { 0, BX_IA_ERROR },
  /* 5A /1 */ { 0, BX_IA_ERROR },
  /* 5B /1 */ { 0, BX_IA_ERROR },
  /* 5C /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMADDSUBPS_VpsHpsWpsVIb },
  /* 5D /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMADDSUBPD_VpdHpdWpdVIb },
  /* 5E /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMSUBADDPS_VpsHpsWpsVIb },
  /* 5F /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMSUBADDPD_VpdHpdWpdVIb },
  /* 60 /1 */ { 0, BX_IA_ERROR },
  /* 61 /1 */ { 0, BX_IA_ERROR },
  /* 62 /1 */ { 0, BX_IA_ERROR },
  /* 63 /1 */ { 0, BX_IA_ERROR },
  /* 64 /1 */ { 0, BX_IA_ERROR },
  /* 65 /1 */ { 0, BX_IA_ERROR },
  /* 66 /1 */ { 0, BX_IA_ERROR },
  /* 67 /1 */ { 0, BX_IA_ERROR },
  /* 68 /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMADDPS_VpsHpsWpsVIb },
  /* 69 /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMADDPD_VpdHpdWpdVIb },
  /* 6A /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMADDSS_VssHssWssVIb },
  /* 6B /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMADDSD_VsdHsdWsdVIb },
  /* 6C /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMSUBPS_VpsHpsWpsVIb },
  /* 6D /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMSUBPD_VpdHpdWpdVIb },
  /* 6E /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMSUBSS_VssHssWssVIb },
  /* 6F /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFMSUBSD_VsdHsdWsdVIb },
  /* 70 /1 */ { 0, BX_IA_ERROR },
  /* 71 /1 */ { 0, BX_IA_ERROR },
  /* 72 /1 */ { 0, BX_IA_ERROR },
  /* 73 /1 */ { 0, BX_IA_ERROR },
  /* 74 /1 */ { 0, BX_IA_ERROR },
  /* 75 /1 */ { 0, BX_IA_ERROR },
  /* 76 /1 */ { 0, BX_IA_ERROR },
  /* 77 /1 */ { 0, BX_IA_ERROR },
  /* 78 /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFNMADDPS_VpsHpsWpsVIb },
  /* 79 /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFNMADDPD_VpdHpdWpdVIb },
  /* 7A /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFNMADDSS_VssHssWssVIb },
  /* 7B /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFNMADDSD_VsdHsdWsdVIb },
  /* 7C /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFNMSUBPS_VpsHpsWpsVIb },
  /* 7D /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFNMSUBPD_VpdHpdWpdVIb },
  /* 7E /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFNMSUBSS_VssHssWssVIb },
  /* 7F /1 */ { BxPrefixSSE66 | BxImmediate_Ib4, BX_IA_VFNMSUBSD_VsdHsdWsdVIb },
  /* 80 /1 */ { 0, BX_IA_ERROR },
  /* 81 /1 */ { 0, BX_IA_ERROR },
  /* 82 /1 */ { 0, BX_IA_ERROR },
  /* 83 /1 */ { 0, BX_IA_ERROR },
  /* 84 /1 */ { 0, BX_IA_ERROR },
  /* 85 /1 */ { 0, BX_IA_ERROR },
  /* 86 /1 */ { 0, BX_IA_ERROR },
  /* 87 /1 */ { 0, BX_IA_ERROR },
  /* 88 /1 */ { 0, BX_IA_ERROR },
  /* 89 /1 */ { 0, BX_IA_ERROR },
  /* 8A /1 */ { 0, BX_IA_ERROR },
  /* 8B /1 */ { 0, BX_IA_ERROR },
  /* 8C /1 */ { 0, BX_IA_ERROR },
  /* 8D /1 */ { 0, BX_IA_ERROR },
  /* 8E /1 */ { 0, BX_IA_ERROR },
  /* 8F /1 */ { 0, BX_IA_ERROR },
  /* 90 /1 */ { 0, BX_IA_ERROR },
  /* 91 /1 */ { 0, BX_IA_ERROR },
  /* 92 /1 */ { 0, BX_IA_ERROR },
  /* 93 /1 */ { 0, BX_IA_ERROR },
  /* 94 /1 */ { 0, BX_IA_ERROR },
  /* 95 /1 */ { 0, BX_IA_ERROR },
  /* 96 /1 */ { 0, BX_IA_ERROR },
  /* 97 /1 */ { 0, BX_IA_ERROR },
  /* 98 /1 */ { 0, BX_IA_ERROR },
  /* 99 /1 */ { 0, BX_IA_ERROR },
  /* 9A /1 */ { 0, BX_IA_ERROR },
  /* 9B /1 */ { 0, BX_IA_ERROR },
  /* 9C /1 */ { 0, BX_IA_ERROR },
  /* 9D /1 */ { 0, BX_IA_ERROR },
  /* 9E /1 */ { 0, BX_IA_ERROR },
  /* 9F /1 */ { 0, BX_IA_ERROR },
  /* A0 /1 */ { 0, BX_IA_ERROR },
  /* A1 /1 */ { 0, BX_IA_ERROR },
  /* A2 /1 */ { 0, BX_IA_ERROR },
  /* A3 /1 */ { 0, BX_IA_ERROR },
  /* A4 /1 */ { 0, BX_IA_ERROR },
  /* A5 /1 */ { 0, BX_IA_ERROR },
  /* A6 /1 */ { 0, BX_IA_ERROR },
  /* A7 /1 */ { 0, BX_IA_ERROR },
  /* A8 /1 */ { 0, BX_IA_ERROR },
  /* A9 /1 */ { 0, BX_IA_ERROR },
  /* AA /1 */ { 0, BX_IA_ERROR },
  /* AB /1 */ { 0, BX_IA_ERROR },
  /* AC /1 */ { 0, BX_IA_ERROR },
  /* AD /1 */ { 0, BX_IA_ERROR },
  /* AE /1 */ { 0, BX_IA_ERROR },
  /* AF /1 */ { 0, BX_IA_ERROR },
  /* B0 /1 */ { 0, BX_IA_ERROR },
  /* B1 /1 */ { 0, BX_IA_ERROR },
  /* B2 /1 */ { 0, BX_IA_ERROR },
  /* B3 /1 */ { 0, BX_IA_ERROR },
  /* B4 /1 */ { 0, BX_IA_ERROR },
  /* B5 /1 */ { 0, BX_IA_ERROR },
  /* B6 /1 */ { 0, BX_IA_ERROR },
  /* B7 /1 */ { 0, BX_IA_ERROR },
  /* B8 /1 */ { 0, BX_IA_ERROR },
  /* B9 /1 */ { 0, BX_IA_ERROR },
  /* BA /1 */ { 0, BX_IA_ERROR },
  /* BB /1 */ { 0, BX_IA_ERROR },
  /* BC /1 */ { 0, BX_IA_ERROR },
  /* BD /1 */ { 0, BX_IA_ERROR },
  /* BE /1 */ { 0, BX_IA_ERROR },
  /* BF /1 */ { 0, BX_IA_ERROR },
  /* C0 /1 */ { 0, BX_IA_ERROR },
  /* C1 /1 */ { 0, BX_IA_ERROR },
  /* C2 /1 */ { 0, BX_IA_ERROR },
  /* C3 /1 */ { 0, BX_IA_ERROR },
  /* C4 /1 */ { 0, BX_IA_ERROR },
  /* C5 /1 */ { 0, BX_IA_ERROR },
  /* C6 /1 */ { 0, BX_IA_ERROR },
  /* C7 /1 */ { 0, BX_IA_ERROR },
  /* C8 /1 */ { 0, BX_IA_ERROR },
  /* C9 /1 */ { 0, BX_IA_ERROR },
  /* CA /1 */ { 0, BX_IA_ERROR },
  /* CB /1 */ { 0, BX_IA_ERROR },
  /* CC /1 */ { 0, BX_IA_ERROR },
  /* CD /1 */ { 0, BX_IA_ERROR },
  /* CE /1 */ { 0, BX_IA_ERROR },
  /* CF /1 */ { 0, BX_IA_ERROR },
  /* D0 /1 */ { 0, BX_IA_ERROR },
  /* D1 /1 */ { 0, BX_IA_ERROR },
  /* D2 /1 */ { 0, BX_IA_ERROR },
  /* D3 /1 */ { 0, BX_IA_ERROR },
  /* D4 /1 */ { 0, BX_IA_ERROR },
  /* D5 /1 */ { 0, BX_IA_ERROR },
  /* D6 /1 */ { 0, BX_IA_ERROR },
  /* D7 /1 */ { 0, BX_IA_ERROR },
  /* D8 /1 */ { 0, BX_IA_ERROR },
  /* D9 /1 */ { 0, BX_IA_ERROR },
  /* DA /1 */ { 0, BX_IA_ERROR },
  /* DB /1 */ { 0, BX_IA_ERROR },
  /* DC /1 */ { 0, BX_IA_ERROR },
  /* DD /1 */ { 0, BX_IA_ERROR },
  /* DE /1 */ { 0, BX_IA_ERROR },
  /* DF /1 */ { 0, BX_IA_ERROR },
  /* E0 /1 */ { 0, BX_IA_ERROR },
  /* E1 /1 */ { 0, BX_IA_ERROR },
  /* E2 /1 */ { 0, BX_IA_ERROR },
  /* E3 /1 */ { 0, BX_IA_ERROR },
  /* E4 /1 */ { 0, BX_IA_ERROR },
  /* E5 /1 */ { 0, BX_IA_ERROR },
  /* E6 /1 */ { 0, BX_IA_ERROR },
  /* E7 /1 */ { 0, BX_IA_ERROR },
  /* E8 /1 */ { 0, BX_IA_ERROR },
  /* E9 /1 */ { 0, BX_IA_ERROR },
  /* EA /1 */ { 0, BX_IA_ERROR },
  /* EB /1 */ { 0, BX_IA_ERROR },
  /* EC /1 */ { 0, BX_IA_ERROR },
  /* ED /1 */ { 0, BX_IA_ERROR },
  /* EE /1 */ { 0, BX_IA_ERROR },
  /* EF /1 */ { 0, BX_IA_ERROR },
  /* F0 /1 */ { 0, BX_IA_ERROR },
  /* F1 /1 */ { 0, BX_IA_ERROR },
  /* F2 /1 */ { 0, BX_IA_ERROR },
  /* F3 /1 */ { 0, BX_IA_ERROR },
  /* F4 /1 */ { 0, BX_IA_ERROR },
  /* F5 /1 */ { 0, BX_IA_ERROR },
  /* F6 /1 */ { 0, BX_IA_ERROR },
  /* F7 /1 */ { 0, BX_IA_ERROR },
  /* F8 /1 */ { 0, BX_IA_ERROR },
  /* F9 /1 */ { 0, BX_IA_ERROR },
  /* FA /1 */ { 0, BX_IA_ERROR },
  /* FB /1 */ { 0, BX_IA_ERROR },
  /* FC /1 */ { 0, BX_IA_ERROR },
  /* FD /1 */ { 0, BX_IA_ERROR },
  /* FE /1 */ { 0, BX_IA_ERROR },
  /* FF /1 */ { 0, BX_IA_ERROR }
};

#endif // BX_SUPPORT_AVX && BX_CPU_LEVEL >= 6

#endif // BX_AVX_FETCHDECODE_TABLES_H
