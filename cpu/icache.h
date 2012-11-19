/////////////////////////////////////////////////////////////////////////
// $Id: icache.h 11356 2012-08-21 19:58:41Z sshwarts $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2007-2011 Stanislav Shwartsman
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

#ifndef BX_ICACHE_H
#define BX_ICACHE_H

extern void handleSMC(bx_phy_address pAddr, Bit32u mask);

class bxPageWriteStampTable
{
#define PHY_MEM_PAGES (1024*1024)
  Bit32u *fineGranularityMapping;

public:
  bxPageWriteStampTable() {
    fineGranularityMapping = new Bit32u[PHY_MEM_PAGES];
    resetWriteStamps();
  }
 ~bxPageWriteStampTable() { delete [] fineGranularityMapping; }

  BX_CPP_INLINE Bit32u hash(bx_phy_address pAddr) const {
    // can share writeStamps between multiple pages if >32 bit phy address
    return ((Bit32u) pAddr) >> 12;
  }

  BX_CPP_INLINE Bit32u getFineGranularityMapping(bx_phy_address pAddr) const
  {
    return fineGranularityMapping[hash(pAddr)];
  }

  BX_CPP_INLINE void markICache(bx_phy_address pAddr, unsigned len)
  {
    Bit32u mask  = 1 << (PAGE_OFFSET((Bit32u) pAddr) >> 7);
           mask |= 1 << (PAGE_OFFSET((Bit32u) pAddr + len - 1) >> 7);

    fineGranularityMapping[hash(pAddr)] |= mask;
  }

  BX_CPP_INLINE void markICacheMask(bx_phy_address pAddr, Bit32u mask)
  {
    fineGranularityMapping[hash(pAddr)] |= mask;
  }

  // whole page is being altered
  BX_CPP_INLINE void decWriteStamp(bx_phy_address pAddr)
  {
    Bit32u index = hash(pAddr);

    if (fineGranularityMapping[index]) {
      handleSMC(pAddr, 0xffffffff); // one of the CPUs might be running trace from this page
      fineGranularityMapping[index] = 0;
    }
  }

  // assumption: write does not split 4K page
  BX_CPP_INLINE void decWriteStamp(bx_phy_address pAddr, unsigned len)
  {
    Bit32u index = hash(pAddr);

    if (fineGranularityMapping[index]) {
       Bit32u mask  = 1 << (PAGE_OFFSET((Bit32u) pAddr) >> 7);
              mask |= 1 << (PAGE_OFFSET((Bit32u) pAddr + len - 1) >> 7);

       if (fineGranularityMapping[index] & mask) {
          // one of the CPUs might be running trace from this page
          handleSMC(pAddr, mask);
          fineGranularityMapping[index] &= ~mask;
       }       
    }
  }

  BX_CPP_INLINE void resetWriteStamps(void);
};

BX_CPP_INLINE void bxPageWriteStampTable::resetWriteStamps(void)
{
  for (Bit32u i=0; i<PHY_MEM_PAGES; i++) {
    fineGranularityMapping[i] = 0;
  }
}

extern bxPageWriteStampTable pageWriteStampTable;

#define BxICacheEntries (64 * 1024)  // Must be a power of 2.
#define BxICacheMemPool (384 * 1024)

#define BX_MAX_TRACE_LENGTH 32

struct bxICacheEntry_c
{
  bx_phy_address pAddr; // Physical address of the instruction
  Bit32u traceMask;

  Bit32u tlen;          // Trace length in instructions
  bxInstruction_c *i;
};

#define BX_ICACHE_INVALID_PHY_ADDRESS (bx_phy_address(-1))

class BOCHSAPI bxICache_c {
public:
  bxICacheEntry_c entry[BxICacheEntries];
  bxInstruction_c mpool[BxICacheMemPool];
  unsigned mpindex;

#define BX_ICACHE_PAGE_SPLIT_ENTRIES 8 /* must be power of two */
  struct pageSplitEntryIndex {
    bx_phy_address ppf; // Physical address of 2nd page of the trace 
    bxICacheEntry_c *e; // Pointer to icache entry
  } pageSplitIndex[BX_ICACHE_PAGE_SPLIT_ENTRIES];
  int nextPageSplitIndex;

#define BX_ICACHE_VICTIM_ENTRIES 8 /* must be power of two */
  struct bxVictimCacheEntry {
    Bit32u fetchModeMask;
    bxICacheEntry_c vc_entry;
  } victimCache[BX_ICACHE_VICTIM_ENTRIES];
  int nextVictimCacheIndex;

public:
  bxICache_c() { flushICacheEntries(); }

  BX_CPP_INLINE unsigned hash(bx_phy_address pAddr, unsigned fetchModeMask) const
  {
//  return ((pAddr + (pAddr << 2) + (pAddr>>6)) & (BxICacheEntries-1)) ^ fetchModeMask;
    return ((pAddr) & (BxICacheEntries-1)) ^ fetchModeMask;
  }

  BX_CPP_INLINE void alloc_trace(bxICacheEntry_c *e)
  {
    // took +1 garbend for instruction chaining speedup (end-of-trace opcode)
    if ((mpindex + BX_MAX_TRACE_LENGTH + 1) > BxICacheMemPool) {
      flushICacheEntries();
    }
    e->i = &mpool[mpindex];
    e->tlen = 0;
  }

  BX_CPP_INLINE void commit_trace(unsigned len) { mpindex += len; }

  BX_CPP_INLINE void commit_page_split_trace(bx_phy_address paddr, bxICacheEntry_c *entry)
  {
    mpindex += entry->tlen;

    // register page split entry
    if (pageSplitIndex[nextPageSplitIndex].ppf != BX_ICACHE_INVALID_PHY_ADDRESS)
      pageSplitIndex[nextPageSplitIndex].e->pAddr = BX_ICACHE_INVALID_PHY_ADDRESS;

    pageSplitIndex[nextPageSplitIndex].ppf = paddr;
    pageSplitIndex[nextPageSplitIndex].e = entry;

    nextPageSplitIndex = (nextPageSplitIndex+1) & (BX_ICACHE_PAGE_SPLIT_ENTRIES-1);
  }

  BX_CPP_INLINE bxICacheEntry_c *lookup_victim_cache(bx_phy_address pAddr, Bit32u fetchModeMask)
  {
    for (int i=0; i < BX_ICACHE_VICTIM_ENTRIES;i++) {
      bxVictimCacheEntry *e = &victimCache[i];
      if (e->vc_entry.pAddr == pAddr && e->fetchModeMask == fetchModeMask) {
        return &e->vc_entry;
      }
    }

    return NULL;
  }

  BX_CPP_INLINE void victim_entry(bxICacheEntry_c *entry, Bit32u fetchModeMask)
  {
    if (entry->pAddr != BX_ICACHE_INVALID_PHY_ADDRESS) {
      victimCache[nextVictimCacheIndex].fetchModeMask = fetchModeMask;
      victimCache[nextVictimCacheIndex].vc_entry = *entry;
      nextVictimCacheIndex = (nextVictimCacheIndex+1) & (BX_ICACHE_VICTIM_ENTRIES-1);
    }
  }

  BX_CPP_INLINE void handleSMC(bx_phy_address pAddr, Bit32u mask);

  BX_CPP_INLINE void flushICacheEntries(void);

  BX_CPP_INLINE bxICacheEntry_c* get_entry(bx_phy_address pAddr, unsigned fetchModeMask)
  {
    return &(entry[hash(pAddr, fetchModeMask)]);
  }
};

BX_CPP_INLINE void bxICache_c::flushICacheEntries(void)
{
  bxICacheEntry_c* e = entry;
  unsigned i;

  for (i=0; i<BxICacheEntries; i++, e++) {
    e->pAddr = BX_ICACHE_INVALID_PHY_ADDRESS;
    e->traceMask = 0;
  }

  nextPageSplitIndex = 0;
  for (i=0;i<BX_ICACHE_PAGE_SPLIT_ENTRIES;i++)
    pageSplitIndex[i].ppf = BX_ICACHE_INVALID_PHY_ADDRESS;

  nextVictimCacheIndex = 0;
  for (i=0;i<BX_ICACHE_VICTIM_ENTRIES;i++)
    victimCache[i].vc_entry.pAddr = BX_ICACHE_INVALID_PHY_ADDRESS;

  mpindex = 0;
}

#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS
extern void genDummyICacheEntry(bxInstruction_c *i);
#endif

BX_CPP_INLINE void bxICache_c::handleSMC(bx_phy_address pAddr, Bit32u mask)
{
  pAddr = LPFOf(pAddr);

  unsigned i;

  if (mask & 0x1) {
    // the store touched 1st cache line in the page, check for
    // page split traces to invalidate.
    for (i=0;i<BX_ICACHE_PAGE_SPLIT_ENTRIES;i++) {
      if (pAddr == pageSplitIndex[i].ppf) {
        pageSplitIndex[i].ppf = BX_ICACHE_INVALID_PHY_ADDRESS;
      }
    }
  }

  for (i=0;i < BX_ICACHE_VICTIM_ENTRIES; i++) {
    bxVictimCacheEntry *e = &victimCache[i];
    e->vc_entry.pAddr = BX_ICACHE_INVALID_PHY_ADDRESS;
  }

  bxICacheEntry_c *e = get_entry(pAddr, 0);

  // go over 32 "cache lines" of 128 byte each
  for (unsigned n=0; n < 32; n++) {
    Bit32u line_mask = (1 << n);
    if (line_mask > mask) break;
    for (unsigned index=0; index < 128; index++, e++) {
      if (pAddr == LPFOf(e->pAddr) && (e->traceMask & mask) != 0) {
        e->pAddr = BX_ICACHE_INVALID_PHY_ADDRESS;
#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS
        for (unsigned instr=0;instr < e->tlen; instr++)
          genDummyICacheEntry(e->i + instr);
#endif
      }
    }
  }
}

extern void flushICaches(void);

#endif
