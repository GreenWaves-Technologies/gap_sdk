#line 1 "/home/haugoug/src/gap_sdk/riscv-gnu-toolchain/riscv-binutils-gdb/bfd/elfnn-riscv.c"
/* RISC-V-specific support for 64-bit ELF.
   Copyright (C) 2011-2017 Free Software Foundation, Inc.

   Contributed by Andrew Waterman (andrew@sifive.com).
   Based on TILE-Gx and MIPS targets.

   PULP family support contributed by Eric Flamand (eflamand@iis.ee.ethz.ch) at ETH-Zurich
   and Greenwaves Technologies (eric.flamand@greenwaves-technologies.com)

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3. If not,
   see <http://www.gnu.org/licenses/>.  */

/* This file handles RISC-V ELF targets.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "bfdlink.h"
#include "genlink.h"
#include "elf-bfd.h"
#include "elfxx-riscv.h"
#include "elf/riscv.h"
#include "opcode/riscv.h"

#define ARCH_SIZE 64

#define MINUS_ONE ((bfd_vma)0 - 1)

#define RISCV_ELF_LOG_WORD_BYTES (ARCH_SIZE == 32 ? 2 : 3)

#define RISCV_ELF_WORD_BYTES (1 << RISCV_ELF_LOG_WORD_BYTES)

/* The name of the dynamic interpreter.  This is put in the .interp
   section.  */

#define ELF64_DYNAMIC_INTERPRETER "/lib/ld.so.1"
#define ELF32_DYNAMIC_INTERPRETER "/lib32/ld.so.1"

#define ELF_ARCH			bfd_arch_riscv
#define ELF_TARGET_ID			RISCV_ELF_DATA
#define ELF_MACHINE_CODE		EM_RISCV
#define ELF_MAXPAGESIZE			0x1000
#define ELF_COMMONPAGESIZE		0x1000

/* Linker argument -mComp
   Set linker to component mode, in this case when export section is generated we use an offset relative to the section head
   for exported symbol. If not, resident mode, we use the absolute address */

#if 64==32
bfd_boolean ComponentMode = FALSE;

/*  Linker argumenr -mDIE, to dump import and export sections */
unsigned int DumpImportExportSections = 0;
#endif

/* The RISC-V linker needs to keep track of the number of relocs that it
   decides to copy as dynamic relocs in check_relocs for each symbol.
   This is so that it can later discard them if they are found to be
   unnecessary.  We store the information in a field extending the
   regular ELF linker hash table.  */

struct riscv_elf_dyn_relocs
{
  struct riscv_elf_dyn_relocs *next;

  /* The input section of the reloc.  */
  asection *sec;

  /* Total number of relocs copied for the input section.  */
  bfd_size_type count;

  /* Number of pc-relative relocs copied for the input section.  */
  bfd_size_type pc_count;
};

/* RISC-V ELF linker hash entry.  */

struct riscv_elf_link_hash_entry
{
  struct elf_link_hash_entry elf;

  /* Track dynamic relocs copied for this symbol.  */
  struct riscv_elf_dyn_relocs *dyn_relocs;

#define GOT_UNKNOWN     0
#define GOT_NORMAL      1
#define GOT_TLS_GD      2
#define GOT_TLS_IE      4
#define GOT_TLS_LE      8
  char tls_type;
};

#define riscv_elf_hash_entry(ent) \
  ((struct riscv_elf_link_hash_entry *)(ent))

struct _bfd_riscv_elf_obj_tdata
{
  struct elf_obj_tdata root;

  /* tls_type for each local got entry.  */
  char *local_got_tls_type;
};

#define _bfd_riscv_elf_tdata(abfd) \
  ((struct _bfd_riscv_elf_obj_tdata *) (abfd)->tdata.any)

#define _bfd_riscv_elf_local_got_tls_type(abfd) \
  (_bfd_riscv_elf_tdata (abfd)->local_got_tls_type)

#define _bfd_riscv_elf_tls_type(abfd, h, symndx)		\
  (*((h) != NULL ? &riscv_elf_hash_entry (h)->tls_type		\
     : &_bfd_riscv_elf_local_got_tls_type (abfd) [symndx]))

#define is_riscv_elf(bfd)				\
  (bfd_get_flavour (bfd) == bfd_target_elf_flavour	\
   && elf_tdata (bfd) != NULL				\
   && elf_object_id (bfd) == RISCV_ELF_DATA)

#include "elf/common.h"
#include "elf/internal.h"

struct riscv_elf_link_hash_table
{
  struct elf_link_hash_table elf;

  /* Short-cuts to get to dynamic linker sections.  */
  asection *sdyntdata;

  /* Small local sym to section mapping cache.  */
  struct sym_cache sym_cache;
};


/* Get the RISC-V ELF linker hash table from a link_info structure.  */
#define riscv_elf_hash_table(p) \
  (elf_hash_table_id ((struct elf_link_hash_table *) ((p)->hash)) \
  == RISCV_ELF_DATA ? ((struct riscv_elf_link_hash_table *) ((p)->hash)) : NULL)

static void
riscv_info_to_howto_rela (bfd *abfd ATTRIBUTE_UNUSED,
			  arelent *cache_ptr,
			  Elf_Internal_Rela *dst)
{
  cache_ptr->howto = riscv_elf_rtype_to_howto (ELF64_R_TYPE (dst->r_info));
}

bfd_boolean _bfd_riscv_elf_final_link (bfd *, struct bfd_link_info *);

static void
riscv_elf_append_rela (bfd *abfd, asection *s, Elf_Internal_Rela *rel)
{
  const struct elf_backend_data *bed;
  bfd_byte *loc;

  bed = get_elf_backend_data (abfd);
  loc = s->contents + (s->reloc_count++ * bed->s->sizeof_rela);
  bed->s->swap_reloca_out (abfd, rel, loc);
}

/* PLT/GOT stuff.  */

#define PLT_HEADER_INSNS 8
#define PLT_ENTRY_INSNS 4
#define PLT_HEADER_SIZE (PLT_HEADER_INSNS * 4)
#define PLT_ENTRY_SIZE (PLT_ENTRY_INSNS * 4)

#define GOT_ENTRY_SIZE RISCV_ELF_WORD_BYTES

#define GOTPLT_HEADER_SIZE (2 * GOT_ENTRY_SIZE)

#define sec_addr(sec) ((sec)->output_section->vma + (sec)->output_offset)

static bfd_vma
riscv_elf_got_plt_val (bfd_vma plt_index, struct bfd_link_info *info)
{
  return sec_addr (riscv_elf_hash_table (info)->elf.sgotplt)
	 + GOTPLT_HEADER_SIZE + (plt_index * GOT_ENTRY_SIZE);
}

#if ARCH_SIZE == 32
# define MATCH_LREG MATCH_LW
#else
# define MATCH_LREG MATCH_LD
#endif

/* Generate a PLT header.  */

static void
riscv_make_plt_header (bfd_vma gotplt_addr, bfd_vma addr, uint32_t *entry)
{
  bfd_vma gotplt_offset_high = RISCV_PCREL_HIGH_PART (gotplt_addr, addr);
  bfd_vma gotplt_offset_low = RISCV_PCREL_LOW_PART (gotplt_addr, addr);

  /* auipc  t2, %hi(.got.plt)
     sub    t1, t1, t3               # shifted .got.plt offset + hdr size + 12
     l[w|d] t3, %lo(.got.plt)(t2)    # _dl_runtime_resolve
     addi   t1, t1, -(hdr size + 12) # shifted .got.plt offset
     addi   t0, t2, %lo(.got.plt)    # &.got.plt
     srli   t1, t1, log2(16/PTRSIZE) # .got.plt offset
     l[w|d] t0, PTRSIZE(t0)          # link map
     jr     t3 */

  entry[0] = RISCV_UTYPE (AUIPC, X_T2, gotplt_offset_high);
  entry[1] = RISCV_RTYPE (SUB, X_T1, X_T1, X_T3);
  entry[2] = RISCV_ITYPE (LREG, X_T3, X_T2, gotplt_offset_low);
  entry[3] = RISCV_ITYPE (ADDI, X_T1, X_T1, -(PLT_HEADER_SIZE + 12));
  entry[4] = RISCV_ITYPE (ADDI, X_T0, X_T2, gotplt_offset_low);
  entry[5] = RISCV_ITYPE (SRLI, X_T1, X_T1, 4 - RISCV_ELF_LOG_WORD_BYTES);
  entry[6] = RISCV_ITYPE (LREG, X_T0, X_T0, RISCV_ELF_WORD_BYTES);
  entry[7] = RISCV_ITYPE (JALR, 0, X_T3, 0);
}

/* Generate a PLT entry.  */

static void
riscv_make_plt_entry (bfd_vma got, bfd_vma addr, uint32_t *entry)
{
  /* auipc  t3, %hi(.got.plt entry)
     l[w|d] t3, %lo(.got.plt entry)(t3)
     jalr   t1, t3
     nop */

  entry[0] = RISCV_UTYPE (AUIPC, X_T3, RISCV_PCREL_HIGH_PART (got, addr));
  entry[1] = RISCV_ITYPE (LREG,  X_T3, X_T3, RISCV_PCREL_LOW_PART (got, addr));
  entry[2] = RISCV_ITYPE (JALR, X_T1, X_T3, 0);
  entry[3] = RISCV_NOP;
}

/* Create an entry in an RISC-V ELF linker hash table.  */

static struct bfd_hash_entry *
link_hash_newfunc (struct bfd_hash_entry *entry,
		   struct bfd_hash_table *table, const char *string)
{
  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (entry == NULL)
    {
      entry =
	bfd_hash_allocate (table,
			   sizeof (struct riscv_elf_link_hash_entry));
      if (entry == NULL)
	return entry;
    }

  /* Call the allocation method of the superclass.  */
  entry = _bfd_elf_link_hash_newfunc (entry, table, string);
  if (entry != NULL)
    {
      struct riscv_elf_link_hash_entry *eh;

      eh = (struct riscv_elf_link_hash_entry *) entry;
      eh->dyn_relocs = NULL;
      eh->tls_type = GOT_UNKNOWN;
    }

  return entry;
}

/* Create a RISC-V ELF linker hash table.  */

static struct bfd_link_hash_table *
riscv_elf_link_hash_table_create (bfd *abfd)
{
  struct riscv_elf_link_hash_table *ret;
  bfd_size_type amt = sizeof (struct riscv_elf_link_hash_table);

  ret = (struct riscv_elf_link_hash_table *) bfd_zmalloc (amt);
  if (ret == NULL)
    return NULL;

  if (!_bfd_elf_link_hash_table_init (&ret->elf, abfd, link_hash_newfunc,
				      sizeof (struct riscv_elf_link_hash_entry),
				      RISCV_ELF_DATA))
    {
      free (ret);
      return NULL;
    }

  return &ret->elf.root;
}

/* Create the .got section.  */

static bfd_boolean
riscv_elf_create_got_section (bfd *abfd, struct bfd_link_info *info)
{
  flagword flags;
  asection *s, *s_got;
  struct elf_link_hash_entry *h;
  const struct elf_backend_data *bed = get_elf_backend_data (abfd);
  struct elf_link_hash_table *htab = elf_hash_table (info);

  /* This function may be called more than once.  */
  if (htab->sgot != NULL)
    return TRUE;

  flags = bed->dynamic_sec_flags;

  s = bfd_make_section_anyway_with_flags (abfd,
					  (bed->rela_plts_and_copies_p
					   ? ".rela.got" : ".rel.got"),
					  (bed->dynamic_sec_flags
					   | SEC_READONLY));
  if (s == NULL
      || ! bfd_set_section_alignment (abfd, s, bed->s->log_file_align))
    return FALSE;
  htab->srelgot = s;

  s = s_got = bfd_make_section_anyway_with_flags (abfd, ".got", flags);
  if (s == NULL
      || !bfd_set_section_alignment (abfd, s, bed->s->log_file_align))
    return FALSE;
  htab->sgot = s;

  /* The first bit of the global offset table is the header.  */
  s->size += bed->got_header_size;

  if (bed->want_got_plt)
    {
      s = bfd_make_section_anyway_with_flags (abfd, ".got.plt", flags);
      if (s == NULL
	  || !bfd_set_section_alignment (abfd, s,
					 bed->s->log_file_align))
	return FALSE;
      htab->sgotplt = s;

      /* Reserve room for the header.  */
      s->size += GOTPLT_HEADER_SIZE;
    }

  if (bed->want_got_sym)
    {
      /* Define the symbol _GLOBAL_OFFSET_TABLE_ at the start of the .got
	 section.  We don't do this in the linker script because we don't want
	 to define the symbol if we are not creating a global offset
	 table.  */
      h = _bfd_elf_define_linkage_sym (abfd, info, s_got,
				       "_GLOBAL_OFFSET_TABLE_");
      elf_hash_table (info)->hgot = h;
      if (h == NULL)
	return FALSE;
    }

  return TRUE;
}

/* Create .plt, .rela.plt, .got, .got.plt, .rela.got, .dynbss, and
   .rela.bss sections in DYNOBJ, and set up shortcuts to them in our
   hash table.  */

static bfd_boolean
riscv_elf_create_dynamic_sections (bfd *dynobj,
				   struct bfd_link_info *info)
{
  struct riscv_elf_link_hash_table *htab;

  htab = riscv_elf_hash_table (info);
  BFD_ASSERT (htab != NULL);

  if (!riscv_elf_create_got_section (dynobj, info))
    return FALSE;

  if (!_bfd_elf_create_dynamic_sections (dynobj, info))
    return FALSE;

  if (!bfd_link_pic (info))
    {
      htab->sdyntdata =
	bfd_make_section_anyway_with_flags (dynobj, ".tdata.dyn",
					    SEC_ALLOC | SEC_THREAD_LOCAL);
    }

  if (!htab->elf.splt || !htab->elf.srelplt || !htab->elf.sdynbss
      || (!bfd_link_pic (info) && (!htab->elf.srelbss || !htab->sdyntdata)))
    abort ();

  return TRUE;
}

/* Copy the extra info we tack onto an elf_link_hash_entry.  */

static void
riscv_elf_copy_indirect_symbol (struct bfd_link_info *info,
				struct elf_link_hash_entry *dir,
				struct elf_link_hash_entry *ind)
{
  struct riscv_elf_link_hash_entry *edir, *eind;

  edir = (struct riscv_elf_link_hash_entry *) dir;
  eind = (struct riscv_elf_link_hash_entry *) ind;

  if (eind->dyn_relocs != NULL)
    {
      if (edir->dyn_relocs != NULL)
	{
	  struct riscv_elf_dyn_relocs **pp;
	  struct riscv_elf_dyn_relocs *p;

	  /* Add reloc counts against the indirect sym to the direct sym
	     list.  Merge any entries against the same section.  */
	  for (pp = &eind->dyn_relocs; (p = *pp) != NULL; )
	    {
	      struct riscv_elf_dyn_relocs *q;

	      for (q = edir->dyn_relocs; q != NULL; q = q->next)
		if (q->sec == p->sec)
		  {
		    q->pc_count += p->pc_count;
		    q->count += p->count;
		    *pp = p->next;
		    break;
		  }
	      if (q == NULL)
		pp = &p->next;
	    }
	  *pp = edir->dyn_relocs;
	}

      edir->dyn_relocs = eind->dyn_relocs;
      eind->dyn_relocs = NULL;
    }

  if (ind->root.type == bfd_link_hash_indirect
      && dir->got.refcount <= 0)
    {
      edir->tls_type = eind->tls_type;
      eind->tls_type = GOT_UNKNOWN;
    }
  _bfd_elf_link_hash_copy_indirect (info, dir, ind);
}

static bfd_boolean
riscv_elf_record_tls_type (bfd *abfd, struct elf_link_hash_entry *h,
			   unsigned long symndx, char tls_type)
{
  char *new_tls_type = &_bfd_riscv_elf_tls_type (abfd, h, symndx);

  *new_tls_type |= tls_type;
  if ((*new_tls_type & GOT_NORMAL) && (*new_tls_type & ~GOT_NORMAL))
    {
      (*_bfd_error_handler)
	(_("%B: `%s' accessed both as normal and thread local symbol"),
	 abfd, h ? h->root.root.string : "<local>");
      return FALSE;
    }
  return TRUE;
}

static bfd_boolean
riscv_elf_record_got_reference (bfd *abfd, struct bfd_link_info *info,
				struct elf_link_hash_entry *h, long symndx)
{
  struct riscv_elf_link_hash_table *htab = riscv_elf_hash_table (info);
  Elf_Internal_Shdr *symtab_hdr = &elf_tdata (abfd)->symtab_hdr;

  if (htab->elf.sgot == NULL)
    {
      if (!riscv_elf_create_got_section (htab->elf.dynobj, info))
	return FALSE;
    }

  if (h != NULL)
    {
      h->got.refcount += 1;
      return TRUE;
    }

  /* This is a global offset table entry for a local symbol.  */
  if (elf_local_got_refcounts (abfd) == NULL)
    {
      bfd_size_type size = symtab_hdr->sh_info * (sizeof (bfd_vma) + 1);
      if (!(elf_local_got_refcounts (abfd) = bfd_zalloc (abfd, size)))
	return FALSE;
      _bfd_riscv_elf_local_got_tls_type (abfd)
	= (char *) (elf_local_got_refcounts (abfd) + symtab_hdr->sh_info);
    }
  elf_local_got_refcounts (abfd) [symndx] += 1;

  return TRUE;
}

static bfd_boolean
bad_static_reloc (bfd *abfd, unsigned r_type, struct elf_link_hash_entry *h)
{
  (*_bfd_error_handler)
    (_("%B: relocation %s against `%s' can not be used when making a shared "
       "object; recompile with -fPIC"),
      abfd, riscv_elf_rtype_to_howto (r_type)->name,
      h != NULL ? h->root.root.string : "a local symbol");
  bfd_set_error (bfd_error_bad_value);
  return FALSE;
}

/* Pulp add on for proprietary dynmaic relocation */
typedef struct PulpImportRef {
        Elf_Internal_Rela       Rel;
        struct PulpImportRef    *Next;
} PulpImportRef;

typedef struct PulpImportEntry {
        char                    *Name;
	int			RelocCount;
        PulpImportRef           *Ref;
        struct PulpImportEntry  *Next;
} PulpImportEntry;

typedef struct PulpExportEntry {
        char                    *Name;
	unsigned int		Address;
        struct PulpExportEntry  *Next;
} PulpExportEntry;

#define HASH_IMPORT_E 1024

static PulpImportEntry * ImportEntries[HASH_IMPORT_E];
static PulpExportEntry * ExportEntries[HASH_IMPORT_E];

static struct bfd_sym_chain ComponentEntry;
static bfd_boolean ComponentEntryProvided;

#if 64 == 32
void PulpRegisterSymbolEntry(struct bfd_sym_chain EntrySymb, bfd_boolean EntryOnCmdLine)

{
	ComponentEntry = EntrySymb;
	ComponentEntryProvided = EntryOnCmdLine;
}
#endif


static unsigned long hash_sdbm(const char *str)

{
        unsigned long hash = 0;
        int c;

        while ((c = (*str++))) hash = c + (hash << 6) + (hash << 16) - hash;
        return (hash % HASH_IMPORT_E);
}

static bfd_boolean ExportLookup(const char *Name)

{
	unsigned int Index = hash_sdbm(Name);
	PulpExportEntry *PtEntry = ExportEntries[Index];

	while (PtEntry && (strcmp(PtEntry->Name, Name) != 0)) PtEntry = PtEntry->Next;

	return (PtEntry != NULL);
}

#if 64 == 32
bfd_boolean InsertExportEntry(const char *Name)

{
	unsigned int Index = hash_sdbm(Name);
	PulpExportEntry *PtEntry = ExportEntries[Index];
	PulpExportEntry *PtPrevEntry = NULL;

	while (PtEntry && (strcmp(PtEntry->Name, Name) != 0)) {
		PtPrevEntry = PtEntry; PtEntry = PtEntry->Next;
	}
	if (PtEntry == NULL) {
		PtEntry = (PulpExportEntry *) bfd_malloc (sizeof (PulpExportEntry));
		if (PtEntry == NULL) return FALSE;
		PtEntry->Name = (char *) bfd_malloc (sizeof (char) * (strlen(Name)+1));
		if (PtEntry->Name == NULL) return FALSE;
		strcpy(PtEntry->Name, Name);
		PtEntry->Address = 0; PtEntry->Next = NULL;
		if (PtPrevEntry) PtPrevEntry->Next = PtEntry; else ExportEntries[Index] = PtEntry;
	}
	return TRUE;
}

unsigned int ExportSectionSize(unsigned int *EntryCount)

{
	int i;
	PulpExportEntry *PtEntry;
	unsigned int Size = 4;	/* Room for Number of Exported Symbs */
	unsigned int Entry = 0;
	
	for (i=0; i<HASH_IMPORT_E; i++) {
		if (ExportEntries[i] == NULL) continue;
		for (PtEntry = ExportEntries[i]; PtEntry; PtEntry = PtEntry->Next) {
			Entry++;
			Size += (strlen(PtEntry->Name)+1+1);	// Null terminated and one byte prefix to pass section index
		}
	}
	/* Align on 4 bytes */
	if (Size % 4) {
		Size = ((Size>>2)+1)<<2;
	}
	/* Add Value section */
	Size += Entry*4;
	if (EntryCount) *EntryCount = Entry;
	if (Entry == 0) Size = 0;
	return Size;
}

#endif

static bfd_boolean ReleaseExportEntry(void)

{
	int i;
	PulpExportEntry *PtEntry, *NextEntry;
	
	for (i=0; i<HASH_IMPORT_E; i++) {
		if (ExportEntries[i] == NULL) continue;
		for (PtEntry = ExportEntries[i]; PtEntry; PtEntry = NextEntry) {
			NextEntry = PtEntry->Next;
			free(PtEntry->Name); free(PtEntry);
		}
		ExportEntries[i] = NULL;
	}
	return TRUE;
}

static bfd_boolean InsertImportEntry(const char *Name, Elf_Internal_Rela *Rel, bfd_vma OutOffset, bfd_boolean Collect)

{
	unsigned int Index = hash_sdbm(Name);
	PulpImportEntry *PtEntry = ImportEntries[Index];
	PulpImportEntry *PtPrevEntry = NULL;
	PulpImportRef *Ref, *PtRef;
	static PulpImportEntry *LastEntry = NULL;
	static Elf_Internal_Rela *LastRel = NULL;

	while (PtEntry && (strcmp(PtEntry->Name, Name) != 0)) {
		PtPrevEntry = PtEntry; PtEntry = PtEntry->Next;
	}
	if (PtEntry == NULL) {
		PtEntry = (PulpImportEntry *) bfd_malloc (sizeof (PulpImportEntry));
		if (PtEntry == NULL) return FALSE;
		PtEntry->Name = (char *) bfd_malloc (sizeof (char) * (strlen(Name)+1));
		if (PtEntry->Name == NULL) return FALSE;
		strcpy(PtEntry->Name, Name);
		PtEntry->Ref = NULL; PtEntry->Next = NULL;
		PtEntry->RelocCount = 0;
		if (PtPrevEntry) PtPrevEntry->Next = PtEntry; else ImportEntries[Index] = PtEntry;
	}
	if (Collect) {
		LastEntry = PtEntry;
		if ((ELF64_R_TYPE(Rel->r_info) == R_RISCV_LO12_I) && (LastEntry == PtEntry) &&
		    (ELF64_R_TYPE(LastRel->r_info) == R_RISCV_HI20) && ((Rel->r_offset - LastRel->r_offset) == 4) ) {
		} else PtEntry->RelocCount = PtEntry->RelocCount + 1;
		LastRel = Rel;
		return TRUE;
	}

	Ref = (PulpImportRef *) bfd_malloc (sizeof (PulpImportRef));
	if (Ref == NULL) return FALSE;
	Ref->Rel = *Rel; Ref->Next = NULL;
	Ref->Rel.r_info = ELF64_R_TYPE(Rel->r_info);
	Ref->Rel.r_offset = Rel->r_offset + OutOffset;
/*
	if (ComponentMode == 0) {
		Ref->Rel.r_offset += Sec->vma;
	}
*/
	PtRef = PtEntry->Ref;
	while (PtRef && PtRef->Next != NULL) PtRef = PtRef->Next;
	if (PtRef) PtRef->Next = Ref; else PtEntry->Ref = Ref;
	return TRUE;
}

static bfd_boolean ReleaseImportEntry(void)

{
	int i;
	PulpImportEntry *PtEntry, *NextEntry;
	PulpImportRef *PtRef, *NextRef;
	
	for (i=0; i<HASH_IMPORT_E; i++) {
		if (ImportEntries[i] == NULL) continue;
		for (PtEntry = ImportEntries[i]; PtEntry; PtEntry = NextEntry) {
			NextEntry = PtEntry->Next;
			for (PtRef = PtEntry->Ref; PtRef; PtRef = NextRef) {
				NextRef = PtRef->Next;
				free(PtRef);
			}
			free(PtEntry->Name); free(PtEntry);
		}
		ImportEntries[i] = NULL;
	}
	return TRUE;
}

/*

N imported symbols

Structure of .pulp.import.names Section:
----------------------------------------
We use only Type 0
	Len(Name) = Length(Name)+1 				Null terminated string
	Size:	Pad4((N+1)*4 + Sum(j:1..N){Len(Namej)})		If Type=0
	     	(N+1)*4						If Type=1

		Base						Bit0: 		Section Type: 0 with names, 1 uses pre resolved indexes
								Bit1:31:	(Section size) / 4 Always 4 byte aligned

		Base						Bit  0:11 	NumberOfImports
								Bit 12:31 	(Size of Names Section) / 4.

	Type=0 (Names)
		Base+4						Name1_Index = Base+4*(N+1)
		Base+8						Name2_Index = Base+4*(N+1)+Len(Name1)
		...
		Base+4*(i)					Namei_Index = Base+4*(N+1)+Sum(j:1..(i-1)){Len(Namej)}
		...
		Base+4*(N)					NameN_Index = Base+4*(N+1)+Sum(j:1..(N-1)){Len(Namej)}
		Base+4*(N+1)					Name1
		Base+4*(N+1)+Len(Name1)				Name2
		...
		Base+4*(N+1)+Sum(j:1..(i-1)){Len(Namej)}	Namei	
		...
		Base+4*(N+1)+Sum(j:1..(N-1)){Len(Namej)}	NameN	
		Pad till next address aligned on 4 bytes
	Type=1 (Pre resolved indexes)
		Base+4						Name1_Index (points to corresponding name in .export)
		Base+8						Name2_Index (points to corresponding name in .export)
		...
		Base+4*(i)					Namei_Index (points to corresponding name in .export)
		...
		Base+4*(N)					NameN_Index (points to corresponding name in .export)

Structure of .pulp.import.reloc Section:
----------------------------------------

	Size for Namei:	4*(N_Reloc(i) + 1)
	Total Size:	4 + Sum(j:1..N){4*(N_Reloc(j) + 1)}

	Entry(i+1) = Entry(i) + 4*(N_Reloc(i) + 1)

		Base			Bit  0:11 		NumberOfImports
					Bit 12:31 		(Size of Relocs Section) / 4. Contains only words thus multiple of 4 bytes

		Base+4			Name_Index(1)		In .pulp.import.names
		Base+6			N_Reloc(1)		Number of reloc for this name
		Base+8			Reloc			One reloc
		Base+12			Reloc			One reloc
		...
		Base+4+4*N_Reloc(1)	Reloc			Size for relocs: N_Reloc(1)*4, Total for Name1: 4*(N_Reloc(1) + 1)
		....

	Reloc:
		Reloc Type: 4 Bits  	=> Bit31 : Bit28
			0	R_RISCV_JAL					Offset = @Name-pc
					pc: 	jal (pc+Offset[20..1])		InsnBits[31:12] =>  I[20],I[10:1],I[11],I[19:12]
			1	Pair of R_RISCV_HI20, R_RISCV_LO12_I
					pc:	lui Reg,Hi20(Name)		InsBits[31:12] => @Name[31:12]
					pc+4:	addi Reg, Reg, Lo12(Name)	InsBits[31:20] => @Name[11:0}
			2	R_RISCV_HI20
					pc:	lui Reg,Hi20(Name)		InsBits[31:12] => @Name[31:12]
			3	R_RISCV_LO12_I
					pc:	addi Reg, Reg, Lo12(Name)	InsBits[31:20] => @Name[11:0}
			4	R_RISCV_LO12_S
					pc:					InsnBits[31:25] => @Name[15:5], InsnBits[11:7] => @Name[4:0]

		Reloc Offset: 28 Bits	=> Bit27 : Bit0				Offset from section base / 2.
										On RiscV we can assume that offset is always a multiple of 2


Structure of .pulp.export Section:
----------------------------------

	Total Size:	Pad4(4 + +Sum(j=1..N){Len(Namej)+1}) + 4*N. Is a multiple of 4

	Base						Bit0		0: Resident, 1: Component
							Bit15:Bit1 	N: Number of exported names
							Bit31:Bit16 	Offset/4 to first Value in this section. /4 since all entities are words
	Base+4						Section Name1	Section (byte) in which name is defined, Null terminated name
	Base+4+Len(Name1)				Section Name2
	...
	Base+4+Sum(j=1..i-1){Len(Namej)+1}		Section Namei
	...
	Base+4+Sum(j=1..N-1){Len(Namej)+1}		Section NameN

	Base+4+Sum(j=1..N){Len(Namej+1)}+Pad4		Value1	Link time Offset for Name1	Pad4: Alignment to 4
	...
	Base+4+Sum(j=1..N){Len(Namej+1)}+Pad4+4*N	ValueN	Link time Offset for NameN
	

*/

#define	IMPORT_REL_JAL			0
#define	IMPORT_REL_HI20_LO12_I		1
#define	IMPORT_REL_HI20			2
#define	IMPORT_REL_LO12_I		3
#define	IMPORT_REL_LO12_S		4

#define	IMPORT_SECN_NAME_SZ		4
#define	IMPORT_SECN_TYPE_SZ		4
#define	IMPORT_SECN_NAME_INDEX_SZ	4

#define	IMPORT_SECR_IMPORT_CNT_SZ	4
#define	IMPORT_SECR_NAME_INDEX_SZ	2
#define	IMPORT_SECR_REL_CNT_SZ		2
#define	IMPORT_SECR_REL_EXPR_SZ		4

static char *RelImage(unsigned int Rel)

{
	switch (Rel) {
		case IMPORT_REL_JAL: return "REL_JAL";
		case IMPORT_REL_HI20_LO12_I : return "REL_HI20_LO12_I";
		case IMPORT_REL_HI20: return "REL_HI20";
		case IMPORT_REL_LO12_I: return "REL_LO12_I";
		case IMPORT_REL_LO12_S: return "REL_LO12_S";
		default: return "Unknown Rel";
	}
}
static unsigned int PulpImportNameSize(const char *Name)

{
	unsigned int Size = strlen(Name)+1;
	return Size;
}

#if 64 == 32
void PulpImportSectionsSize(int Mode, unsigned int *SecName, unsigned int *SecReloc, unsigned int *N_Import, bfd_boolean Collect)

{
	int i;
	PulpImportEntry *PtEntry;
	PulpImportRef *PtRef;
	unsigned int NameSize = IMPORT_SECN_TYPE_SZ;		/* Name Section Type */
	unsigned int RefSize = IMPORT_SECR_IMPORT_CNT_SZ;	/* N Import */
	unsigned int N_Imp = 0;
	int Skip = 0;
	
	for (i=0; i<HASH_IMPORT_E; i++) {
		if (ImportEntries[i] == NULL) continue;
		for (PtEntry = ImportEntries[i]; PtEntry; PtEntry = PtEntry->Next) {
			if (((PtEntry->RelocCount == 0)&&Collect) || ((PtEntry->Ref == NULL)&&(!Collect))) continue;
			RefSize += IMPORT_SECR_NAME_INDEX_SZ;		/* Name Index */
			RefSize += IMPORT_SECR_REL_CNT_SZ;		/* Number of Relocs for Name Index */
			NameSize += IMPORT_SECN_NAME_INDEX_SZ;		/* Name Index */
			N_Imp ++;
			if (Mode == 0) NameSize += PulpImportNameSize(PtEntry->Name);
			if (Collect) RefSize += PtEntry->RelocCount*IMPORT_SECR_REL_EXPR_SZ;
			else {
				for (PtRef = PtEntry->Ref; PtRef; PtRef = PtRef->Next) {
					if (Skip) {
						Skip = 0; continue;
					}
					if ((PtRef->Rel.r_info == R_RISCV_HI20) && PtRef->Next &&
					    (PtRef->Next->Rel.r_info == R_RISCV_LO12_I) && ((PtRef->Next->Rel.r_offset - PtRef->Rel.r_offset)==4)) Skip = 1;
					RefSize += IMPORT_SECR_REL_EXPR_SZ;	/* Reloc Expr */
				}
			}
		}
	}
	/* Force Names section size to be 4 bytes aligned */
	if (NameSize%4) NameSize = ((NameSize>>2)+1)<<2;
	*SecName = NameSize;
	*SecReloc = RefSize;
	*N_Import = N_Imp;
}
#endif

static bfd_boolean PulpExportCreateSection(unsigned int **Section, unsigned int *SizeSection, struct bfd_link_info *info)

{
	PulpExportEntry *PtEntry;
	char *Base = NULL;
	unsigned int *BaseI;
	unsigned int *Entries;
	unsigned int Entry = 0, BaseLinkedVal, Addr = 0;
	unsigned int Size = ExportSectionSize(&Entry);
	int i, j;

	*SizeSection = Size;
	if (Size == 0) {
		*Section = NULL; return TRUE;
	}
	*Section = (unsigned int *) bfd_malloc (Size);
	Entries = (unsigned int *) bfd_malloc (Entry*sizeof(unsigned int));

	if (*Section == NULL || Entries == NULL) {
	  	(*_bfd_error_handler) (_("Export Create Section, Can't allocate memory"));
		return FALSE;
	}

	Entry = 0;
	Base = (char *) (&(*Section)[1]);
	Addr = 4;
	for (i=0; i<HASH_IMPORT_E; i++) {
		if (ExportEntries[i] == NULL) continue;
		for (PtEntry = ExportEntries[i]; PtEntry; PtEntry = PtEntry->Next) {
			int Len = (int) strlen(PtEntry->Name);
  			struct bfd_link_hash_entry *h;
  			h = bfd_link_hash_lookup (info->hash, PtEntry->Name, FALSE, FALSE, TRUE);
			if (h == NULL) {
	  			(*_bfd_error_handler) (_("Export Create Section, Can't find symbol: %s"), PtEntry->Name);
				return FALSE;
			}
			if (ComponentMode)
				// We don't want lma or vma added here just an offset relative to the beginning of the output section in which it is
				Entries[Entry++] = h->u.def.value + h->u.def.section->output_offset;
			else 
				Entries[Entry++] = h->u.def.value + sec_addr (h->u.def.section);
				// Entries[Entry++] = h->u.def.value + h->u.def.section->output_offset + h->u.def.section->lma;
			Base[0] = 0; /* Here should come the section in which the symbol is defined */
			for (j=0; j<Len; j++)  Base[j+1] = PtEntry->Name[j];
			Base[j+1] = 0; /* Null termination */
			Base += (Len+2); Addr += (Len+2);
		}
	}
	{
		unsigned long int Base1 = (unsigned long int) Base;
		if (Base1 % 4) Base1 = ((Base1>>2)+1)<<2;
		BaseI = (unsigned int *) Base1;
		if (Addr % 4) Addr = ((Addr>>2)+1)<<2;
	}
	BaseLinkedVal = Addr;
	Entry = 0;
	for (i=0; i<HASH_IMPORT_E; i++) {
		if (ExportEntries[i] == NULL) continue;
		for (PtEntry = ExportEntries[i]; PtEntry; PtEntry = PtEntry->Next) {
			BaseI[Entry] = Entries[Entry]; Entry++;
		}
	}
	(*Section)[0] = (ComponentMode&0x01) | ((Entry<<1)&0x0FFFE) | ((BaseLinkedVal>>2) << 16);
	free(Entries);
	return TRUE;
}

static bfd_boolean PulpImportCreateNameAndRelocSections(int Mode,
						 unsigned int **S_Name, unsigned int *S_NameSize,
						 unsigned int **S_Reloc, unsigned int *S_RelocSize,
						 unsigned int *NImport)

{
	PulpImportEntry *PtEntry;
	PulpImportRef *PtRef;
	unsigned int SecNameSize;
	unsigned int SecRelocSize;
	unsigned int *SecName;
	unsigned int *SecReloc;
	unsigned int N_Import;
	int Skip = 0;
	unsigned N_Imp = 0;
	unsigned int HeadName, HeadRel;
	unsigned int i;
	static int Trace = 0;

	PulpImportSectionsSize(Mode, &SecNameSize, &SecRelocSize, &N_Import, FALSE);

	SecName = (unsigned int *) bfd_malloc (SecNameSize);
	SecReloc = (unsigned int *) bfd_malloc (SecRelocSize);
	*S_Name = SecName; *S_NameSize = SecNameSize;
	*S_Reloc = SecReloc; *S_RelocSize = SecRelocSize;

	if (SecName == NULL || SecReloc == NULL) return FALSE;
	// SecName[0]  = (Mode&0x1) | ((SecNameSize)<<1);

	for (i=0; i< (SecNameSize>>2); i++) SecName[i] = 0;
	for (i=0; i< (SecRelocSize>>2); i++) SecReloc[i] = 0;

	SecName[0] = (N_Import&0x0FFF) | ((SecNameSize>>2)<<12);
	HeadName = (1 + N_Import)*4;

	if (Trace) fprintf(stderr, "Names: Size: %d, Relocs: Size: %d, N Imports: %d, Head Strings: %X\n", SecNameSize, SecRelocSize, N_Import, HeadName*4);

	for (i=0; i<HASH_IMPORT_E; i++) {
		if (ImportEntries[i] == NULL) continue;
		for (PtEntry = ImportEntries[i]; PtEntry; PtEntry = PtEntry->Next) {
			unsigned int j;
			unsigned int NameSize;
			char *Pt;

			if (PtEntry->Ref == NULL) continue;

			if (Trace) fprintf(stderr, "At: %5X Creating Name  Entry: %5d, String: %5X (%s)\n", (1+N_Imp)*4, N_Imp, HeadName, PtEntry->Name);
			SecName[1+N_Imp] = HeadName; N_Imp++;
			if (Mode == 1) continue;
			NameSize = PulpImportNameSize(PtEntry->Name);
			Pt = ((char *) SecName) + HeadName;
			for (j=0; j<strlen(PtEntry->Name); j++) Pt[j] = PtEntry->Name[j];
			Pt[j] = 0;
			if (Trace) fprintf(stderr, "At: %5X Creating Name String: %s\n", HeadName, PtEntry->Name);
			HeadName += NameSize;
		}
	}
	SecReloc[0] = (N_Import&0x0FFF) | ((SecRelocSize>>2)<<12); N_Imp = 0; HeadRel = 1;
	for (i=0; i<HASH_IMPORT_E; i++) {
		unsigned int Base;
		if (ImportEntries[i] == NULL) continue;
		for (PtEntry = ImportEntries[i]; PtEntry; PtEntry = PtEntry->Next) {
			unsigned int RelCount = 0;
			if (PtEntry->Ref == NULL) continue;
			Base = HeadRel++;
			for (PtRef = PtEntry->Ref; PtRef; PtRef = PtRef->Next) {
				unsigned int Rel;
				if (Skip) {
					Skip = 0; continue;
				}
				if ((PtRef->Rel.r_info == R_RISCV_HI20) && PtRef->Next &&
				    (PtRef->Next->Rel.r_info == R_RISCV_LO12_I) && ((PtRef->Next->Rel.r_offset - PtRef->Rel.r_offset)==4)) {
					Skip = 1;
					Rel = IMPORT_REL_HI20_LO12_I;
				} else {
					switch (PtRef->Rel.r_info) {
						case R_RISCV_JAL: Rel = IMPORT_REL_JAL; break;
						case R_RISCV_HI20: Rel = IMPORT_REL_HI20; break;
						case R_RISCV_LO12_I: Rel = IMPORT_REL_LO12_I; break;
						case R_RISCV_LO12_S: Rel = IMPORT_REL_LO12_S; break;
						default: {
								reloc_howto_type *howto = riscv_elf_rtype_to_howto (PtRef->Rel.r_info);
								Rel = -1; /* Error */
	  							(*_bfd_error_handler) (_("Unknown Relocation: %X (%s)"),
											(int) PtRef->Rel.r_info,
											howto?howto->name:"Unknown");
								return FALSE;
							}
					}
				}
				if (Trace) fprintf(stderr, "At: %5X Adding   Rel for Entry: %5d => %8X [Rel:%8X, Offset: %8X]\n",
						   HeadRel*4, N_Imp+1, (unsigned int) ((Rel<<28) | ((PtRef->Rel.r_offset>>1) & 0x0FFFFFFF)),
						   (unsigned int) Rel, (unsigned int) PtRef->Rel.r_offset);
				SecReloc[HeadRel++] = (Rel<<28) | ((PtRef->Rel.r_offset>>1) & 0x0FFFFFFF);
				RelCount++;
			}
			SecReloc[Base] = ((4*(N_Imp+1))&0x0FFFF) | ((RelCount<<16)&0xFFFF0000);
			if (Trace) fprintf(stderr, "At: %5X Creating Rel     Entry: %5d, Rel Count: %d\n", Base*4, 4*(N_Imp+1), RelCount); 
			N_Imp++;
		}
	}
	*NImport = N_Import;
	return TRUE;
}

/* We adjust reloc offset to absolute address when ComponentMode=0, e.g resident mode. In this case we need to add the lma of the text section */

static void AdjustRelocsImport(unsigned int *ImportRelocs, unsigned int BaseText)

{
	unsigned int Addr, N_Import;
	unsigned int i, j;

	if (ImportRelocs == NULL) return;

	N_Import = (ImportRelocs[0] & 0x0FFF); // ??? >>1;
	Addr = 4;
	for (i=0; i<N_Import; i++) {
		unsigned int RelCount = (ImportRelocs[Addr/4]>>16) & 0x0FFFF;

		Addr += 4;
		for (j=0; j<RelCount; j++) {
			unsigned int Rel = ImportRelocs[Addr/4];
			unsigned int Offset = (((Rel & 0x0FFFFFFF)<<1)+BaseText)>>1;
			unsigned int Type = (Rel>>28);

			Rel = (Type<<28) | (Offset & 0x0FFFFFFF);
			ImportRelocs[Addr/4] = Rel;
			Addr += 4;
		}
	}
}

static void DumpCEquiv(unsigned int *Section, unsigned int Size, unsigned int Elem, char *DeclName)

{
	unsigned int i;
	unsigned DeclSize =  Size/Elem;
	unsigned short *Half = (unsigned short *) Section;
	unsigned char *Byte = (unsigned char *) Section;

	switch (Elem) {
		case 1:
			fprintf(stderr, "unsigned char %s[%d] = {\n\t", DeclName, DeclSize);
			for (i=0; i<DeclSize; i++) {
				fprintf(stderr, "0X%X, ", Byte[i]);
				if (((i+1)%5)==0) fprintf(stderr, "\n\t");
			}
			fprintf(stderr, "\n};\n\n");
			break;
		case 2:
			fprintf(stderr, "unsigned short int %s[%d] = {\n\t", DeclName, DeclSize);
			for (i=0; i<DeclSize; i++) {
				fprintf(stderr, "0X%X, ", Half[i]);
				if (((i+1)%5)==0) fprintf(stderr, "\n\t");
			}
			fprintf(stderr, "\n};\n\n");
			break;
		case 4:
			fprintf(stderr, "unsigned int %s[%d] = {\n\t", DeclName, DeclSize);
			for (i=0; i<DeclSize; i++) {
				fprintf(stderr, "0X%X, ", Section[i]);
				if (((i+1)%5)==0) fprintf(stderr, "\n\t");
			}
			fprintf(stderr, "\n};\n\n");
			break;
		default: ;
	}
}

static void DiassembleImports(unsigned int *ImportNames, unsigned SecNamesSize, unsigned int *ImportRelocs, unsigned int SecRelocsSize, unsigned int BaseText)

{
	static int RawDump = 0;
	unsigned int Addr, N_Import;
	unsigned int i, j;
	char *Name;

	if (ImportNames == NULL || ImportRelocs == NULL) {
		return;
	}
	N_Import = ImportRelocs[0] & 0x0FFF;

	fprintf(stderr, "Section: .pulp.import.names\n");
	Addr = 0;
	fprintf(stderr, "%8s  %17s %20s\n", "Offset", "Content", "Comment");
	fprintf(stderr, "%8x: 0x%15X (NImport = %d, Section Size: 0x%X)\n",
		Addr, ImportNames[Addr/4], ImportNames[Addr/4]&0x0FFF, (ImportNames[Addr/4]>>12)*4);
	Addr += 4;
	Name = (char *) &ImportNames[1];
	for (i=0; i<N_Import; i++) {
		fprintf(stderr, "%8x: 0x%15X (Import Symbol %5d, Name @ in this section)\n", Addr, ImportNames[Addr/4], i);
		Addr += 4; Name += 4;
	}
	for (i=0; i<N_Import; i++) {
		fprintf(stderr, "%8x: %17s (Import Symbol %5d)\n", Addr, Name, i);
		Addr = Addr + strlen(Name) + 1; Name = Name + strlen(Name) + 1;
	}

	fprintf(stderr, "Section: .pulp.import.relocs, Mode=%s, BaseText=%X\n", ComponentMode?"Component":"Resident", BaseText);
	Addr = 0;
	fprintf(stderr, "%8s  %17s %20s\n", "Offset", "Content", "Comment");
	fprintf(stderr, "%8x: 0x%15X (Number of Imported Symbols: %d, Section Size: 0x%X)\n",
		Addr, ImportRelocs[Addr/4], ImportRelocs[Addr/4]&0x0FFF, ((ImportRelocs[Addr/4]>>12)&0x000FFFFF)<<2);
	Addr += 4;
	for (i=0; i<N_Import; i++) {
		unsigned int Entry = (ImportRelocs[Addr/4] & 0x0FFFF);
		unsigned int RelCount = (ImportRelocs[Addr/4]>>16) & 0x0FFFF;

		Name = ((char *) ImportNames) + ImportNames[Entry>>2];
		fprintf(stderr, "%8x: 0x%15X (Name @: 0x%6X, Reloc: %3d) %s\n", Addr, ImportRelocs[Addr/4], Entry, RelCount, Name);
		Addr += 4;
		for (j=0; j<RelCount; j++) {
			unsigned int Rel = ImportRelocs[Addr/4];
			unsigned int Offset = ((Rel & 0x0FFFFFFF)<<1);
			unsigned int Type = (Rel>>28);

			fprintf(stderr, "%8x: 0x%15X (Offset: 0x%6X, Reloc: %s)\n", Addr, ImportRelocs[Addr/4], Offset, RelImage(Type));
			Addr += 4;
		}
	}
	if (RawDump) {
		unsigned int NameSize = SecNamesSize>>2, RelocSize = SecRelocsSize>>2;
		if (SecNamesSize % 4) NameSize++;
		if (SecRelocsSize % 4) RelocSize++;
		fprintf(stderr, "unsigned int CompNames[%d] = {\n\t", NameSize);
		for (i=0; i<NameSize; i++) {
			fprintf(stderr, "0X%X, ", ImportNames[i]);
			if (((i+1)%5)==0) fprintf(stderr, "\n\t");
		}
		fprintf(stderr, "\n};\n");
		fprintf(stderr, "unsigned int CompRelocs[%d] = {\n\t", RelocSize);
		for (i=0; i<RelocSize; i++) {
			fprintf(stderr, "0X%X, ", ImportRelocs[i]);
			if (((i+1)%5)==0) fprintf(stderr, "\n\t");
		}
		fprintf(stderr, "\n};\n");
	}
}

static void DiassembleExports(unsigned int *Section, unsigned int SectionSize)

{
	static int RawDump = 0;
	unsigned int Entry;
	char *Base;
	unsigned int *BaseI;
	unsigned long int Addr;
	unsigned int i;

	if (Section == NULL) return;

	fprintf(stderr, "Section: .pulp.export\n");
	Entry = (Section[0]&0x0FFFF)>>1;
	Base = (char *) (&Section[1]);

	Addr = 0;
	fprintf(stderr, "%8s  %17s %20s\n", "Offset", "Content", "Comment");
	fprintf(stderr, "%8x: 0x%15X (Type: %s, Number of Exported Symbols: %d, Base Linker Values: 0x%X, Section Size: 0x%X)\n",
		(unsigned int) Addr, Section[Addr], (Section[Addr]&0x01)?"Component":"Resident",
		(Section[Addr]&0x0FFFF)>>1, ((Section[Addr]>>16)<<2)&0x0FFFF,
		((Section[Addr]&0x0FFFF)>>1)*4 + (((Section[Addr]>>16)<<2)&0x0FFFF));
	Addr += 4;
	for (i=0; i<Entry; i++) {
		unsigned int Off = strlen(Base+1) + 2;
		fprintf(stderr, "%8x: %17s Section: %2d (Exported Symbol %5d)\n", (unsigned int) Addr, Base+1, Base[0], i);
		Base = Base + Off; Addr = Addr + Off;
	}
	while ((unsigned long int) Base % 4) {
		Base++; Addr++;
	}
	// BaseI = (unsigned int *) Base;
	Addr = (Section[0]>>16)<<2;
	BaseI = Section + (Section[0]>>16);
	for (i=0; i<Entry; i++) {
		fprintf(stderr, "%8x: 0x%15X (Exported Symbol %5d, Offset in Section)\n", (unsigned int) Addr, BaseI[i], i);
		Addr+=4;
	}
	if (RawDump) {
		unsigned int Size = SectionSize>>2;
		if (SectionSize % 4) Size++;
		fprintf(stderr, "unsigned int ExportSymb[%d] = {\n\t", Size);
		for (i=0; i<Size; i++) {
			fprintf(stderr, "0X%X, ", Section[i]);
			if (((i+1)%5)==0) fprintf(stderr, "\n\t");
		}
		fprintf(stderr, "\n};\n");
	}
}


/* Look through the relocs for a section during the first phase, and
   allocate space in the global offset table or procedure linkage
   table.  */

static bfd_boolean
riscv_elf_check_relocs (bfd *abfd, struct bfd_link_info *info,
			asection *sec, const Elf_Internal_Rela *relocs)
{
  struct riscv_elf_link_hash_table *htab;
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel;
  asection *sreloc = NULL;

  if (bfd_link_relocatable (info))
    return TRUE;

  htab = riscv_elf_hash_table (info);
  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);

  if (htab->elf.dynobj == NULL)
    htab->elf.dynobj = abfd;

  for (rel = relocs; rel < relocs + sec->reloc_count; rel++)
    {
      unsigned int r_type;
      unsigned long r_symndx;
      struct elf_link_hash_entry *h;

      r_symndx = ELF64_R_SYM (rel->r_info);
      r_type = ELF64_R_TYPE (rel->r_info);

      if (r_symndx >= NUM_SHDR_ENTRIES (symtab_hdr))
	{
	  (*_bfd_error_handler) (_("%B: bad symbol index: %d"),
				 abfd, r_symndx);
	  return FALSE;
	}

      if (r_symndx < symtab_hdr->sh_info)
	h = NULL;
      else
	{
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;

	  /* PR15323, ref flags aren't set for references in the same
	     object.  */
	  h->root.non_ir_ref = 1;
	}

      if (h && h->root.type == bfd_link_hash_defweak) {
                static int Trace = 0;
                asection *sec1;
                reloc_howto_type *howto = riscv_elf_rtype_to_howto (ELF64_R_TYPE (rel->r_info));

                sec1 = h->root.u.def.section;

                if (sec1 != NULL && (strcmp(sec1->name, ".pulp.import")==0)) {
                        if (Trace) printf("Pre Importing %15s in reloc: %4d -> %4d:%22s, at offset: (%8X + %8X) => %X\n",
                                          h->root.root.string, (int) rel->r_info,
                                          (int) ELF64_R_TYPE(rel->r_info), howto->name, (int) rel->r_offset, (int) sec1->output_offset,
                                          (int) sec1->output_offset+(int)rel->r_offset);
                        sec1->flags |= SEC_KEEP;
                        InsertImportEntry(h->root.root.string, rel, sec1->output_offset, TRUE);
                }

      }
      switch (r_type)
	{
	case R_RISCV_TLS_GD_HI20:
	  if (!riscv_elf_record_got_reference (abfd, info, h, r_symndx)
	      || !riscv_elf_record_tls_type (abfd, h, r_symndx, GOT_TLS_GD))
	    return FALSE;
	  break;

	case R_RISCV_TLS_GOT_HI20:
	  if (bfd_link_pic (info))
	    info->flags |= DF_STATIC_TLS;
	  if (!riscv_elf_record_got_reference (abfd, info, h, r_symndx)
	      || !riscv_elf_record_tls_type (abfd, h, r_symndx, GOT_TLS_IE))
	    return FALSE;
	  break;

	case R_RISCV_GOT_HI20:
	  if (!riscv_elf_record_got_reference (abfd, info, h, r_symndx)
	      || !riscv_elf_record_tls_type (abfd, h, r_symndx, GOT_NORMAL))
	    return FALSE;
	  break;

	case R_RISCV_CALL_PLT:
	  /* This symbol requires a procedure linkage table entry.  We
	     actually build the entry in adjust_dynamic_symbol,
	     because this might be a case of linking PIC code without
	     linking in any dynamic objects, in which case we don't
	     need to generate a procedure linkage table after all.  */

	  if (h != NULL)
	    {
	      h->needs_plt = 1;
	      h->plt.refcount += 1;
	    }
	  break;

	case R_RISCV_CALL:
	case R_RISCV_JAL:
	case R_RISCV_BRANCH:
	case R_RISCV_RVC_BRANCH:
	case R_RISCV_RVC_JUMP:
	case R_RISCV_PCREL_HI20:
	  /* In shared libraries, these relocs are known to bind locally.  */
	  if (bfd_link_pic (info))
	    break;
	  goto static_reloc;

	case R_RISCV_TPREL_HI20:
	  if (!bfd_link_executable (info))
	    return bad_static_reloc (abfd, r_type, h);
	  if (h != NULL)
	    riscv_elf_record_tls_type (abfd, h, r_symndx, GOT_TLS_LE);
	  goto static_reloc;

	case R_RISCV_HI20:
	  if (bfd_link_pic (info))
	    return bad_static_reloc (abfd, r_type, h);
	  /* Fall through.  */

	case R_RISCV_COPY:
	case R_RISCV_JUMP_SLOT:
	case R_RISCV_RELATIVE:
	case R_RISCV_64:
	case R_RISCV_32:
	  /* Fall through.  */

	static_reloc:
	  /* This reloc might not bind locally.  */
	  if (h != NULL)
	    h->non_got_ref = 1;

	  if (h != NULL && !bfd_link_pic (info))
	    {
	      /* We may need a .plt entry if the function this reloc
		 refers to is in a shared lib.  */
	      h->plt.refcount += 1;
	    }

	  /* If we are creating a shared library, and this is a reloc
	     against a global symbol, or a non PC relative reloc
	     against a local symbol, then we need to copy the reloc
	     into the shared library.  However, if we are linking with
	     -Bsymbolic, we do not need to copy a reloc against a
	     global symbol which is defined in an object we are
	     including in the link (i.e., DEF_REGULAR is set).  At
	     this point we have not seen all the input files, so it is
	     possible that DEF_REGULAR is not set now but will be set
	     later (it is never cleared).  In case of a weak definition,
	     DEF_REGULAR may be cleared later by a strong definition in
	     a shared library.  We account for that possibility below by
	     storing information in the relocs_copied field of the hash
	     table entry.  A similar situation occurs when creating
	     shared libraries and symbol visibility changes render the
	     symbol local.

	     If on the other hand, we are creating an executable, we
	     may need to keep relocations for symbols satisfied by a
	     dynamic library if we manage to avoid copy relocs for the
	     symbol.  */
	  if ((bfd_link_pic (info)
	       && (sec->flags & SEC_ALLOC) != 0
	       && (! riscv_elf_rtype_to_howto (r_type)->pc_relative
		   || (h != NULL
		       && (! info->symbolic
			   || h->root.type == bfd_link_hash_defweak
			   || !h->def_regular))))
	      || (!bfd_link_pic (info)
		  && (sec->flags & SEC_ALLOC) != 0
		  && h != NULL
		  && (h->root.type == bfd_link_hash_defweak
		      || !h->def_regular)))
	    {
	      struct riscv_elf_dyn_relocs *p;
	      struct riscv_elf_dyn_relocs **head;

	      /* When creating a shared object, we must copy these
		 relocs into the output file.  We create a reloc
		 section in dynobj and make room for the reloc.  */
	      if (sreloc == NULL)
		{
		  sreloc = _bfd_elf_make_dynamic_reloc_section
		    (sec, htab->elf.dynobj, RISCV_ELF_LOG_WORD_BYTES,
		    abfd, /*rela?*/ TRUE);

		  if (sreloc == NULL)
		    return FALSE;
		}

	      /* If this is a global symbol, we count the number of
		 relocations we need for this symbol.  */
	      if (h != NULL)
		head = &((struct riscv_elf_link_hash_entry *) h)->dyn_relocs;
	      else
		{
		  /* Track dynamic relocs needed for local syms too.
		     We really need local syms available to do this
		     easily.  Oh well.  */

		  asection *s;
		  void *vpp;
		  Elf_Internal_Sym *isym;

		  isym = bfd_sym_from_r_symndx (&htab->sym_cache,
						abfd, r_symndx);
		  if (isym == NULL)
		    return FALSE;

		  s = bfd_section_from_elf_index (abfd, isym->st_shndx);
		  if (s == NULL)
		    s = sec;

		  vpp = &elf_section_data (s)->local_dynrel;
		  head = (struct riscv_elf_dyn_relocs **) vpp;
		}

	      p = *head;
	      if (p == NULL || p->sec != sec)
		{
		  bfd_size_type amt = sizeof *p;
		  p = ((struct riscv_elf_dyn_relocs *)
		       bfd_alloc (htab->elf.dynobj, amt));
		  if (p == NULL)
		    return FALSE;
		  p->next = *head;
		  *head = p;
		  p->sec = sec;
		  p->count = 0;
		  p->pc_count = 0;
		}

	      p->count += 1;
	      p->pc_count += riscv_elf_rtype_to_howto (r_type)->pc_relative;
	    }

	  break;

	case R_RISCV_GNU_VTINHERIT:
	  if (!bfd_elf_gc_record_vtinherit (abfd, sec, h, rel->r_offset))
	    return FALSE;
	  break;

	case R_RISCV_GNU_VTENTRY:
	  if (!bfd_elf_gc_record_vtentry (abfd, sec, h, rel->r_addend))
	    return FALSE;
	  break;

	default:
	  break;
	}
    }

  return TRUE;
}

static asection *
riscv_elf_gc_mark_hook (asection *sec,
			struct bfd_link_info *info,
			Elf_Internal_Rela *rel,
			struct elf_link_hash_entry *h,
			Elf_Internal_Sym *sym)
{
  if (h != NULL)
    switch (ELF64_R_TYPE (rel->r_info))
      {
      case R_RISCV_GNU_VTINHERIT:
      case R_RISCV_GNU_VTENTRY:
	return NULL;
      }

  return _bfd_elf_gc_mark_hook (sec, info, rel, h, sym);
}

/* Update the got entry reference counts for the section being removed.  */

static bfd_boolean
riscv_elf_gc_sweep_hook (bfd *abfd,
			 struct bfd_link_info *info,
			 asection *sec,
			 const Elf_Internal_Rela *relocs)
{
  const Elf_Internal_Rela *rel, *relend;
  Elf_Internal_Shdr *symtab_hdr = &elf_symtab_hdr (abfd);
  struct elf_link_hash_entry **sym_hashes = elf_sym_hashes (abfd);
  bfd_signed_vma *local_got_refcounts = elf_local_got_refcounts (abfd);

  if (bfd_link_relocatable (info))
    return TRUE;

  elf_section_data (sec)->local_dynrel = NULL;

  for (rel = relocs, relend = relocs + sec->reloc_count; rel < relend; rel++)
    {
      unsigned long r_symndx;
      struct elf_link_hash_entry *h = NULL;

      r_symndx = ELF64_R_SYM (rel->r_info);
      if (r_symndx >= symtab_hdr->sh_info)
	{
	  struct riscv_elf_link_hash_entry *eh;
	  struct riscv_elf_dyn_relocs **pp;
	  struct riscv_elf_dyn_relocs *p;

	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
	  eh = (struct riscv_elf_link_hash_entry *) h;
	  for (pp = &eh->dyn_relocs; (p = *pp) != NULL; pp = &p->next)
	    if (p->sec == sec)
	      {
		/* Everything must go for SEC.  */
		*pp = p->next;
		break;
	      }
	}

      switch (ELF64_R_TYPE (rel->r_info))
	{
	case R_RISCV_GOT_HI20:
	case R_RISCV_TLS_GOT_HI20:
	case R_RISCV_TLS_GD_HI20:
	  if (h != NULL)
	    {
	      if (h->got.refcount > 0)
		h->got.refcount--;
	    }
	  else
	    {
	      if (local_got_refcounts &&
		  local_got_refcounts[r_symndx] > 0)
		local_got_refcounts[r_symndx]--;
	    }
	  break;

	case R_RISCV_HI20:
	case R_RISCV_PCREL_HI20:
	case R_RISCV_COPY:
	case R_RISCV_JUMP_SLOT:
	case R_RISCV_RELATIVE:
	case R_RISCV_64:
	case R_RISCV_32:
	case R_RISCV_BRANCH:
	case R_RISCV_CALL:
	case R_RISCV_JAL:
	case R_RISCV_RVC_BRANCH:
	case R_RISCV_RVC_JUMP:
	  if (bfd_link_pic (info))
	    break;
	  /* Fall through.  */

	case R_RISCV_CALL_PLT:
	  if (h != NULL)
	    {
	      if (h->plt.refcount > 0)
		h->plt.refcount--;
	    }
	  break;

	default:
	  break;
	}
    }

  return TRUE;
}

/* Adjust a symbol defined by a dynamic object and referenced by a
   regular object.  The current definition is in some section of the
   dynamic object, but we're not including those sections.  We have to
   change the definition to something the rest of the link can
   understand.  */

static bfd_boolean
riscv_elf_adjust_dynamic_symbol (struct bfd_link_info *info,
				 struct elf_link_hash_entry *h)
{
  struct riscv_elf_link_hash_table *htab;
  struct riscv_elf_link_hash_entry * eh;
  struct riscv_elf_dyn_relocs *p;
  bfd *dynobj;
  asection *s, *srel;

  htab = riscv_elf_hash_table (info);
  BFD_ASSERT (htab != NULL);

  dynobj = htab->elf.dynobj;

  /* Make sure we know what is going on here.  */
  BFD_ASSERT (dynobj != NULL
	      && (h->needs_plt
		  || h->type == STT_GNU_IFUNC
		  || h->u.weakdef != NULL
		  || (h->def_dynamic
		      && h->ref_regular
		      && !h->def_regular)));

  /* If this is a function, put it in the procedure linkage table.  We
     will fill in the contents of the procedure linkage table later
     (although we could actually do it here).  */
  if (h->type == STT_FUNC || h->type == STT_GNU_IFUNC || h->needs_plt)
    {
      if (h->plt.refcount <= 0
	  || SYMBOL_CALLS_LOCAL (info, h)
	  || (ELF_ST_VISIBILITY (h->other) != STV_DEFAULT
	      && h->root.type == bfd_link_hash_undefweak))
	{
	  /* This case can occur if we saw a R_RISCV_CALL_PLT reloc in an
	     input file, but the symbol was never referred to by a dynamic
	     object, or if all references were garbage collected.  In such
	     a case, we don't actually need to build a PLT entry.  */
	  h->plt.offset = (bfd_vma) -1;
	  h->needs_plt = 0;
	}

      return TRUE;
    }
  else
    h->plt.offset = (bfd_vma) -1;

  /* If this is a weak symbol, and there is a real definition, the
     processor independent code will have arranged for us to see the
     real definition first, and we can just use the same value.  */
  if (h->u.weakdef != NULL)
    {
      BFD_ASSERT (h->u.weakdef->root.type == bfd_link_hash_defined
		  || h->u.weakdef->root.type == bfd_link_hash_defweak);
      h->root.u.def.section = h->u.weakdef->root.u.def.section;
      h->root.u.def.value = h->u.weakdef->root.u.def.value;
      return TRUE;
    }

  /* This is a reference to a symbol defined by a dynamic object which
     is not a function.  */

  /* If we are creating a shared library, we must presume that the
     only references to the symbol are via the global offset table.
     For such cases we need not do anything here; the relocations will
     be handled correctly by relocate_section.  */
  if (bfd_link_pic (info))
    return TRUE;

  /* If there are no references to this symbol that do not use the
     GOT, we don't need to generate a copy reloc.  */
  if (!h->non_got_ref)
    return TRUE;

  /* If -z nocopyreloc was given, we won't generate them either.  */
  if (info->nocopyreloc)
    {
      h->non_got_ref = 0;
      return TRUE;
    }

  eh = (struct riscv_elf_link_hash_entry *) h;
  for (p = eh->dyn_relocs; p != NULL; p = p->next)
    {
      s = p->sec->output_section;
      if (s != NULL && (s->flags & SEC_READONLY) != 0)
	break;
    }

  /* If we didn't find any dynamic relocs in read-only sections, then
     we'll be keeping the dynamic relocs and avoiding the copy reloc.  */
  if (p == NULL)
    {
      h->non_got_ref = 0;
      return TRUE;
    }

  /* We must allocate the symbol in our .dynbss section, which will
     become part of the .bss section of the executable.  There will be
     an entry for this symbol in the .dynsym section.  The dynamic
     object will contain position independent code, so all references
     from the dynamic object to this symbol will go through the global
     offset table.  The dynamic linker will use the .dynsym entry to
     determine the address it must put in the global offset table, so
     both the dynamic object and the regular object will refer to the
     same memory location for the variable.  */

  /* We must generate a R_RISCV_COPY reloc to tell the dynamic linker
     to copy the initial value out of the dynamic object and into the
     runtime process image.  We need to remember the offset into the
     .rel.bss section we are going to use.  */
  if ((h->root.u.def.section->flags & SEC_READONLY) != 0)
    {
      s = htab->elf.sdynrelro;
      srel = htab->elf.sreldynrelro;
    }
  else
    {
      s = htab->elf.sdynbss;
      srel = htab->elf.srelbss;
    }
  if ((h->root.u.def.section->flags & SEC_ALLOC) != 0 && h->size != 0)
    {
      srel->size += sizeof (Elf64_External_Rela);
      h->needs_copy = 1;
    }

  if (eh->tls_type & ~GOT_NORMAL)
    return _bfd_elf_adjust_dynamic_copy (info, h, htab->sdyntdata);

  return _bfd_elf_adjust_dynamic_copy (info, h, s);
}

/* Allocate space in .plt, .got and associated reloc sections for
   dynamic relocs.  */

static bfd_boolean
allocate_dynrelocs (struct elf_link_hash_entry *h, void *inf)
{
  struct bfd_link_info *info;
  struct riscv_elf_link_hash_table *htab;
  struct riscv_elf_link_hash_entry *eh;
  struct riscv_elf_dyn_relocs *p;

  if (h->root.type == bfd_link_hash_indirect)
    return TRUE;

  info = (struct bfd_link_info *) inf;
  htab = riscv_elf_hash_table (info);
  BFD_ASSERT (htab != NULL);

  if (htab->elf.dynamic_sections_created
      && h->plt.refcount > 0)
    {
      /* Make sure this symbol is output as a dynamic symbol.
	 Undefined weak syms won't yet be marked as dynamic.  */
      if (h->dynindx == -1
	  && !h->forced_local)
	{
	  if (! bfd_elf_link_record_dynamic_symbol (info, h))
	    return FALSE;
	}

      if (WILL_CALL_FINISH_DYNAMIC_SYMBOL (1, bfd_link_pic (info), h))
	{
	  asection *s = htab->elf.splt;

	  if (s->size == 0)
	    s->size = PLT_HEADER_SIZE;

	  h->plt.offset = s->size;

	  /* Make room for this entry.  */
	  s->size += PLT_ENTRY_SIZE;

	  /* We also need to make an entry in the .got.plt section.  */
	  htab->elf.sgotplt->size += GOT_ENTRY_SIZE;

	  /* We also need to make an entry in the .rela.plt section.  */
	  htab->elf.srelplt->size += sizeof (Elf64_External_Rela);

	  /* If this symbol is not defined in a regular file, and we are
	     not generating a shared library, then set the symbol to this
	     location in the .plt.  This is required to make function
	     pointers compare as equal between the normal executable and
	     the shared library.  */
	  if (! bfd_link_pic (info)
	      && !h->def_regular)
	    {
	      h->root.u.def.section = s;
	      h->root.u.def.value = h->plt.offset;
	    }
	}
      else
	{
	  h->plt.offset = (bfd_vma) -1;
	  h->needs_plt = 0;
	}
    }
  else
    {
      h->plt.offset = (bfd_vma) -1;
      h->needs_plt = 0;
    }

  if (h->got.refcount > 0)
    {
      asection *s;
      bfd_boolean dyn;
      int tls_type = riscv_elf_hash_entry (h)->tls_type;

      /* Make sure this symbol is output as a dynamic symbol.
	 Undefined weak syms won't yet be marked as dynamic.  */
      if (h->dynindx == -1
	  && !h->forced_local)
	{
	  if (! bfd_elf_link_record_dynamic_symbol (info, h))
	    return FALSE;
	}

      s = htab->elf.sgot;
      h->got.offset = s->size;
      dyn = htab->elf.dynamic_sections_created;
      if (tls_type & (GOT_TLS_GD | GOT_TLS_IE))
	{
	  /* TLS_GD needs two dynamic relocs and two GOT slots.  */
	  if (tls_type & GOT_TLS_GD)
	    {
	      s->size += 2 * RISCV_ELF_WORD_BYTES;
	      htab->elf.srelgot->size += 2 * sizeof (Elf64_External_Rela);
	    }

	  /* TLS_IE needs one dynamic reloc and one GOT slot.  */
	  if (tls_type & GOT_TLS_IE)
	    {
	      s->size += RISCV_ELF_WORD_BYTES;
	      htab->elf.srelgot->size += sizeof (Elf64_External_Rela);
	    }
	}
      else
	{
	  s->size += RISCV_ELF_WORD_BYTES;
	  if (WILL_CALL_FINISH_DYNAMIC_SYMBOL (dyn, bfd_link_pic (info), h))
	    htab->elf.srelgot->size += sizeof (Elf64_External_Rela);
	}
    }
  else
    h->got.offset = (bfd_vma) -1;

  eh = (struct riscv_elf_link_hash_entry *) h;
  if (eh->dyn_relocs == NULL)
    return TRUE;

  /* In the shared -Bsymbolic case, discard space allocated for
     dynamic pc-relative relocs against symbols which turn out to be
     defined in regular objects.  For the normal shared case, discard
     space for pc-relative relocs that have become local due to symbol
     visibility changes.  */

  if (bfd_link_pic (info))
    {
      if (SYMBOL_CALLS_LOCAL (info, h))
	{
	  struct riscv_elf_dyn_relocs **pp;

	  for (pp = &eh->dyn_relocs; (p = *pp) != NULL; )
	    {
	      p->count -= p->pc_count;
	      p->pc_count = 0;
	      if (p->count == 0)
		*pp = p->next;
	      else
		pp = &p->next;
	    }
	}

      /* Also discard relocs on undefined weak syms with non-default
	 visibility.  */
      if (eh->dyn_relocs != NULL
	  && h->root.type == bfd_link_hash_undefweak)
	{
	  if (ELF_ST_VISIBILITY (h->other) != STV_DEFAULT)
	    eh->dyn_relocs = NULL;

	  /* Make sure undefined weak symbols are output as a dynamic
	     symbol in PIEs.  */
	  else if (h->dynindx == -1
		   && !h->forced_local)
	    {
	      if (! bfd_elf_link_record_dynamic_symbol (info, h))
		return FALSE;
	    }
	}
    }
  else
    {
      /* For the non-shared case, discard space for relocs against
	 symbols which turn out to need copy relocs or are not
	 dynamic.  */

      if (!h->non_got_ref
	  && ((h->def_dynamic
	       && !h->def_regular)
	      || (htab->elf.dynamic_sections_created
		  && (h->root.type == bfd_link_hash_undefweak
		      || h->root.type == bfd_link_hash_undefined))))
	{
	  /* Make sure this symbol is output as a dynamic symbol.
	     Undefined weak syms won't yet be marked as dynamic.  */
	  if (h->dynindx == -1
	      && !h->forced_local)
	    {
	      if (! bfd_elf_link_record_dynamic_symbol (info, h))
		return FALSE;
	    }

	  /* If that succeeded, we know we'll be keeping all the
	     relocs.  */
	  if (h->dynindx != -1)
	    goto keep;
	}

      eh->dyn_relocs = NULL;

    keep: ;
    }

  /* Finally, allocate space.  */
  for (p = eh->dyn_relocs; p != NULL; p = p->next)
    {
      asection *sreloc = elf_section_data (p->sec)->sreloc;
      sreloc->size += p->count * sizeof (Elf64_External_Rela);
    }

  return TRUE;
}

/* Find any dynamic relocs that apply to read-only sections.  */

static bfd_boolean
readonly_dynrelocs (struct elf_link_hash_entry *h, void *inf)
{
  struct riscv_elf_link_hash_entry *eh;
  struct riscv_elf_dyn_relocs *p;

  eh = (struct riscv_elf_link_hash_entry *) h;
  for (p = eh->dyn_relocs; p != NULL; p = p->next)
    {
      asection *s = p->sec->output_section;

      if (s != NULL && (s->flags & SEC_READONLY) != 0)
	{
	  ((struct bfd_link_info *) inf)->flags |= DF_TEXTREL;
	  return FALSE;
	}
    }
  return TRUE;
}

static bfd_boolean
riscv_elf_size_dynamic_sections (bfd *output_bfd, struct bfd_link_info *info)
{
  struct riscv_elf_link_hash_table *htab;
  bfd *dynobj;
  asection *s;
  bfd *ibfd;

  htab = riscv_elf_hash_table (info);
  BFD_ASSERT (htab != NULL);
  dynobj = htab->elf.dynobj;
  BFD_ASSERT (dynobj != NULL);

  if (elf_hash_table (info)->dynamic_sections_created)
    {
      /* Set the contents of the .interp section to the interpreter.  */
      if (bfd_link_executable (info) && !info->nointerp)
	{
	  s = bfd_get_linker_section (dynobj, ".interp");
	  BFD_ASSERT (s != NULL);
	  s->size = strlen (ELF64_DYNAMIC_INTERPRETER) + 1;
	  s->contents = (unsigned char *) ELF64_DYNAMIC_INTERPRETER;
	}
    }

  /* Set up .got offsets for local syms, and space for local dynamic
     relocs.  */
  for (ibfd = info->input_bfds; ibfd != NULL; ibfd = ibfd->link.next)
    {
      bfd_signed_vma *local_got;
      bfd_signed_vma *end_local_got;
      char *local_tls_type;
      bfd_size_type locsymcount;
      Elf_Internal_Shdr *symtab_hdr;
      asection *srel;

      if (! is_riscv_elf (ibfd))
	continue;

      for (s = ibfd->sections; s != NULL; s = s->next)
	{
	  struct riscv_elf_dyn_relocs *p;

	  for (p = elf_section_data (s)->local_dynrel; p != NULL; p = p->next)
	    {
	      if (!bfd_is_abs_section (p->sec)
		  && bfd_is_abs_section (p->sec->output_section))
		{
		  /* Input section has been discarded, either because
		     it is a copy of a linkonce section or due to
		     linker script /DISCARD/, so we'll be discarding
		     the relocs too.  */
		}
	      else if (p->count != 0)
		{
		  srel = elf_section_data (p->sec)->sreloc;
		  srel->size += p->count * sizeof (Elf64_External_Rela);
		  if ((p->sec->output_section->flags & SEC_READONLY) != 0)
		    info->flags |= DF_TEXTREL;
		}
	    }
	}

      local_got = elf_local_got_refcounts (ibfd);
      if (!local_got)
	continue;

      symtab_hdr = &elf_symtab_hdr (ibfd);
      locsymcount = symtab_hdr->sh_info;
      end_local_got = local_got + locsymcount;
      local_tls_type = _bfd_riscv_elf_local_got_tls_type (ibfd);
      s = htab->elf.sgot;
      srel = htab->elf.srelgot;
      for (; local_got < end_local_got; ++local_got, ++local_tls_type)
	{
	  if (*local_got > 0)
	    {
	      *local_got = s->size;
	      s->size += RISCV_ELF_WORD_BYTES;
	      if (*local_tls_type & GOT_TLS_GD)
		s->size += RISCV_ELF_WORD_BYTES;
	      if (bfd_link_pic (info)
		  || (*local_tls_type & (GOT_TLS_GD | GOT_TLS_IE)))
		srel->size += sizeof (Elf64_External_Rela);
	    }
	  else
	    *local_got = (bfd_vma) -1;
	}
    }

  /* Allocate global sym .plt and .got entries, and space for global
     sym dynamic relocs.  */
  elf_link_hash_traverse (&htab->elf, allocate_dynrelocs, info);

  if (htab->elf.sgotplt)
    {
      struct elf_link_hash_entry *got;
      got = elf_link_hash_lookup (elf_hash_table (info),
				  "_GLOBAL_OFFSET_TABLE_",
				  FALSE, FALSE, FALSE);

      /* Don't allocate .got.plt section if there are no GOT nor PLT
	 entries and there is no refeence to _GLOBAL_OFFSET_TABLE_.  */
      if ((got == NULL
	   || !got->ref_regular_nonweak)
	  && (htab->elf.sgotplt->size == GOTPLT_HEADER_SIZE)
	  && (htab->elf.splt == NULL
	      || htab->elf.splt->size == 0)
	  && (htab->elf.sgot == NULL
	      || (htab->elf.sgot->size
		  == get_elf_backend_data (output_bfd)->got_header_size)))
	htab->elf.sgotplt->size = 0;
    }

  /* The check_relocs and adjust_dynamic_symbol entry points have
     determined the sizes of the various dynamic sections.  Allocate
     memory for them.  */
  for (s = dynobj->sections; s != NULL; s = s->next)
    {
      if ((s->flags & SEC_LINKER_CREATED) == 0)
	continue;

      if (s == htab->elf.splt
	  || s == htab->elf.sgot
	  || s == htab->elf.sgotplt
	  || s == htab->elf.sdynbss
	  || s == htab->elf.sdynrelro)
	{
	  /* Strip this section if we don't need it; see the
	     comment below.  */
	}
      else if (strncmp (s->name, ".rela", 5) == 0)
	{
	  if (s->size != 0)
	    {
	      /* We use the reloc_count field as a counter if we need
		 to copy relocs into the output file.  */
	      s->reloc_count = 0;
	    }
	}
      else
	{
	  /* It's not one of our sections.  */
	  continue;
	}

      if (s->size == 0)
	{
	  /* If we don't need this section, strip it from the
	     output file.  This is mostly to handle .rela.bss and
	     .rela.plt.  We must create both sections in
	     create_dynamic_sections, because they must be created
	     before the linker maps input sections to output
	     sections.  The linker does that before
	     adjust_dynamic_symbol is called, and it is that
	     function which decides whether anything needs to go
	     into these sections.  */
	  s->flags |= SEC_EXCLUDE;
	  continue;
	}

      if ((s->flags & SEC_HAS_CONTENTS) == 0)
	continue;

      /* Allocate memory for the section contents.  Zero the memory
	 for the benefit of .rela.plt, which has 4 unused entries
	 at the beginning, and we don't want garbage.  */
      s->contents = (bfd_byte *) bfd_zalloc (dynobj, s->size);
      if (s->contents == NULL)
	return FALSE;
    }

  if (elf_hash_table (info)->dynamic_sections_created)
    {
      /* Add some entries to the .dynamic section.  We fill in the
	 values later, in riscv_elf_finish_dynamic_sections, but we
	 must add the entries now so that we get the correct size for
	 the .dynamic section.  The DT_DEBUG entry is filled in by the
	 dynamic linker and used by the debugger.  */
#define add_dynamic_entry(TAG, VAL) \
  _bfd_elf_add_dynamic_entry (info, TAG, VAL)

      if (bfd_link_executable (info))
	{
	  if (!add_dynamic_entry (DT_DEBUG, 0))
	    return FALSE;
	}

      if (htab->elf.srelplt->size != 0)
	{
	  if (!add_dynamic_entry (DT_PLTGOT, 0)
	      || !add_dynamic_entry (DT_PLTRELSZ, 0)
	      || !add_dynamic_entry (DT_PLTREL, DT_RELA)
	      || !add_dynamic_entry (DT_JMPREL, 0))
	    return FALSE;
	}

      if (!add_dynamic_entry (DT_RELA, 0)
	  || !add_dynamic_entry (DT_RELASZ, 0)
	  || !add_dynamic_entry (DT_RELAENT, sizeof (Elf64_External_Rela)))
	return FALSE;

      /* If any dynamic relocs apply to a read-only section,
	 then we need a DT_TEXTREL entry.  */
      if ((info->flags & DF_TEXTREL) == 0)
	elf_link_hash_traverse (&htab->elf, readonly_dynrelocs, info);

      if (info->flags & DF_TEXTREL)
	{
	  if (!add_dynamic_entry (DT_TEXTREL, 0))
	    return FALSE;
	}
    }
#undef add_dynamic_entry

  return TRUE;
}

#define TP_OFFSET 0
#define DTP_OFFSET 0x800

/* Return the relocation value for a TLS dtp-relative reloc.  */

static bfd_vma
dtpoff (struct bfd_link_info *info, bfd_vma address)
{
  /* If tls_sec is NULL, we should have signalled an error already.  */
  if (elf_hash_table (info)->tls_sec == NULL)
    return 0;
  return address - elf_hash_table (info)->tls_sec->vma - DTP_OFFSET;
}

/* Return the relocation value for a static TLS tp-relative relocation.  */

static bfd_vma
tpoff (struct bfd_link_info *info, bfd_vma address)
{
  /* If tls_sec is NULL, we should have signalled an error already.  */
  if (elf_hash_table (info)->tls_sec == NULL)
    return 0;
  return address - elf_hash_table (info)->tls_sec->vma - TP_OFFSET;
}

/* Return the global pointer's value, or 0 if it is not in use.  */

static bfd_vma
riscv_global_pointer_value (struct bfd_link_info *info)
{
  struct bfd_link_hash_entry *h;

  h = bfd_link_hash_lookup (info->hash, RISCV_GP_SYMBOL, FALSE, FALSE, TRUE);
  if (h == NULL || h->type != bfd_link_hash_defined)
    return 0;

  return h->u.def.value + sec_addr (h->u.def.section);
}

/* Emplace a static relocation.  */

static bfd_reloc_status_type
perform_relocation (const reloc_howto_type *howto,
		    const Elf_Internal_Rela *rel,
		    bfd_vma value,
		    asection *input_section,
		    bfd *input_bfd,
		    bfd_byte *contents,
                    bfd_boolean IsImport)
{
  if (howto->pc_relative)
    value -= sec_addr (input_section) + rel->r_offset;
  value += rel->r_addend;

  switch (ELF64_R_TYPE (rel->r_info))
    {
    case R_RISCV_HI20:
    case R_RISCV_TPREL_HI20:
    case R_RISCV_PCREL_HI20:
    case R_RISCV_GOT_HI20:
    case R_RISCV_TLS_GOT_HI20:
    case R_RISCV_TLS_GD_HI20:
      if (ARCH_SIZE > 32 && !VALID_UTYPE_IMM (RISCV_CONST_HIGH_PART (value)))
	return bfd_reloc_overflow;
      value = ENCODE_UTYPE_IMM (RISCV_CONST_HIGH_PART (value));
      break;
    /* Pulp specific relocs */
    case R_RISCV_12_I:
      if (!VALID_ITYPE_IMM (value)) return bfd_reloc_overflow;
      value = ENCODE_ITYPE_IMM (value);
      break;
    case R_RISCV_12_S:
      if (!VALID_STYPE_IMM (value)) return bfd_reloc_overflow;
      value = ENCODE_STYPE_IMM (value);
      break;

    case R_RISCV_REL12:
      value = ENCODE_ITYPE_IMM (value>>howto->rightshift);
      break;
    case R_RISCV_RELU5:
      value = ENCODE_I1TYPE_UIMM (value>>howto->rightshift);
      break;
    /* End of Pulp specific relocs */
    case R_RISCV_LO12_I:
    case R_RISCV_GPREL_I:
    case R_RISCV_TPREL_LO12_I:
    case R_RISCV_TPREL_I:
    case R_RISCV_PCREL_LO12_I:
      value = ENCODE_ITYPE_IMM (value);
      break;

    case R_RISCV_LO12_S:
    case R_RISCV_GPREL_S:
    case R_RISCV_TPREL_LO12_S:
    case R_RISCV_TPREL_S:
    case R_RISCV_PCREL_LO12_S:
      value = ENCODE_STYPE_IMM (value);
      break;

    case R_RISCV_CALL:
    case R_RISCV_CALL_PLT:
      if (ARCH_SIZE > 32 && !VALID_UTYPE_IMM (RISCV_CONST_HIGH_PART (value)))
	return bfd_reloc_overflow;
      value = ENCODE_UTYPE_IMM (RISCV_CONST_HIGH_PART (value))
	      | (ENCODE_ITYPE_IMM (value) << 32);
      break;

    case R_RISCV_JAL:
      if (!IsImport && !VALID_UJTYPE_IMM (value)) return bfd_reloc_overflow;
      value = ENCODE_UJTYPE_IMM (value);
      break;

    case R_RISCV_BRANCH:
      if (!VALID_SBTYPE_IMM (value))
	return bfd_reloc_overflow;
      value = ENCODE_SBTYPE_IMM (value);
      break;

    case R_RISCV_RVC_BRANCH:
      if (!VALID_RVC_B_IMM (value))
	return bfd_reloc_overflow;
      value = ENCODE_RVC_B_IMM (value);
      break;

    case R_RISCV_RVC_JUMP:
      if (!VALID_RVC_J_IMM (value))
	return bfd_reloc_overflow;
      value = ENCODE_RVC_J_IMM (value);
      break;

    case R_RISCV_RVC_LUI:
      if (!VALID_RVC_LUI_IMM (RISCV_CONST_HIGH_PART (value)))
	return bfd_reloc_overflow;
      value = ENCODE_RVC_LUI_IMM (RISCV_CONST_HIGH_PART (value));
      break;

    case R_RISCV_32:
    case R_RISCV_64:
    case R_RISCV_ADD8:
    case R_RISCV_ADD16:
    case R_RISCV_ADD32:
    case R_RISCV_ADD64:
    case R_RISCV_SUB6:
    case R_RISCV_SUB8:
    case R_RISCV_SUB16:
    case R_RISCV_SUB32:
    case R_RISCV_SUB64:
    case R_RISCV_SET6:
    case R_RISCV_SET8:
    case R_RISCV_SET16:
    case R_RISCV_SET32:
    case R_RISCV_TLS_DTPREL32:
    case R_RISCV_TLS_DTPREL64:
      break;

    default:
      return bfd_reloc_notsupported;
    }

  bfd_vma word = bfd_get (howto->bitsize, input_bfd, contents + rel->r_offset);
  word = (word & ~howto->dst_mask) | (value & howto->dst_mask);
  bfd_put (howto->bitsize, input_bfd, word, contents + rel->r_offset);

  return bfd_reloc_ok;
}

/* Remember all PC-relative high-part relocs we've encountered to help us
   later resolve the corresponding low-part relocs.  */

typedef struct
{
  bfd_vma address;
  bfd_vma value;
} riscv_pcrel_hi_reloc;

typedef struct riscv_pcrel_lo_reloc
{
  asection *                     input_section;
  struct bfd_link_info *         info;
  reloc_howto_type *             howto;
  const Elf_Internal_Rela *      reloc;
  bfd_vma                        addr;
  const char *                   name;
  bfd_byte *                     contents;
  struct riscv_pcrel_lo_reloc *  next;
} riscv_pcrel_lo_reloc;

typedef struct
{
  htab_t hi_relocs;
  riscv_pcrel_lo_reloc *lo_relocs;
} riscv_pcrel_relocs;

static hashval_t
riscv_pcrel_reloc_hash (const void *entry)
{
  const riscv_pcrel_hi_reloc *e = entry;
  return (hashval_t)(e->address >> 2);
}

static bfd_boolean
riscv_pcrel_reloc_eq (const void *entry1, const void *entry2)
{
  const riscv_pcrel_hi_reloc *e1 = entry1, *e2 = entry2;
  return e1->address == e2->address;
}

static bfd_boolean
riscv_init_pcrel_relocs (riscv_pcrel_relocs *p)
{

  p->lo_relocs = NULL;
  p->hi_relocs = htab_create (1024, riscv_pcrel_reloc_hash,
			      riscv_pcrel_reloc_eq, free);
  return p->hi_relocs != NULL;
}

static void
riscv_free_pcrel_relocs (riscv_pcrel_relocs *p)
{
  riscv_pcrel_lo_reloc *cur = p->lo_relocs;

  while (cur != NULL)
    {
      riscv_pcrel_lo_reloc *next = cur->next;
      free (cur);
      cur = next;
    }

  htab_delete (p->hi_relocs);
}

static bfd_boolean
riscv_record_pcrel_hi_reloc (riscv_pcrel_relocs *p, bfd_vma addr, bfd_vma value)
{
  riscv_pcrel_hi_reloc entry = {addr, value - addr};
  riscv_pcrel_hi_reloc **slot =
    (riscv_pcrel_hi_reloc **) htab_find_slot (p->hi_relocs, &entry, INSERT);

  BFD_ASSERT (*slot == NULL);
  *slot = (riscv_pcrel_hi_reloc *) bfd_malloc (sizeof (riscv_pcrel_hi_reloc));
  if (*slot == NULL)
    return FALSE;
  **slot = entry;
  return TRUE;
}

static bfd_boolean
riscv_record_pcrel_lo_reloc (riscv_pcrel_relocs *p,
			     asection *input_section,
			     struct bfd_link_info *info,
			     reloc_howto_type *howto,
			     const Elf_Internal_Rela *reloc,
			     bfd_vma addr,
			     const char *name,
			     bfd_byte *contents)
{
  riscv_pcrel_lo_reloc *entry;
  entry = (riscv_pcrel_lo_reloc *) bfd_malloc (sizeof (riscv_pcrel_lo_reloc));
  if (entry == NULL)
    return FALSE;
  *entry = (riscv_pcrel_lo_reloc) {input_section, info, howto, reloc, addr,
				   name, contents, p->lo_relocs};
  p->lo_relocs = entry;
  return TRUE;
}

static bfd_boolean
riscv_resolve_pcrel_lo_relocs (riscv_pcrel_relocs *p)
{
  riscv_pcrel_lo_reloc *r;

  for (r = p->lo_relocs; r != NULL; r = r->next)
    {
      bfd *input_bfd = r->input_section->owner;

      riscv_pcrel_hi_reloc search = {r->addr, 0};
      riscv_pcrel_hi_reloc *entry = htab_find (p->hi_relocs, &search);
      if (entry == NULL)
        {
	  ((*r->info->callbacks->reloc_overflow)
	   (r->info, NULL, r->name, r->howto->name, (bfd_vma) 0,
	    input_bfd, r->input_section, r->reloc->r_offset));
	  return TRUE;
        }

      perform_relocation (r->howto, r->reloc, entry->value, r->input_section,
			  input_bfd, r->contents, FALSE);
    }

  return TRUE;
}

static bfd_boolean RegisterImportReloc(struct bfd_link_info *info,
                                bfd *input_bfd,
                                asection *input_section,
                                Elf_Internal_Rela *rel,
                                unsigned long r_symndx,
                                Elf_Internal_Shdr *symtab_hdr,
                                struct elf_link_hash_entry **sym_hashes,
                                reloc_howto_type *howto)

{
        struct elf_link_hash_entry *h;
        asection *sec;

        if (sym_hashes == NULL) return FALSE;

        /* It seems this can happen with erroneous or unsupported input (mixing a.out and elf in an archive, for example.)  */
        h = sym_hashes[r_symndx - symtab_hdr->sh_info];

        if (info->wrap_hash != NULL && (input_section->flags & SEC_DEBUGGING) != 0)
                h = ((struct elf_link_hash_entry *) unwrap_hash_lookup (info, input_bfd, &h->root));

        while (h->root.type == bfd_link_hash_indirect || h->root.type == bfd_link_hash_warning)
                h = (struct elf_link_hash_entry *) h->root.u.i.link;
        if (h->root.type == bfd_link_hash_defweak) {
                sec = h->root.u.def.section;
                if (sec != NULL && sec->output_section != NULL && (strcmp(sec->name, ".pulp.import")==0)) {
                        static int Trace = 0;
                        sec->flags |= SEC_KEEP;
                        if (Trace) printf("    Importing %15s in reloc: %4d -> %4d:%22s, at offset: (%8X + %8X) => %X\n",
                                          h->root.root.string, (int) rel->r_info,
                                          (int) ELF64_R_TYPE(rel->r_info), howto->name, (int) rel->r_offset, (int) input_section->output_offset,
                                          (int) ((int) input_section->output_offset+(int)rel->r_offset));
                        InsertImportEntry(h->root.root.string, rel, input_section->output_offset, FALSE);
                        return TRUE;
                }
        }
        return FALSE;
}


/* Relocate a RISC-V ELF section.

   The RELOCATE_SECTION function is called by the new ELF backend linker
   to handle the relocations for a section.

   The relocs are always passed as Rela structures.

   This function is responsible for adjusting the section contents as
   necessary, and (if generating a relocatable output file) adjusting
   the reloc addend as necessary.

   This function does not have to worry about setting the reloc
   address or the reloc symbol index.

   LOCAL_SYMS is a pointer to the swapped in local symbols.

   LOCAL_SECTIONS is an array giving the section in the input file
   corresponding to the st_shndx field of each local symbol.

   The global hash table entry for the global symbols can be found
   via elf_sym_hashes (input_bfd).

   When generating relocatable output, this function must handle
   STB_LOCAL/STT_SECTION symbols specially.  The output symbol is
   going to be the section symbol corresponding to the output
   section, which means that the addend must be adjusted
   accordingly.  */

static bfd_boolean
riscv_elf_relocate_section (bfd *output_bfd,
			    struct bfd_link_info *info,
			    bfd *input_bfd,
			    asection *input_section,
			    bfd_byte *contents,
			    Elf_Internal_Rela *relocs,
			    Elf_Internal_Sym *local_syms,
			    asection **local_sections)
{
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;
  riscv_pcrel_relocs pcrel_relocs;
  bfd_boolean ret = FALSE;
  asection *sreloc = elf_section_data (input_section)->sreloc;
  struct riscv_elf_link_hash_table *htab = riscv_elf_hash_table (info);
  Elf_Internal_Shdr *symtab_hdr = &elf_symtab_hdr (input_bfd);
  struct elf_link_hash_entry **sym_hashes = elf_sym_hashes (input_bfd);
  bfd_vma *local_got_offsets = elf_local_got_offsets (input_bfd);

  if (!riscv_init_pcrel_relocs (&pcrel_relocs))
    return FALSE;

  relend = relocs + input_section->reloc_count;
  for (rel = relocs; rel < relend; rel++)
    {
      unsigned long r_symndx;
      struct elf_link_hash_entry *h;
      Elf_Internal_Sym *sym;
      asection *sec;
      bfd_vma relocation;
      bfd_reloc_status_type r = bfd_reloc_ok;
      const char *name;
      bfd_vma off, ie_off;
      bfd_boolean unresolved_reloc, is_ie = FALSE, IsImport = FALSE;
      bfd_vma pc = sec_addr (input_section) + rel->r_offset;
      int r_type = ELF64_R_TYPE (rel->r_info), tls_type;
      reloc_howto_type *howto = riscv_elf_rtype_to_howto (r_type);
      const char *msg = NULL;

      if (r_type == R_RISCV_GNU_VTINHERIT || r_type == R_RISCV_GNU_VTENTRY)
	continue;

      /* This is a final link.  */
      r_symndx = ELF64_R_SYM (rel->r_info);
      h = NULL;
      sym = NULL;
      sec = NULL;
      unresolved_reloc = FALSE;
      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];
	  relocation = _bfd_elf_rela_local_sym (output_bfd, sym, &sec, rel);
	}
      else
	{
	  bfd_boolean warned, ignored;

	  IsImport = RegisterImportReloc(info, input_bfd, input_section, rel, r_symndx, symtab_hdr, sym_hashes, howto);

	  RELOC_FOR_GLOBAL_SYMBOL (info, input_bfd, input_section, rel,
				   r_symndx, symtab_hdr, sym_hashes,
				   h, sec, relocation,
				   unresolved_reloc, warned, ignored);
	  if (warned)
	    {
	      /* To avoid generating warning messages about truncated
		 relocations, set the relocation's address to be the same as
		 the start of this section.  */
	      if (input_section->output_section != NULL)
		relocation = input_section->output_section->vma;
	      else
		relocation = 0;
	    }
	}

      if (sec != NULL && discarded_section (sec))
	RELOC_AGAINST_DISCARDED_SECTION (info, input_bfd, input_section,
					 rel, 1, relend, howto, 0, contents);

      if (bfd_link_relocatable (info))
	continue;

      if (h != NULL)
	name = h->root.root.string;
      else
	{
	  name = (bfd_elf_string_from_elf_section
		  (input_bfd, symtab_hdr->sh_link, sym->st_name));
	  if (name == NULL || *name == '\0')
	    name = bfd_section_name (input_bfd, sec);
	}

      switch (r_type)
	{
	case R_RISCV_NONE:
	case R_RISCV_RELAX:
	case R_RISCV_TPREL_ADD:
	case R_RISCV_COPY:
	case R_RISCV_JUMP_SLOT:
	case R_RISCV_RELATIVE:
	  /* These require nothing of us at all.  */
	  continue;

	case R_RISCV_HI20:
	case R_RISCV_BRANCH:
	case R_RISCV_RVC_BRANCH:
	case R_RISCV_RVC_LUI:
	case R_RISCV_LO12_I:
	case R_RISCV_LO12_S:

	/* Pulp specific */
        case R_RISCV_RELU5:
        case R_RISCV_REL12:
        case R_RISCV_12_I:
        case R_RISCV_12_S:
	/* End of Pulp specific */

	case R_RISCV_SET6:
	case R_RISCV_SET8:
	case R_RISCV_SET16:
	case R_RISCV_SET32:
	  /* These require no special handling beyond perform_relocation.  */
	  break;

	case R_RISCV_GOT_HI20:
	  if (h != NULL)
	    {
	      bfd_boolean dyn, pic;

	      off = h->got.offset;
	      BFD_ASSERT (off != (bfd_vma) -1);
	      dyn = elf_hash_table (info)->dynamic_sections_created;
	      pic = bfd_link_pic (info);

	      if (! WILL_CALL_FINISH_DYNAMIC_SYMBOL (dyn, pic, h)
		  || (pic && SYMBOL_REFERENCES_LOCAL (info, h)))
		{
		  /* This is actually a static link, or it is a
		     -Bsymbolic link and the symbol is defined
		     locally, or the symbol was forced to be local
		     because of a version file.  We must initialize
		     this entry in the global offset table.  Since the
		     offset must always be a multiple of the word size,
		     we use the least significant bit to record whether
		     we have initialized it already.

		     When doing a dynamic link, we create a .rela.got
		     relocation entry to initialize the value.  This
		     is done in the finish_dynamic_symbol routine.  */
		  if ((off & 1) != 0)
		    off &= ~1;
		  else
		    {
		      bfd_put_64 (output_bfd, relocation,
				  htab->elf.sgot->contents + off);
		      h->got.offset |= 1;
		    }
		}
	      else
		unresolved_reloc = FALSE;
	    }
	  else
	    {
	      BFD_ASSERT (local_got_offsets != NULL
			  && local_got_offsets[r_symndx] != (bfd_vma) -1);

	      off = local_got_offsets[r_symndx];

	      /* The offset must always be a multiple of the word size.
		 So, we can use the least significant bit to record
		 whether we have already processed this entry.  */
	      if ((off & 1) != 0)
		off &= ~1;
	      else
		{
		  if (bfd_link_pic (info))
		    {
		      asection *s;
		      Elf_Internal_Rela outrel;

		      /* We need to generate a R_RISCV_RELATIVE reloc
			 for the dynamic linker.  */
		      s = htab->elf.srelgot;
		      BFD_ASSERT (s != NULL);

		      outrel.r_offset = sec_addr (htab->elf.sgot) + off;
		      outrel.r_info =
			ELF64_R_INFO (0, R_RISCV_RELATIVE);
		      outrel.r_addend = relocation;
		      relocation = 0;
		      riscv_elf_append_rela (output_bfd, s, &outrel);
		    }

		  bfd_put_64 (output_bfd, relocation,
			      htab->elf.sgot->contents + off);
		  local_got_offsets[r_symndx] |= 1;
		}
	    }
	  relocation = sec_addr (htab->elf.sgot) + off;
	  if (!riscv_record_pcrel_hi_reloc (&pcrel_relocs, pc, relocation))
	    r = bfd_reloc_overflow;
	  break;

	case R_RISCV_ADD8:
	case R_RISCV_ADD16:
	case R_RISCV_ADD32:
	case R_RISCV_ADD64:
	  {
	    bfd_vma old_value = bfd_get (howto->bitsize, input_bfd,
					 contents + rel->r_offset);
	    relocation = old_value + relocation;
	  }
	  break;

	case R_RISCV_SUB6:
	case R_RISCV_SUB8:
	case R_RISCV_SUB16:
	case R_RISCV_SUB32:
	case R_RISCV_SUB64:
	  {
	    bfd_vma old_value = bfd_get (howto->bitsize, input_bfd,
					 contents + rel->r_offset);
	    relocation = old_value - relocation;
	  }
	  break;

	case R_RISCV_CALL_PLT:
	case R_RISCV_CALL:
	case R_RISCV_JAL:
	case R_RISCV_RVC_JUMP:
	  if (bfd_link_pic (info) && h != NULL && h->plt.offset != MINUS_ONE)
	    {
	      /* Refer to the PLT entry.  */
	      relocation = sec_addr (htab->elf.splt) + h->plt.offset;
	      unresolved_reloc = FALSE;
	    }
	  break;

	case R_RISCV_TPREL_HI20:
	  relocation = tpoff (info, relocation);
	  break;

	case R_RISCV_TPREL_LO12_I:
	case R_RISCV_TPREL_LO12_S:
	  relocation = tpoff (info, relocation);
	  break;

	case R_RISCV_TPREL_I:
	case R_RISCV_TPREL_S:
	  relocation = tpoff (info, relocation);
	  if (VALID_ITYPE_IMM (relocation + rel->r_addend))
	    {
	      /* We can use tp as the base register.  */
	      bfd_vma insn = bfd_get_32 (input_bfd, contents + rel->r_offset);
	      insn &= ~(OP_MASK_RS1 << OP_SH_RS1);
	      insn |= X_TP << OP_SH_RS1;
	      bfd_put_32 (input_bfd, insn, contents + rel->r_offset);
	    }
	  else
	    r = bfd_reloc_overflow;
	  break;

	case R_RISCV_GPREL_I:
	case R_RISCV_GPREL_S:
	  {
	    bfd_vma gp = riscv_global_pointer_value (info);
	    bfd_boolean x0_base = VALID_ITYPE_IMM (relocation + rel->r_addend);
	    if (x0_base || VALID_ITYPE_IMM (relocation + rel->r_addend - gp))
	      {
		/* We can use x0 or gp as the base register.  */
		bfd_vma insn = bfd_get_32 (input_bfd, contents + rel->r_offset);
		insn &= ~(OP_MASK_RS1 << OP_SH_RS1);
		if (!x0_base)
		  {
		    rel->r_addend -= gp;
		    insn |= X_GP << OP_SH_RS1;
		  }
		bfd_put_32 (input_bfd, insn, contents + rel->r_offset);
	      }
	    else
	      r = bfd_reloc_overflow;
	    break;
	  }

	case R_RISCV_PCREL_HI20:
	  if (!riscv_record_pcrel_hi_reloc (&pcrel_relocs, pc,
					    relocation + rel->r_addend))
	    r = bfd_reloc_overflow;
	  break;

	case R_RISCV_PCREL_LO12_I:
	case R_RISCV_PCREL_LO12_S:
	  if (riscv_record_pcrel_lo_reloc (&pcrel_relocs, input_section, info,
					   howto, rel, relocation, name,
					   contents))
	    continue;
	  r = bfd_reloc_overflow;
	  break;

	case R_RISCV_TLS_DTPREL32:
	case R_RISCV_TLS_DTPREL64:
	  relocation = dtpoff (info, relocation);
	  break;

	case R_RISCV_32:
	case R_RISCV_64:
	  if ((input_section->flags & SEC_ALLOC) == 0)
	    break;

	  if ((bfd_link_pic (info)
	       && (h == NULL
		   || ELF_ST_VISIBILITY (h->other) == STV_DEFAULT
		   || h->root.type != bfd_link_hash_undefweak)
	       && (! howto->pc_relative
		   || !SYMBOL_CALLS_LOCAL (info, h)))
	      || (!bfd_link_pic (info)
		  && h != NULL
		  && h->dynindx != -1
		  && !h->non_got_ref
		  && ((h->def_dynamic
		       && !h->def_regular)
		      || h->root.type == bfd_link_hash_undefweak
		      || h->root.type == bfd_link_hash_undefined)))
	    {
	      Elf_Internal_Rela outrel;
	      bfd_boolean skip_static_relocation, skip_dynamic_relocation;

	      /* When generating a shared object, these relocations
		 are copied into the output file to be resolved at run
		 time.  */

	      outrel.r_offset =
		_bfd_elf_section_offset (output_bfd, info, input_section,
					 rel->r_offset);
	      skip_static_relocation = outrel.r_offset != (bfd_vma) -2;
	      skip_dynamic_relocation = outrel.r_offset >= (bfd_vma) -2;
	      outrel.r_offset += sec_addr (input_section);

	      if (skip_dynamic_relocation)
		memset (&outrel, 0, sizeof outrel);
	      else if (h != NULL && h->dynindx != -1
		       && !(bfd_link_pic (info)
			    && SYMBOLIC_BIND (info, h)
			    && h->def_regular))
		{
		  outrel.r_info = ELF64_R_INFO (h->dynindx, r_type);
		  outrel.r_addend = rel->r_addend;
		}
	      else
		{
		  outrel.r_info = ELF64_R_INFO (0, R_RISCV_RELATIVE);
		  outrel.r_addend = relocation + rel->r_addend;
		}

	      riscv_elf_append_rela (output_bfd, sreloc, &outrel);
	      if (skip_static_relocation)
		continue;
	    }
	  break;

	case R_RISCV_TLS_GOT_HI20:
	  is_ie = TRUE;
	  /* Fall through.  */

	case R_RISCV_TLS_GD_HI20:
	  if (h != NULL)
	    {
	      off = h->got.offset;
	      h->got.offset |= 1;
	    }
	  else
	    {
	      off = local_got_offsets[r_symndx];
	      local_got_offsets[r_symndx] |= 1;
	    }

	  tls_type = _bfd_riscv_elf_tls_type (input_bfd, h, r_symndx);
	  BFD_ASSERT (tls_type & (GOT_TLS_IE | GOT_TLS_GD));
	  /* If this symbol is referenced by both GD and IE TLS, the IE
	     reference's GOT slot follows the GD reference's slots.  */
	  ie_off = 0;
	  if ((tls_type & GOT_TLS_GD) && (tls_type & GOT_TLS_IE))
	    ie_off = 2 * GOT_ENTRY_SIZE;

	  if ((off & 1) != 0)
	    off &= ~1;
	  else
	    {
	      Elf_Internal_Rela outrel;
	      int indx = 0;
	      bfd_boolean need_relocs = FALSE;

	      if (htab->elf.srelgot == NULL)
		abort ();

	      if (h != NULL)
		{
		  bfd_boolean dyn, pic;
		  dyn = htab->elf.dynamic_sections_created;
		  pic = bfd_link_pic (info);

		  if (WILL_CALL_FINISH_DYNAMIC_SYMBOL (dyn, pic, h)
		      && (!pic || !SYMBOL_REFERENCES_LOCAL (info, h)))
		    indx = h->dynindx;
		}

	      /* The GOT entries have not been initialized yet.  Do it
	         now, and emit any relocations.  */
	      if ((bfd_link_pic (info) || indx != 0)
		  && (h == NULL
		      || ELF_ST_VISIBILITY (h->other) == STV_DEFAULT
		      || h->root.type != bfd_link_hash_undefweak))
		    need_relocs = TRUE;

	      if (tls_type & GOT_TLS_GD)
		{
		  if (need_relocs)
		    {
		      outrel.r_offset = sec_addr (htab->elf.sgot) + off;
		      outrel.r_addend = 0;
		      outrel.r_info = ELF64_R_INFO (indx, R_RISCV_TLS_DTPMOD64);
		      bfd_put_64 (output_bfd, 0,
				  htab->elf.sgot->contents + off);
		      riscv_elf_append_rela (output_bfd, htab->elf.srelgot, &outrel);
		      if (indx == 0)
			{
			  BFD_ASSERT (! unresolved_reloc);
			  bfd_put_64 (output_bfd,
				      dtpoff (info, relocation),
				      (htab->elf.sgot->contents + off +
				       RISCV_ELF_WORD_BYTES));
			}
		      else
			{
			  bfd_put_64 (output_bfd, 0,
				      (htab->elf.sgot->contents + off +
				       RISCV_ELF_WORD_BYTES));
			  outrel.r_info = ELF64_R_INFO (indx, R_RISCV_TLS_DTPREL64);
			  outrel.r_offset += RISCV_ELF_WORD_BYTES;
			  riscv_elf_append_rela (output_bfd, htab->elf.srelgot, &outrel);
			}
		    }
		  else
		    {
		      /* If we are not emitting relocations for a
			 general dynamic reference, then we must be in a
			 static link or an executable link with the
			 symbol binding locally.  Mark it as belonging
			 to module 1, the executable.  */
		      bfd_put_64 (output_bfd, 1,
				  htab->elf.sgot->contents + off);
		      bfd_put_64 (output_bfd,
				  dtpoff (info, relocation),
				  (htab->elf.sgot->contents + off +
				   RISCV_ELF_WORD_BYTES));
		   }
		}

	      if (tls_type & GOT_TLS_IE)
		{
		  if (need_relocs)
		    {
		      bfd_put_64 (output_bfd, 0,
				  htab->elf.sgot->contents + off + ie_off);
		      outrel.r_offset = sec_addr (htab->elf.sgot)
				       + off + ie_off;
		      outrel.r_addend = 0;
		      if (indx == 0)
			outrel.r_addend = tpoff (info, relocation);
		      outrel.r_info = ELF64_R_INFO (indx, R_RISCV_TLS_TPREL64);
		      riscv_elf_append_rela (output_bfd, htab->elf.srelgot, &outrel);
		    }
		  else
		    {
		      bfd_put_64 (output_bfd, tpoff (info, relocation),
				  htab->elf.sgot->contents + off + ie_off);
		    }
		}
	    }

	  BFD_ASSERT (off < (bfd_vma) -2);
	  relocation = sec_addr (htab->elf.sgot) + off + (is_ie ? ie_off : 0);
	  if (!riscv_record_pcrel_hi_reloc (&pcrel_relocs, pc, relocation))
	    r = bfd_reloc_overflow;
	  unresolved_reloc = FALSE;
	  break;

	default:
	  r = bfd_reloc_notsupported;
	}

      /* Dynamic relocs are not propagated for SEC_DEBUGGING sections
	 because such sections are not SEC_ALLOC and thus ld.so will
	 not process them.  */
      if (unresolved_reloc
	  && !((input_section->flags & SEC_DEBUGGING) != 0
	       && h->def_dynamic)
	  && _bfd_elf_section_offset (output_bfd, info, input_section,
				      rel->r_offset) != (bfd_vma) -1)
	{
	  (*_bfd_error_handler)
	    (_("%B(%A+0x%lx): unresolvable %s relocation against symbol `%s'"),
	     input_bfd,
	     input_section,
	     (long) rel->r_offset,
	     howto->name,
	     h->root.root.string);
	  continue;
	}

      if (r == bfd_reloc_ok)
	r = perform_relocation (howto, rel, relocation, input_section,
				input_bfd, contents, IsImport);

      switch (r)
	{
	case bfd_reloc_ok:
	  continue;

	case bfd_reloc_overflow:
	  info->callbacks->reloc_overflow
	    (info, (h ? &h->root : NULL), name, howto->name,
	     (bfd_vma) 0, input_bfd, input_section, rel->r_offset);
	  break;

	case bfd_reloc_undefined:
	  info->callbacks->undefined_symbol
	    (info, name, input_bfd, input_section, rel->r_offset,
	     TRUE);
	  break;

	case bfd_reloc_outofrange:
	  msg = _("internal error: out of range error");
	  break;

	case bfd_reloc_notsupported:
	  msg = _("internal error: unsupported relocation error");
	  break;

	case bfd_reloc_dangerous:
	  msg = _("internal error: dangerous relocation");
	  break;

	default:
	  msg = _("internal error: unknown error");
	  break;
	}

      if (msg)
	info->callbacks->warning
	  (info, msg, name, input_bfd, input_section, rel->r_offset);
      goto out;
    }

  ret = riscv_resolve_pcrel_lo_relocs (&pcrel_relocs);
out:
  riscv_free_pcrel_relocs (&pcrel_relocs);
  return ret;
}

/* Finish up dynamic symbol handling.  We set the contents of various
   dynamic sections here.  */

static bfd_boolean
riscv_elf_finish_dynamic_symbol (bfd *output_bfd,
				 struct bfd_link_info *info,
				 struct elf_link_hash_entry *h,
				 Elf_Internal_Sym *sym)
{
  struct riscv_elf_link_hash_table *htab = riscv_elf_hash_table (info);
  const struct elf_backend_data *bed = get_elf_backend_data (output_bfd);

  if (h->plt.offset != (bfd_vma) -1)
    {
      /* We've decided to create a PLT entry for this symbol.  */
      bfd_byte *loc;
      bfd_vma i, header_address, plt_idx, got_address;
      uint32_t plt_entry[PLT_ENTRY_INSNS];
      Elf_Internal_Rela rela;

      BFD_ASSERT (h->dynindx != -1);

      /* Calculate the address of the PLT header.  */
      header_address = sec_addr (htab->elf.splt);

      /* Calculate the index of the entry.  */
      plt_idx = (h->plt.offset - PLT_HEADER_SIZE) / PLT_ENTRY_SIZE;

      /* Calculate the address of the .got.plt entry.  */
      got_address = riscv_elf_got_plt_val (plt_idx, info);

      /* Find out where the .plt entry should go.  */
      loc = htab->elf.splt->contents + h->plt.offset;

      /* Fill in the PLT entry itself.  */
      riscv_make_plt_entry (got_address, header_address + h->plt.offset,
			    plt_entry);
      for (i = 0; i < PLT_ENTRY_INSNS; i++)
	bfd_put_32 (output_bfd, plt_entry[i], loc + 4*i);

      /* Fill in the initial value of the .got.plt entry.  */
      loc = htab->elf.sgotplt->contents
	    + (got_address - sec_addr (htab->elf.sgotplt));
      bfd_put_64 (output_bfd, sec_addr (htab->elf.splt), loc);

      /* Fill in the entry in the .rela.plt section.  */
      rela.r_offset = got_address;
      rela.r_addend = 0;
      rela.r_info = ELF64_R_INFO (h->dynindx, R_RISCV_JUMP_SLOT);

      loc = htab->elf.srelplt->contents + plt_idx * sizeof (Elf64_External_Rela);
      bed->s->swap_reloca_out (output_bfd, &rela, loc);

      if (!h->def_regular)
	{
	  /* Mark the symbol as undefined, rather than as defined in
	     the .plt section.  Leave the value alone.  */
	  sym->st_shndx = SHN_UNDEF;
	  /* If the symbol is weak, we do need to clear the value.
	     Otherwise, the PLT entry would provide a definition for
	     the symbol even if the symbol wasn't defined anywhere,
	     and so the symbol would never be NULL.  */
	  if (!h->ref_regular_nonweak)
	    sym->st_value = 0;
	}
    }

  if (h->got.offset != (bfd_vma) -1
      && !(riscv_elf_hash_entry (h)->tls_type & (GOT_TLS_GD | GOT_TLS_IE)))
    {
      asection *sgot;
      asection *srela;
      Elf_Internal_Rela rela;

      /* This symbol has an entry in the GOT.  Set it up.  */

      sgot = htab->elf.sgot;
      srela = htab->elf.srelgot;
      BFD_ASSERT (sgot != NULL && srela != NULL);

      rela.r_offset = sec_addr (sgot) + (h->got.offset &~ (bfd_vma) 1);

      /* If this is a -Bsymbolic link, and the symbol is defined
	 locally, we just want to emit a RELATIVE reloc.  Likewise if
	 the symbol was forced to be local because of a version file.
	 The entry in the global offset table will already have been
	 initialized in the relocate_section function.  */
      if (bfd_link_pic (info)
	  && (info->symbolic || h->dynindx == -1)
	  && h->def_regular)
	{
	  asection *sec = h->root.u.def.section;
	  rela.r_info = ELF64_R_INFO (0, R_RISCV_RELATIVE);
	  rela.r_addend = (h->root.u.def.value
			   + sec->output_section->vma
			   + sec->output_offset);
	}
      else
	{
	  BFD_ASSERT (h->dynindx != -1);
	  rela.r_info = ELF64_R_INFO (h->dynindx, R_RISCV_64);
	  rela.r_addend = 0;
	}

      bfd_put_64 (output_bfd, 0,
		  sgot->contents + (h->got.offset & ~(bfd_vma) 1));
      riscv_elf_append_rela (output_bfd, srela, &rela);
    }

  if (h->needs_copy)
    {
      Elf_Internal_Rela rela;
      asection *s;

      /* This symbols needs a copy reloc.  Set it up.  */
      BFD_ASSERT (h->dynindx != -1);

      rela.r_offset = sec_addr (h->root.u.def.section) + h->root.u.def.value;
      rela.r_info = ELF64_R_INFO (h->dynindx, R_RISCV_COPY);
      rela.r_addend = 0;
      if (h->root.u.def.section == htab->elf.sdynrelro)
	s = htab->elf.sreldynrelro;
      else
	s = htab->elf.srelbss;
      riscv_elf_append_rela (output_bfd, s, &rela);
    }

  /* Mark some specially defined symbols as absolute.  */
  if (h == htab->elf.hdynamic
      || (h == htab->elf.hgot || h == htab->elf.hplt))
    sym->st_shndx = SHN_ABS;

  return TRUE;
}

/* Finish up the dynamic sections.  */

static bfd_boolean
riscv_finish_dyn (bfd *output_bfd, struct bfd_link_info *info,
		  bfd *dynobj, asection *sdyn)
{
  struct riscv_elf_link_hash_table *htab = riscv_elf_hash_table (info);
  const struct elf_backend_data *bed = get_elf_backend_data (output_bfd);
  size_t dynsize = bed->s->sizeof_dyn;
  bfd_byte *dyncon, *dynconend;

  dynconend = sdyn->contents + sdyn->size;
  for (dyncon = sdyn->contents; dyncon < dynconend; dyncon += dynsize)
    {
      Elf_Internal_Dyn dyn;
      asection *s;

      bed->s->swap_dyn_in (dynobj, dyncon, &dyn);

      switch (dyn.d_tag)
	{
	case DT_PLTGOT:
	  s = htab->elf.sgotplt;
	  dyn.d_un.d_ptr = s->output_section->vma + s->output_offset;
	  break;
	case DT_JMPREL:
	  s = htab->elf.srelplt;
	  dyn.d_un.d_ptr = s->output_section->vma + s->output_offset;
	  break;
	case DT_PLTRELSZ:
	  s = htab->elf.srelplt;
	  dyn.d_un.d_val = s->size;
	  break;
	default:
	  continue;
	}

      bed->s->swap_dyn_out (output_bfd, &dyn, dyncon);
    }
  return TRUE;
}

static bfd_boolean
riscv_elf_finish_dynamic_sections (bfd *output_bfd,
				   struct bfd_link_info *info)
{
  bfd *dynobj;
  asection *sdyn;
  struct riscv_elf_link_hash_table *htab;

  htab = riscv_elf_hash_table (info);
  BFD_ASSERT (htab != NULL);
  dynobj = htab->elf.dynobj;

  sdyn = bfd_get_linker_section (dynobj, ".dynamic");

  if (elf_hash_table (info)->dynamic_sections_created)
    {
      asection *splt;
      bfd_boolean ret;

      splt = htab->elf.splt;
      BFD_ASSERT (splt != NULL && sdyn != NULL);

      ret = riscv_finish_dyn (output_bfd, info, dynobj, sdyn);

      if (ret != TRUE)
	return ret;

      /* Fill in the head and tail entries in the procedure linkage table.  */
      if (splt->size > 0)
	{
	  int i;
	  uint32_t plt_header[PLT_HEADER_INSNS];
	  riscv_make_plt_header (sec_addr (htab->elf.sgotplt),
				 sec_addr (splt), plt_header);

	  for (i = 0; i < PLT_HEADER_INSNS; i++)
	    bfd_put_32 (output_bfd, plt_header[i], splt->contents + 4*i);
	}

      elf_section_data (splt->output_section)->this_hdr.sh_entsize
	= PLT_ENTRY_SIZE;
    }

  if (htab->elf.sgotplt)
    {
      asection *output_section = htab->elf.sgotplt->output_section;

      if (bfd_is_abs_section (output_section))
	{
	  (*_bfd_error_handler)
	    (_("discarded output section: `%A'"), htab->elf.sgotplt);
	  return FALSE;
	}

      if (htab->elf.sgotplt->size > 0)
	{
	  /* Write the first two entries in .got.plt, needed for the dynamic
	     linker.  */
	  bfd_put_64 (output_bfd, (bfd_vma) -1, htab->elf.sgotplt->contents);
	  bfd_put_64 (output_bfd, (bfd_vma) 0,
		      htab->elf.sgotplt->contents + GOT_ENTRY_SIZE);
	}

      elf_section_data (output_section)->this_hdr.sh_entsize = GOT_ENTRY_SIZE;
    }

  if (htab->elf.sgot)
    {
      asection *output_section = htab->elf.sgot->output_section;

      if (htab->elf.sgot->size > 0)
	{
	  /* Set the first entry in the global offset table to the address of
	     the dynamic section.  */
	  bfd_vma val = sdyn ? sec_addr (sdyn) : 0;
	  bfd_put_64 (output_bfd, val, htab->elf.sgot->contents);
	}

      elf_section_data (output_section)->this_hdr.sh_entsize = GOT_ENTRY_SIZE;
    }

  return TRUE;
}

/* Return address for Ith PLT stub in section PLT, for relocation REL
   or (bfd_vma) -1 if it should not be included.  */

static bfd_vma
riscv_elf_plt_sym_val (bfd_vma i, const asection *plt,
		       const arelent *rel ATTRIBUTE_UNUSED)
{
  return plt->vma + PLT_HEADER_SIZE + i * PLT_ENTRY_SIZE;
}

static enum elf_reloc_type_class
riscv_reloc_type_class (const struct bfd_link_info *info ATTRIBUTE_UNUSED,
			const asection *rel_sec ATTRIBUTE_UNUSED,
			const Elf_Internal_Rela *rela)
{
  switch (ELF64_R_TYPE (rela->r_info))
    {
    case R_RISCV_RELATIVE:
      return reloc_class_relative;
    case R_RISCV_JUMP_SLOT:
      return reloc_class_plt;
    case R_RISCV_COPY:
      return reloc_class_copy;
    default:
      return reloc_class_normal;
    }
}

/* Merge backend specific data from an object file to the output
   object file when linking.  */

static bfd_boolean
_bfd_riscv_elf_merge_private_bfd_data (bfd *ibfd, struct bfd_link_info *info)
{
  bfd *obfd = info->output_bfd;
  flagword new_flags = elf_elfheader (ibfd)->e_flags;
  flagword old_flags = elf_elfheader (obfd)->e_flags;

  if (!is_riscv_elf (ibfd) || !is_riscv_elf (obfd))
    return TRUE;

  if (strcmp (bfd_get_target (ibfd), bfd_get_target (obfd)) != 0)
    {
      (*_bfd_error_handler)
	(_("%B: ABI is incompatible with that of the selected emulation:\n"
	   "  target emulation `%s' does not match `%s'"),
	 ibfd, bfd_get_target (ibfd), bfd_get_target (obfd));
      return FALSE;
    }

  if (!_bfd_elf_merge_object_attributes (ibfd, info))
    return FALSE;

  if (! elf_flags_init (obfd))
    {
      elf_flags_init (obfd) = TRUE;
      elf_elfheader (obfd)->e_flags = new_flags;
      return TRUE;
    }

  /* Disallow linking different float ABIs.  */
  if ((old_flags ^ new_flags) & EF_RISCV_FLOAT_ABI)
    {
      (*_bfd_error_handler)
	(_("%B: can't link hard-float modules with soft-float modules"), ibfd);
      goto fail;
    }

  /* Allow linking RVC and non-RVC, and keep the RVC flag.  */
  elf_elfheader (obfd)->e_flags |= new_flags & EF_RISCV_RVC;

  return TRUE;

fail:
  bfd_set_error (bfd_error_bad_value);
  return FALSE;
}

/* Delete some bytes from a section while relaxing.  */

static bfd_boolean
riscv_relax_delete_bytes (bfd *abfd, asection *sec, bfd_vma addr, size_t count)
{
  unsigned int i, symcount;
  bfd_vma toaddr = sec->size;
  struct elf_link_hash_entry **sym_hashes = elf_sym_hashes (abfd);
  Elf_Internal_Shdr *symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  unsigned int sec_shndx = _bfd_elf_section_from_bfd_section (abfd, sec);
  struct bfd_elf_section_data *data = elf_section_data (sec);
  bfd_byte *contents = data->this_hdr.contents;

  /* Actually delete the bytes.  */
  sec->size -= count;
  memmove (contents + addr, contents + addr + count, toaddr - addr - count);

  /* Adjust the location of all of the relocs.  Note that we need not
     adjust the addends, since all PC-relative references must be against
     symbols, which we will adjust below.  */
  for (i = 0; i < sec->reloc_count; i++)
    if (data->relocs[i].r_offset > addr && data->relocs[i].r_offset < toaddr)
      data->relocs[i].r_offset -= count;

  /* Adjust the local symbols defined in this section.  */
  for (i = 0; i < symtab_hdr->sh_info; i++)
    {
      Elf_Internal_Sym *sym = (Elf_Internal_Sym *) symtab_hdr->contents + i;
      if (sym->st_shndx == sec_shndx)
	{
	  /* If the symbol is in the range of memory we just moved, we
	     have to adjust its value.  */
	  if (sym->st_value > addr && sym->st_value <= toaddr)
	    sym->st_value -= count;

	  /* If the symbol *spans* the bytes we just deleted (i.e. its
	     *end* is in the moved bytes but its *start* isn't), then we
	     must adjust its size.  */
	  if (sym->st_value <= addr
	      && sym->st_value + sym->st_size > addr
	      && sym->st_value + sym->st_size <= toaddr)
	    sym->st_size -= count;
	}
    }

  /* Now adjust the global symbols defined in this section.  */
  symcount = ((symtab_hdr->sh_size / sizeof (Elf64_External_Sym))
	      - symtab_hdr->sh_info);

  for (i = 0; i < symcount; i++)
    {
      struct elf_link_hash_entry *sym_hash = sym_hashes[i];

      if ((sym_hash->root.type == bfd_link_hash_defined
	   || sym_hash->root.type == bfd_link_hash_defweak)
	  && sym_hash->root.u.def.section == sec)
	{
	  /* As above, adjust the value if needed.  */
	  if (sym_hash->root.u.def.value > addr
	      && sym_hash->root.u.def.value <= toaddr)
	    sym_hash->root.u.def.value -= count;

	  /* As above, adjust the size if needed.  */
	  if (sym_hash->root.u.def.value <= addr
	      && sym_hash->root.u.def.value + sym_hash->size > addr
	      && sym_hash->root.u.def.value + sym_hash->size <= toaddr)
	    sym_hash->size -= count;
	}
    }

  return TRUE;
}

typedef bfd_boolean (*relax_func_t) (bfd *, asection *, asection *,
				     struct bfd_link_info *,
				     Elf_Internal_Rela *,
				     bfd_vma, bfd_vma, bfd_vma, bfd_boolean, bfd_boolean *);

/* Relax AUIPC + JALR into JAL.  */

static bfd_boolean
_bfd_riscv_relax_call (bfd *abfd, asection *sec, asection *sym_sec,
		       struct bfd_link_info *link_info,
		       Elf_Internal_Rela *rel,
		       bfd_vma symval,
		       bfd_vma max_alignment,
		       bfd_vma reserve_size ATTRIBUTE_UNUSED,
		       bfd_boolean is_import,
		       bfd_boolean *again)
{
  bfd_byte *contents = elf_section_data (sec)->this_hdr.contents;
  bfd_signed_vma foff = symval - (sec_addr (sec) + rel->r_offset);
  bfd_boolean near_zero = (symval + RISCV_IMM_REACH/2) < RISCV_IMM_REACH;
  bfd_vma auipc, jalr;
  int rd, r_type, len = 4, rvc = elf_elfheader (abfd)->e_flags & EF_RISCV_RVC;
  static bfd_boolean Mem20Range = TRUE;

  /* If the call crosses section boundaries, an alignment directive could
     cause the PC-relative offset to later increase.  */
  if (VALID_UJTYPE_IMM (foff) && sym_sec->output_section != sec->output_section)
    foff += (foff < 0 ? -max_alignment : max_alignment);

  /* See if this function call can be shortened.  */
  if (!VALID_UJTYPE_IMM (foff) && !(!bfd_link_pic (link_info) && near_zero))
  if ((is_import&&!Mem20Range) || (!VALID_UJTYPE_IMM (foff) && !(!bfd_link_pic (link_info) && near_zero)))
    return TRUE;

  /* Shorten the function call.  */
  BFD_ASSERT (rel->r_offset + 8 <= sec->size);

  auipc = bfd_get_32 (abfd, contents + rel->r_offset);
  jalr = bfd_get_32 (abfd, contents + rel->r_offset + 4);
  rd = (jalr >> OP_SH_RD) & OP_MASK_RD;
  rvc = rvc && VALID_RVC_J_IMM (foff) && ARCH_SIZE == 32;
  rvc = rvc &&!is_import;

  if (rvc && (rd == 0 || rd == X_RA))
    {
      /* Relax to C.J[AL] rd, addr.  */
      r_type = R_RISCV_RVC_JUMP;
      auipc = rd == 0 ? MATCH_C_J : MATCH_C_JAL;
      len = 2;
    }
  else if (VALID_UJTYPE_IMM (foff) || is_import)
    {
      /* Relax to JAL rd, addr.  */
      r_type = R_RISCV_JAL;
      auipc = MATCH_JAL | (rd << OP_SH_RD);
    }
  else /* near_zero */
    {
      /* Relax to JALR rd, x0, addr.  */
      r_type = R_RISCV_LO12_I;
      auipc = MATCH_JALR | (rd << OP_SH_RD);
    }

  /* Replace the R_RISCV_CALL reloc.  */
  rel->r_info = ELF64_R_INFO (ELF64_R_SYM (rel->r_info), r_type);
  /* Replace the AUIPC.  */
  bfd_put (8 * len, abfd, auipc, contents + rel->r_offset);

  /* Delete unnecessary JALR.  */
  *again = TRUE;
  return riscv_relax_delete_bytes (abfd, sec, rel->r_offset + len, 8 - len);
}

/* Traverse all output sections and return the max alignment.  */

static bfd_vma
_bfd_riscv_get_max_alignment (asection *sec)
{
  unsigned int max_alignment_power = 0;
  asection *o;

  for (o = sec->output_section->owner->sections; o != NULL; o = o->next)
    {
      if (o->alignment_power > max_alignment_power)
	max_alignment_power = o->alignment_power;
    }

  return (bfd_vma) 1 << max_alignment_power;
}

/* Relax non-PIC global variable references.  */

static bfd_boolean
_bfd_riscv_relax_lui (bfd *abfd,
		      asection *sec,
		      asection *sym_sec,
		      struct bfd_link_info *link_info,
		      Elf_Internal_Rela *rel,
		      bfd_vma symval,
		      bfd_vma max_alignment,
		      bfd_vma reserve_size,
		      bfd_boolean is_import,
		      bfd_boolean *again)
{
  bfd_byte *contents = elf_section_data (sec)->this_hdr.contents;
  bfd_vma gp = riscv_global_pointer_value (link_info);
  int use_rvc = elf_elfheader (abfd)->e_flags & EF_RISCV_RVC;

  /* Mergeable symbols and code might later move out of range.  */
  if (is_import || (sym_sec->flags & (SEC_MERGE | SEC_CODE)))
    return TRUE;

  BFD_ASSERT (rel->r_offset + 4 <= sec->size);

  if (gp)
    {
      /* If gp and the symbol are in the same output section, then
	 consider only that section's alignment.  */
      struct bfd_link_hash_entry *h =
	bfd_link_hash_lookup (link_info->hash, RISCV_GP_SYMBOL, FALSE, FALSE,
			      TRUE);
      if (h->u.def.section->output_section == sym_sec->output_section)
	max_alignment = (bfd_vma) 1 << sym_sec->output_section->alignment_power;
    }

  /* Is the reference in range of x0 or gp?
     Valid gp range conservatively because of alignment issue.  */
  if (VALID_ITYPE_IMM (symval)
      || (symval >= gp
	  && VALID_ITYPE_IMM (symval - gp + max_alignment + reserve_size))
      || (symval < gp
	  && VALID_ITYPE_IMM (symval - gp - max_alignment - reserve_size)))
    {
      unsigned sym = ELF64_R_SYM (rel->r_info);
      switch (ELF64_R_TYPE (rel->r_info))
	{
	case R_RISCV_LO12_I:
	  rel->r_info = ELF64_R_INFO (sym, R_RISCV_GPREL_I);
	  return TRUE;

	case R_RISCV_LO12_S:
	  rel->r_info = ELF64_R_INFO (sym, R_RISCV_GPREL_S);
	  return TRUE;

	case R_RISCV_HI20:
	  /* We can delete the unnecessary LUI and reloc.  */
	  rel->r_info = ELF64_R_INFO (0, R_RISCV_NONE);
	  *again = TRUE;
	  return riscv_relax_delete_bytes (abfd, sec, rel->r_offset, 4);

	default:
	  abort ();
	}
    }

  /* Can we relax LUI to C.LUI?  Alignment might move the section forward;
     account for this assuming page alignment at worst.  */
  if (use_rvc
      && ELF64_R_TYPE (rel->r_info) == R_RISCV_HI20
      && VALID_RVC_LUI_IMM (RISCV_CONST_HIGH_PART (symval))
      && VALID_RVC_LUI_IMM (RISCV_CONST_HIGH_PART (symval + ELF_MAXPAGESIZE)))
    {
      /* Replace LUI with C.LUI if legal (i.e., rd != x2/sp).  */
      bfd_vma lui = bfd_get_32 (abfd, contents + rel->r_offset);
      if (((lui >> OP_SH_RD) & OP_MASK_RD) == X_SP)
	return TRUE;

      lui = (lui & (OP_MASK_RD << OP_SH_RD)) | MATCH_C_LUI;
      bfd_put_32 (abfd, lui, contents + rel->r_offset);

      /* Replace the R_RISCV_HI20 reloc.  */
      rel->r_info = ELF64_R_INFO (ELF64_R_SYM (rel->r_info), R_RISCV_RVC_LUI);

      *again = TRUE;
      return riscv_relax_delete_bytes (abfd, sec, rel->r_offset + 2, 2);
    }

  return TRUE;
}

/* Relax non-PIC TLS references.  */

static bfd_boolean
_bfd_riscv_relax_tls_le (bfd *abfd,
			 asection *sec,
			 asection *sym_sec ATTRIBUTE_UNUSED,
			 struct bfd_link_info *link_info,
			 Elf_Internal_Rela *rel,
			 bfd_vma symval,
			 bfd_vma max_alignment ATTRIBUTE_UNUSED,
			 bfd_vma reserve_size ATTRIBUTE_UNUSED,
			 bfd_boolean is_import,
			 bfd_boolean *again)
{
  /* See if this symbol is in range of tp.  */
  if (RISCV_CONST_HIGH_PART (tpoff (link_info, symval)) != 0 || is_import)
    return TRUE;

  BFD_ASSERT (rel->r_offset + 4 <= sec->size);
  switch (ELF64_R_TYPE (rel->r_info))
    {
    case R_RISCV_TPREL_LO12_I:
      rel->r_info = ELF64_R_INFO (ELF64_R_SYM (rel->r_info), R_RISCV_TPREL_I);
      return TRUE;

    case R_RISCV_TPREL_LO12_S:
      rel->r_info = ELF64_R_INFO (ELF64_R_SYM (rel->r_info), R_RISCV_TPREL_S);
      return TRUE;

    case R_RISCV_TPREL_HI20:
    case R_RISCV_TPREL_ADD:
      /* We can delete the unnecessary instruction and reloc.  */
      rel->r_info = ELF64_R_INFO (0, R_RISCV_NONE);
      *again = TRUE;
      return riscv_relax_delete_bytes (abfd, sec, rel->r_offset, 4);

    default:
      abort ();
    }
}

/* Implement R_RISCV_ALIGN by deleting excess alignment NOPs.  */

static bfd_boolean
_bfd_riscv_relax_align (bfd *abfd, asection *sec,
			asection *sym_sec ATTRIBUTE_UNUSED,
			struct bfd_link_info *link_info ATTRIBUTE_UNUSED,
			Elf_Internal_Rela *rel,
			bfd_vma symval,
			bfd_vma max_alignment ATTRIBUTE_UNUSED,
			bfd_vma reserve_size ATTRIBUTE_UNUSED,
			bfd_boolean is_import ATTRIBUTE_UNUSED,
			bfd_boolean *again ATTRIBUTE_UNUSED)
{
  bfd_byte *contents = elf_section_data (sec)->this_hdr.contents;
  bfd_vma alignment = 1, pos;
  while (alignment <= rel->r_addend)
    alignment *= 2;

  symval -= rel->r_addend;
  bfd_vma aligned_addr = ((symval - 1) & ~(alignment - 1)) + alignment;
  bfd_vma nop_bytes = aligned_addr - symval;

  /* Once we've handled an R_RISCV_ALIGN, we can't relax anything else.  */
  sec->sec_flg0 = TRUE;

  /* Make sure there are enough NOPs to actually achieve the alignment.  */
  if (rel->r_addend < nop_bytes)
    return FALSE;

  /* Delete the reloc.  */
  rel->r_info = ELF64_R_INFO (0, R_RISCV_NONE);

  /* If the number of NOPs is already correct, there's nothing to do.  */
  if (nop_bytes == rel->r_addend)
    return TRUE;

  /* Write as many RISC-V NOPs as we need.  */
  for (pos = 0; pos < (nop_bytes & -4); pos += 4)
    bfd_put_32 (abfd, RISCV_NOP, contents + rel->r_offset + pos);

  /* Write a final RVC NOP if need be.  */
  if (nop_bytes % 4 != 0)
    bfd_put_16 (abfd, RVC_NOP, contents + rel->r_offset + pos);

  /* Delete the excess bytes.  */
  return riscv_relax_delete_bytes (abfd, sec, rel->r_offset + nop_bytes,
				   rel->r_addend - nop_bytes);
}


static bfd_boolean
_bfd_riscv_relax_import_pcrel (bfd *abfd, asection *sec,
                        asection *sym_sec ATTRIBUTE_UNUSED,
                        struct bfd_link_info *link_info ATTRIBUTE_UNUSED,
                        Elf_Internal_Rela *rel,
                        bfd_vma symval ATTRIBUTE_UNUSED,
			bfd_vma max_alignment ATTRIBUTE_UNUSED,
			bfd_vma reserve_size ATTRIBUTE_UNUSED,
                        bfd_boolean is_import,
                        bfd_boolean *again)

{
  bfd_byte *contents = elf_section_data (sec)->this_hdr.contents;

  if (is_import) {
        unsigned sym = ELF64_R_SYM (rel->r_info);
        switch (ELF64_R_TYPE (rel->r_info)) {
                case R_RISCV_PCREL_LO12_I:
                        rel->r_info = ELF64_R_INFO (sym, R_RISCV_LO12_I);
                        return TRUE;
                case R_RISCV_PCREL_HI20:
                        {
                                bfd_vma lui = bfd_get_32 (abfd, contents + rel->r_offset);
                                lui = (lui & (OP_MASK_RD << OP_SH_RD)) | MATCH_LUI;
                                bfd_put_32 (abfd, lui, contents + rel->r_offset);
                                rel->r_info = ELF64_R_INFO (sym, R_RISCV_HI20);
                                /*
                                        PCREL_HI20 is always followed by a reloc on the lsp part of the symbol, we use
                                        this assumption to force the reloc to pseudo absolute
                                */
                                (rel+1)->r_info = ELF64_R_INFO(sym, R_RISCV_LO12_I);
                        }
                        return TRUE;
        }
  }
  *again = FALSE;
  return TRUE;
}

static bfd_boolean
_bfd_riscv_relax_got_ref (bfd *abfd, asection *sec,
                        asection *sym_sec ATTRIBUTE_UNUSED,
                        struct bfd_link_info *link_info ATTRIBUTE_UNUSED,
                        Elf_Internal_Rela *rel,
                        bfd_vma symval ATTRIBUTE_UNUSED,
                        bfd_vma max_alignment ATTRIBUTE_UNUSED,
                        bfd_vma reserve_size ATTRIBUTE_UNUSED,
                        bfd_boolean is_import ATTRIBUTE_UNUSED,
                        bfd_boolean *again ATTRIBUTE_UNUSED)

{
  bfd_byte *contents = elf_section_data (sec)->this_hdr.contents;
  unsigned sym = ELF64_R_SYM (rel->r_info);
  Elf_Internal_Rela *low_part_rel = rel + 1;
  bfd_vma low_part_ref;

  rel->r_info = ELF64_R_INFO (sym, R_RISCV_PCREL_HI20);
  /* Force second part of the access to be an addi instead of the usual load got */
  low_part_ref = bfd_get_32 (abfd, contents + low_part_rel->r_offset);
  low_part_ref = (low_part_ref & ((OP_MASK_RD << OP_SH_RD) | (OP_MASK_RS1 << OP_SH_RS1))) | MATCH_ADDI;
  bfd_put_32 (abfd, low_part_ref, contents + low_part_rel->r_offset);

  sym = ELF64_R_SYM(low_part_rel->r_info);
  low_part_rel->r_info = ELF64_R_INFO (sym, R_RISCV_PCREL_LO12_I);

  return TRUE;
}

/* Relax a section.  Pass 0 shortens code sequences unless disabled.
   Pass 1, which cannot be disabled, handles code alignment directives.  */

static bfd_boolean
_bfd_riscv_relax_section (bfd *abfd, asection *sec,
			  struct bfd_link_info *info,
			  bfd_boolean *again)
{
  Elf_Internal_Shdr *symtab_hdr = &elf_symtab_hdr (abfd);
  struct riscv_elf_link_hash_table *htab = riscv_elf_hash_table (info);
  struct bfd_elf_section_data *data = elf_section_data (sec);
  Elf_Internal_Rela *relocs;
  bfd_boolean ret = FALSE;
  unsigned int i;
  bfd_vma max_alignment, reserve_size = 0;

  *again = FALSE;

  if (bfd_link_relocatable (info)
      || sec->sec_flg0
      || (sec->flags & SEC_RELOC) == 0
      || sec->reloc_count == 0
      || (info->disable_target_specific_optimizations
	  && info->relax_pass == 0))
    return TRUE;

  /* Read this BFD's relocs if we haven't done so already.  */
  if (data->relocs)
    relocs = data->relocs;
  else if (!(relocs = _bfd_elf_link_read_relocs (abfd, sec, NULL, NULL,
						 info->keep_memory)))
    goto fail;

  max_alignment = _bfd_riscv_get_max_alignment (sec);

  /* Examine and consider relaxing each reloc.  */
  for (i = 0; i < sec->reloc_count; i++)
    {
      asection *sym_sec;
      Elf_Internal_Rela *rel = relocs + i;
      relax_func_t relax_func;
      int type = ELF64_R_TYPE (rel->r_info);
      bfd_vma symval;
      bfd_boolean Is_Import = FALSE;

      if (info->relax_pass == 0)
	{
	  if (type == R_RISCV_CALL || type == R_RISCV_CALL_PLT)
	    relax_func = _bfd_riscv_relax_call;
	  else if (type == R_RISCV_HI20
		   || type == R_RISCV_LO12_I
		   || type == R_RISCV_LO12_S)
	    relax_func = _bfd_riscv_relax_lui;
	  else if (type == R_RISCV_TPREL_HI20
		   || type == R_RISCV_TPREL_ADD
		   || type == R_RISCV_TPREL_LO12_I
		   || type == R_RISCV_TPREL_LO12_S)
	    relax_func = _bfd_riscv_relax_tls_le;
          else if (type == R_RISCV_PCREL_HI20 || type == R_RISCV_PCREL_LO12_I)
            relax_func = _bfd_riscv_relax_import_pcrel;
          else if (ComponentMode && (type == R_RISCV_GOT_HI20))
            relax_func = _bfd_riscv_relax_got_ref;
	  else
	    continue;

	  /* Only relax this reloc if it is paired with R_RISCV_RELAX.  */
	  if (i == sec->reloc_count - 1
	      || ELF64_R_TYPE ((rel + 1)->r_info) != R_RISCV_RELAX
	      || rel->r_offset != (rel + 1)->r_offset)
	    continue;

	  /* Skip over the R_RISCV_RELAX.  */
	  i++;
	}
      else if (type == R_RISCV_ALIGN)
	relax_func = _bfd_riscv_relax_align;
      else
	continue;

      data->relocs = relocs;

      /* Read this BFD's contents if we haven't done so already.  */
      if (!data->this_hdr.contents
	  && !bfd_malloc_and_get_section (abfd, sec, &data->this_hdr.contents))
	goto fail;

      /* Read this BFD's symbols if we haven't done so already.  */
      if (symtab_hdr->sh_info != 0
	  && !symtab_hdr->contents
	  && !(symtab_hdr->contents =
	       (unsigned char *) bfd_elf_get_elf_syms (abfd, symtab_hdr,
						       symtab_hdr->sh_info,
						       0, NULL, NULL, NULL)))
	goto fail;

      /* Get the value of the symbol referred to by the reloc.  */
      if (ELF64_R_SYM (rel->r_info) < symtab_hdr->sh_info)
	{
	  /* A local symbol.  */
	  Elf_Internal_Sym *isym = ((Elf_Internal_Sym *) symtab_hdr->contents
				    + ELF64_R_SYM (rel->r_info));
	  reserve_size = (isym->st_size - rel->r_addend) > isym->st_size
	    ? 0 : isym->st_size - rel->r_addend;

	  if (isym->st_shndx == SHN_UNDEF)
	    sym_sec = sec, symval = sec_addr (sec) + rel->r_offset;
	  else
	    {
	      BFD_ASSERT (isym->st_shndx < elf_numsections (abfd));
	      sym_sec = elf_elfsections (abfd)[isym->st_shndx]->bfd_section;
	      if (sec_addr (sym_sec) == 0)
		continue;
	      symval = sec_addr (sym_sec) + isym->st_value;
	    }
	}
      else
	{
	  unsigned long indx;
	  struct elf_link_hash_entry *h;

	  indx = ELF64_R_SYM (rel->r_info) - symtab_hdr->sh_info;
	  h = elf_sym_hashes (abfd)[indx];

	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;

	  if (h->plt.offset != MINUS_ONE)
	    symval = sec_addr (htab->elf.splt) + h->plt.offset;
	  else if (h->root.u.def.section->output_section == NULL
		   || (h->root.type != bfd_link_hash_defined
		       && h->root.type != bfd_link_hash_defweak))
	    continue;
	  else
	    symval = sec_addr (h->root.u.def.section) + h->root.u.def.value;

	  if (h->type != STT_FUNC)
	    reserve_size =
	      (h->size - rel->r_addend) > h->size ? 0 : h->size - rel->r_addend;
	  sym_sec = h->root.u.def.section;
          if (h->root.type == bfd_link_hash_defweak && strcmp(sec->name, "pulp.import")) Is_Import = TRUE;

	}

      symval += rel->r_addend;

      if (!relax_func (abfd, sec, sym_sec, info, rel, symval,
		       max_alignment, reserve_size, Is_Import, again))
	goto fail;
    }

  ret = TRUE;

fail:
  if (relocs != data->relocs)
    free (relocs);

  return ret;
}

#if ARCH_SIZE == 32
# define PRSTATUS_SIZE			0 /* FIXME */
# define PRSTATUS_OFFSET_PR_CURSIG	12
# define PRSTATUS_OFFSET_PR_PID		24
# define PRSTATUS_OFFSET_PR_REG		72
# define ELF_GREGSET_T_SIZE		128
# define PRPSINFO_SIZE			128
# define PRPSINFO_OFFSET_PR_PID		16
# define PRPSINFO_OFFSET_PR_FNAME	32
# define PRPSINFO_OFFSET_PR_PSARGS	48
#else
# define PRSTATUS_SIZE			376
# define PRSTATUS_OFFSET_PR_CURSIG	12
# define PRSTATUS_OFFSET_PR_PID		32
# define PRSTATUS_OFFSET_PR_REG		112
# define ELF_GREGSET_T_SIZE		256
# define PRPSINFO_SIZE			136
# define PRPSINFO_OFFSET_PR_PID		24
# define PRPSINFO_OFFSET_PR_FNAME	40
# define PRPSINFO_OFFSET_PR_PSARGS	56
#endif

/* Support for core dump NOTE sections.  */

static bfd_boolean
riscv_elf_grok_prstatus (bfd *abfd, Elf_Internal_Note *note)
{
  switch (note->descsz)
    {
      default:
	return FALSE;

      case PRSTATUS_SIZE:  /* sizeof(struct elf_prstatus) on Linux/RISC-V.  */
	/* pr_cursig */
	elf_tdata (abfd)->core->signal
	  = bfd_get_16 (abfd, note->descdata + PRSTATUS_OFFSET_PR_CURSIG);

	/* pr_pid */
	elf_tdata (abfd)->core->lwpid
	  = bfd_get_32 (abfd, note->descdata + PRSTATUS_OFFSET_PR_PID);
	break;
    }

  /* Make a ".reg/999" section.  */
  return _bfd_elfcore_make_pseudosection (abfd, ".reg", ELF_GREGSET_T_SIZE,
					  note->descpos + PRSTATUS_OFFSET_PR_REG);
}

static bfd_boolean
riscv_elf_grok_psinfo (bfd *abfd, Elf_Internal_Note *note)
{
  switch (note->descsz)
    {
      default:
	return FALSE;

      case PRPSINFO_SIZE: /* sizeof(struct elf_prpsinfo) on Linux/RISC-V.  */
	/* pr_pid */
	elf_tdata (abfd)->core->pid
	  = bfd_get_32 (abfd, note->descdata + PRPSINFO_OFFSET_PR_PID);

	/* pr_fname */
	elf_tdata (abfd)->core->program = _bfd_elfcore_strndup
	  (abfd, note->descdata + PRPSINFO_OFFSET_PR_FNAME, 16);

	/* pr_psargs */
	elf_tdata (abfd)->core->command = _bfd_elfcore_strndup
	  (abfd, note->descdata + PRPSINFO_OFFSET_PR_PSARGS, 80);
	break;
    }

  /* Note that for some reason, a spurious space is tacked
     onto the end of the args in some (at least one anyway)
     implementations, so strip it off if it exists.  */

  {
    char *command = elf_tdata (abfd)->core->command;
    int n = strlen (command);

    if (0 < n && command[n - 1] == ' ')
      command[n - 1] = '\0';
  }

  return TRUE;
}

/* Set the right mach type.  */
static bfd_boolean
riscv_elf_object_p (bfd *abfd)
{
  /* There are only two mach types in RISCV currently.  */
  if (strcmp (abfd->xvec->name, "elf32-littleriscv") == 0)
    bfd_default_set_arch_mach (abfd, bfd_arch_riscv, bfd_mach_riscv32);
  else
    bfd_default_set_arch_mach (abfd, bfd_arch_riscv, bfd_mach_riscv64);

  return TRUE;
}

bfd_boolean
_bfd_riscv_elf64_final_link (bfd *abfd, struct bfd_link_info *info)

{
	struct bfd_section *s;
	unsigned int SecNameSize, SecRelocSize, NImport=0, ExportSize;
	unsigned int *NameSection, *RelocSection, *ExportSection;
	static int Trace = 0;


	if (!bfd_elf_final_link (abfd, info)) return FALSE;

	if (PulpImportCreateNameAndRelocSections(0,
						 &NameSection,  &SecNameSize,
						 &RelocSection, &SecRelocSize, &NImport) == FALSE) {
      		(*_bfd_error_handler)(_("Failed to create Import sections"));
		return FALSE;
	}
	if (Trace) printf("NImport: %d, SecNameSize: %d, SecRelocSize: %d\n", NImport, SecNameSize, SecRelocSize);

	if (NImport) {
		struct bfd_section *TextSec = NULL;
		unsigned int BaseText = 0;

		if (ComponentMode == 0) {
			TextSec = bfd_get_section_by_name (info->output_bfd, ".text");
			if (TextSec) {
				BaseText = (unsigned int) TextSec->lma;
				AdjustRelocsImport(RelocSection, BaseText);
			} else {
      				(*_bfd_error_handler)(_("Failed to find .text section in output_bfd"));
			}
		}

		if (DumpImportExportSections==1 || DumpImportExportSections==3)
			DiassembleImports(NameSection, SecNameSize, RelocSection, SecRelocSize, BaseText);
		if (DumpImportExportSections==2 || DumpImportExportSections==3) {
			DumpCEquiv((unsigned int *) NameSection, SecNameSize, 4, ComponentMode?"CompImportNames":"ResiImportNames");
			DumpCEquiv(RelocSection, SecRelocSize, 4, ComponentMode?"CompImportRelocs":"ResiImportRelocs");
		}

		s = bfd_get_section_by_name (abfd, ".pulp.import.names");
		if (s) {
			s->contents = xmalloc(SecNameSize);
			s->size = SecNameSize;
			if (! bfd_set_section_contents (abfd, s, (char *) NameSection, 0, SecNameSize)) {
      				(*_bfd_error_handler)(_(".pulp.import.names: Failed to set content"));
	        		return FALSE;
			} else if (Trace) {
				fprintf(stderr, ".pulp.import.names: Set content OK\n");
			}
		} else {
      			(*_bfd_error_handler)(_("Can't find .pulp.import.names"));
	        	return FALSE;
		}
		s = bfd_get_section_by_name (abfd, ".pulp.import.relocs");
		if (s) {
			s->contents = xmalloc(SecRelocSize);
			s->size = SecRelocSize;
			if (! bfd_set_section_contents (abfd, s, (char *) RelocSection, 0, SecRelocSize)) {
      				(*_bfd_error_handler)(_(".pulp.import.relocs: Failed to set content"));
	        		return FALSE;
			} else if (Trace) {
				fprintf(stderr, ".pulp.import.relocs: Set content OK\n");
			}
		} else {
      			(*_bfd_error_handler)(_("Can't find .pulp.import.relocs"));
	        	return FALSE;
		}
		(void) ReleaseImportEntry();
	} else {
		/* In this case both sections are empty with size = 4 for the section descriptor
		   descriptor itself is 0 */
		s = bfd_get_section_by_name (abfd, ".pulp.import.names");
		if (s) {
			unsigned int Empty = 0;
			SecNameSize = 4;
			s->contents = xmalloc(SecNameSize);
			s->size = SecNameSize;
			if (! bfd_set_section_contents (abfd, s, (char *) &Empty, 0, SecNameSize)) {
      				(*_bfd_error_handler)(_(".pulp.import.names: Failed to set content"));
	        		return FALSE;
			} else if (Trace) {
				fprintf(stderr, ".pulp.import.names: Set content OK\n");
			}
		}
		s = bfd_get_section_by_name (abfd, ".pulp.import.relocs");
		if (s) {
			unsigned int Empty = 0;
			SecRelocSize = 4;
			s->contents = xmalloc(SecRelocSize);
			s->size = SecRelocSize;
			if (! bfd_set_section_contents (abfd, s, (char *) &Empty, 0, SecRelocSize)) {
      				(*_bfd_error_handler)(_(".pulp.import.relocs: Failed to set content"));
	        		return FALSE;
			} else if (Trace) {
				fprintf(stderr, ".pulp.import.relocs: Set content OK\n");
			}
		}
	}

	if (ComponentMode) {
		if (ComponentEntryProvided == FALSE)
			(*_bfd_error_handler)(_("No Entry provided for Component"));
		else if (ExportLookup(ComponentEntry.name) == FALSE)
			 (*_bfd_error_handler)(_("Component provided entry: %s not found in component export list"), ComponentEntry.name);
	}
	if (PulpExportCreateSection(&ExportSection, &ExportSize, info) == FALSE) {
      		(*_bfd_error_handler)(_("Failed to create Export Section"));
		return FALSE;
	} else if (ExportSection) {
		if (DumpImportExportSections==1 || DumpImportExportSections==3)
			DiassembleExports(ExportSection, ExportSize);
		if (DumpImportExportSections==1 || DumpImportExportSections==2) {
			DumpCEquiv(ExportSection, ExportSize, 4, ComponentMode?"CompExports":"ResiExports");
			if (ComponentMode) {
				struct bfd_section *CompSec = NULL;
				CompSec = bfd_get_section_by_name (info->output_bfd, ".component.body");
				if (CompSec) {
					long Size = CompSec->size;
					char *Buffer = xmalloc (Size);
					bfd_get_section_contents (info->output_bfd, CompSec, Buffer, 0, Size);
					DumpCEquiv((unsigned int *) Buffer, Size, 1, "ComponentBody");
					free(Buffer);
				}
			}
		}
		s = bfd_get_section_by_name (abfd, ".pulp.export");
		if (s) {
			s->contents = xmalloc(ExportSize);
			s->size = ExportSize;
			if (! bfd_set_section_contents (abfd, s, (char *) ExportSection, 0, ExportSize)) {
      				(*_bfd_error_handler)(_(".pulp.export: Failed to set content"));
	        		return FALSE;
			} else if (Trace) {
				fprintf(stderr, ".pulp.export: Set content OK\n");
			}
		} else {
      			(*_bfd_error_handler)(_("Can't find .pulp.export"));
	        	return FALSE;
		}
		(void) ReleaseExportEntry();
	} else if (ComponentMode) {
		/* We should have at least on export to be able to enter the component */
		(*_bfd_error_handler)(_("Component has empty export section"));
	}

	return TRUE;
}
#define TARGET_LITTLE_SYM		riscv_elf64_vec
#define TARGET_LITTLE_NAME		"elf64-littleriscv"

#define elf_backend_reloc_type_class	     riscv_reloc_type_class

#define bfd_elf64_bfd_reloc_name_lookup	     riscv_reloc_name_lookup
#define bfd_elf64_bfd_link_hash_table_create riscv_elf_link_hash_table_create
#define bfd_elf64_bfd_reloc_type_lookup	     riscv_reloc_type_lookup
#define bfd_elf64_bfd_merge_private_bfd_data \
  _bfd_riscv_elf_merge_private_bfd_data

#define elf_backend_copy_indirect_symbol     riscv_elf_copy_indirect_symbol
#define elf_backend_create_dynamic_sections  riscv_elf_create_dynamic_sections
#define elf_backend_check_relocs	     riscv_elf_check_relocs
#define elf_backend_adjust_dynamic_symbol    riscv_elf_adjust_dynamic_symbol
#define elf_backend_size_dynamic_sections    riscv_elf_size_dynamic_sections
#define elf_backend_relocate_section	     riscv_elf_relocate_section
#define elf_backend_finish_dynamic_symbol    riscv_elf_finish_dynamic_symbol
#define elf_backend_finish_dynamic_sections  riscv_elf_finish_dynamic_sections
#define elf_backend_gc_mark_hook	     riscv_elf_gc_mark_hook
#define elf_backend_gc_sweep_hook	     riscv_elf_gc_sweep_hook
#define elf_backend_plt_sym_val		     riscv_elf_plt_sym_val
#define elf_backend_grok_prstatus            riscv_elf_grok_prstatus
#define elf_backend_grok_psinfo              riscv_elf_grok_psinfo
#define elf_backend_object_p                 riscv_elf_object_p
#define elf_info_to_howto_rel		     NULL
#define elf_info_to_howto		     riscv_info_to_howto_rela
#define bfd_elf64_bfd_relax_section	     _bfd_riscv_relax_section

#define elf_backend_init_index_section	     _bfd_elf_init_1_index_section
#define bfd_elf64_bfd_final_link             _bfd_riscv_elf64_final_link

#define elf_backend_can_gc_sections	1
#define elf_backend_can_refcount	1
#define elf_backend_want_got_plt	1
#define elf_backend_plt_readonly	1
#define elf_backend_plt_alignment	4
#define elf_backend_want_plt_sym	1
#define elf_backend_got_header_size	(ARCH_SIZE / 8)
#define elf_backend_want_dynrelro	1
#define elf_backend_rela_normal		1
#define elf_backend_default_execstack	0

#include "elf64-target.h"
