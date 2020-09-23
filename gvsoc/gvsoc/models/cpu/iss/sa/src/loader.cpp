/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "sa_iss.hpp"

static int look_for_symbol_group(bfd *abfd, const char *sec_name, unsigned int Flag,
                          const char *s1, unsigned int *v1,
                          const char *s2, unsigned int *v2,
                          const char *s3, unsigned int *v3,
                          const char *s4, unsigned int *v4)

{
        int storage_needed;
        static asymbol **symbol_table = NULL;
        static int init_done = 0;
        static int number_of_symbols = 0;
        int i;
        int found_v1 = 0, found_v2 = 0, found_v3 = 0, found_v4 = 0;

        if (!init_done) {
                init_done = 1;
                storage_needed = bfd_get_symtab_upper_bound (abfd);
                if (storage_needed <= 0) return 0;

                symbol_table = (asymbol **) malloc (storage_needed);
    if (symbol_table == NULL) return 0;

                number_of_symbols = bfd_canonicalize_symtab (abfd, symbol_table);
                if (number_of_symbols < 0) return 0;
        }
        for (i = 0; i < number_of_symbols; i++) {
                asymbol *sym = symbol_table[i];
                if        (s1 && !found_v1 && (strcmp(sym->name, s1) == 0) && (sym->flags & Flag) && (strcmp(sym->section->name, sec_name) == 0)) {
                        *v1 = sym->value + sym->section->vma; found_v1 = 1;
                } else if (s2 && !found_v2 && (strcmp(sym->name, s2) == 0) && (sym->flags & Flag) && (strcmp(sym->section->name, sec_name) == 0)) {
                        *v2 = sym->value + sym->section->vma; found_v2 = 1;
                } else if (s3 && !found_v3 && (strcmp(sym->name, s3) == 0) && (sym->flags & Flag) && (strcmp(sym->section->name, sec_name) == 0)) {
                        *v3 = sym->value + sym->section->vma; found_v3 = 1;
                } else if (s4 && !found_v4 && (strcmp(sym->name, s4) == 0) && (sym->flags & Flag) && (strcmp(sym->section->name, sec_name) == 0)) {
                        *v4 = sym->value + sym->section->vma; found_v4 = 1;
                }
        }
        return ((!s1 || (s1 && found_v1)) && (!s2 || (s1 && found_v2)) && (!s3 || (s1 && found_v3)) && (!s4 || (s1 && found_v4)));
}

static int handle_argc_argc(iss_t *iss, struct bfd *abfd, char **prog_argv)

{
  unsigned int a_argc, a_argv, a_argbuf, a_stack, a_base, a_size;
  char *name = bfd_get_filename (abfd);
  int len = strlen (name) + 1;
  bfd_byte buf[4];
  int my_argc = 0;
  int Ok = 1;
  int i;
  static int Trace = 0;

  if (look_for_symbol_group(abfd, ".data", BSF_GLOBAL, "argc", &a_argc, "argv", &a_argv, "argbuf", &a_argbuf, "stack", &a_stack)) {

    iss->a_argc = a_argc;
    iss->a_argv = a_argv;
    iss->a_argbuf = a_argbuf;
    iss->a_stack = a_stack;

    for (i = 0; prog_argv[i] != NULL; my_argc++, i++) len += strlen (prog_argv[i]) + 1;

    if (Trace) {
      fprintf (stderr,  "Found argc=0x%X [%d], argv=0x%X [%d], argbuf=0x%X [%d], stack=0x%X\n",
          a_argc, (a_argv-a_argc), a_argv, (a_argbuf-a_argv), a_argbuf, (a_stack-a_argbuf), a_stack);
      fprintf (stderr,  "Total buffer length=%d\n", len);
      for (i = 0; i<my_argc; i++) fprintf (stderr,  "  argv[%d] = \"%s\", L:%d\n", i, prog_argv[i], (int) strlen(prog_argv[i])+1);
    }
  } else {
    if (prog_argv && prog_argv[1] != NULL) {
      fprintf (stderr,  "Program argc, argv error: Trying to pass arguments but at least one of [argc,argv,argbuf,stack] is undefined\n");
      fprintf (stderr,  "  Check your crt0\n");
    }
    Ok = 0;
  }
  if (Ok) {
  
    Ok &= ((a_argv>a_argc) && (a_argbuf>a_argv) && (a_stack>a_argbuf));   // In the following order: argc,argv,argbuf,stack
    if (!((a_argv>a_argc) && (a_argbuf>a_argv) && (a_stack>a_argbuf))) {
      fprintf (stderr,  "Program argc, argv error: expecting &argc>argv>&argbuf>&stack\n");
      fprintf (stderr,  "    Reading argc=0x%X [%d], argv=0x%X [%d], argbuf=0x%X [%d], stack=0x%X\n",
          a_argc, (a_argv-a_argc), a_argv, (a_argbuf-a_argv), a_argbuf, (a_stack-a_argbuf), a_stack);
    }
    Ok &= ((a_argc & 0x3)==0);            // int aligned
    if (!((a_argc & 0x3)==0)) {
      fprintf (stderr,  "Program argc, argv error: argc is not 4 byte aligned\n");
      fprintf (stderr,  "    Reading argc=0x%X [%d], argv=0x%X [%d], argbuf=0x%X [%d], stack=0x%X\n",
          a_argc, (a_argv-a_argc), a_argv, (a_argbuf-a_argv), a_argbuf, (a_stack-a_argbuf), a_stack);
    }
    Ok &= ((a_argbuf & 0x3)==0);            // int aligned
    if (!((a_argbuf & 0x3)==0)) {
      fprintf (stderr,  "Program argc, argv error: arg buffer is not 4 byte aligned\n");
      fprintf (stderr,  "    Reading argc=0x%X [%d], argv=0x%X [%d], argbuf=0x%X [%d], stack=0x%X\n",
          a_argc, (a_argv-a_argc), a_argv, (a_argbuf-a_argv), a_argbuf, (a_stack-a_argbuf), a_stack);
    }
    Ok &= (a_argv == (a_argc+4));             // argv right after argc
    if (!(a_argv == (a_argc+4))) {
      fprintf (stderr,  "Program argc, argv error: expecting &argv = &argc+4\n");
      fprintf (stderr,  "    Reading argc=0x%X [%d], argv=0x%X [%d], argbuf=0x%X [%d], stack=0x%X\n",
          a_argc, (a_argv-a_argc), a_argv, (a_argbuf-a_argv), a_argbuf, (a_stack-a_argbuf), a_stack);
    }
    Ok &= (my_argc <= ((a_argbuf - a_argv)>>2));        // Enough room for argv pointers
    if (!(my_argc <= ((a_argbuf - a_argv)>>2))) {
      fprintf (stderr,  "Program argc, argv error: Max requested argc exceeded: %d\n", my_argc);
      fprintf (stderr,  "    Reading argc=0x%X [%d], argv=0x%X [%d], argbuf=0x%X [%d], stack=0x%X\n",
          a_argc, (a_argv-a_argc), a_argv, (a_argbuf-a_argv), a_argbuf, (a_stack-a_argbuf), a_stack);
    }
    Ok &= (len <= (a_stack - a_argbuf));          // Enough space in the buffer
    if (!(len <= (a_stack - a_argbuf))) {
      fprintf (stderr,  "Program argc, argv error: Max argv buffer (argbuf) size exceeded: %d\n", len);
      fprintf (stderr,  "    Reading argc=0x%X [%d], argv=0x%X [%d], argbuf=0x%X [%d], stack=0x%X\n",
          a_argc, (a_argv-a_argc), a_argv, (a_argbuf-a_argv), a_argbuf, (a_stack-a_argbuf), a_stack);
    }
    if (Ok) {
      int j;
      storeWord (iss, a_argc, my_argc);
      for (i = 0; (Ok && (i < my_argc)); i++, a_argv += 4) {
        size_t strln = strlen (prog_argv[i]) + 1;

        storeWord (iss, a_argv, a_argbuf);
        for (j=0; j<strln; j++) storeByte(iss, a_argbuf+j, prog_argv[i][j]);
        a_argbuf += (unsigned int) strln;
      }
      if (!Ok) {
        if (Trace) fprintf (stderr,  "At least on write command for argc, argv failed\n");
        storeWord (iss, a_argc, 0);
      } else if (Trace) fprintf (stderr,  "Sucessfull argc, argv initialization\n");
    } else if (Trace) fprintf (stderr,  "Failed to check pre conditions for using argc, argv\n");
  } else if (Trace) fprintf (stderr,  "One of argc, argv, argbuf, stack symbols was not found in loaded elf file\n");

  if (look_for_symbol_group(abfd, ".text", BSF_LOCAL, "__mem_base", &a_base, "__mem_size", &a_size, NULL, NULL, NULL, NULL)) {
    unsigned int base, size;
    loadWord(iss, a_base, &base);
    loadWord(iss, a_size, &size);

    if (Trace) fprintf(stderr, "Mem Base: [%X] = %X, Mem Size: [%X] = %X\n", a_base, base, a_size, size);
    if ((base+size) > iss->mem_size) {
      fprintf(stderr, "crt0: __mem_base+_mem_size (%X+%X) exceeds simulator allocated memory: %lX\n",
        base, size, iss->mem_size);
      Ok=0;
    }
  } else {
    fprintf(stderr, "crt0: can't find __mem_base and/or __mem_size symbols\n");
    Ok=0;
  }
  return Ok;
}

int load_binary(iss_t *iss, const char *name, int argc, char **argv, iss_reg_t *bootaddr)
{
  bfd *abfd;
  int trace = 0;
  char *myname;
  asection *s;
  int lma_p = 0;
  int verbose_p = 10;
  int found_loadable_section = 0;
  unsigned long data_count = 0; /* Number of bytes transferred to memory */
  struct timeval start_time;

  abfd = bfd_openr (name, 0);
  // printf("%p\n", abfd);
  if (!abfd)
    {
      fprintf (stderr, "%s: can't open %s: %s\n",
          myname, name, bfd_errmsg (bfd_get_error ()));
      exit (1);
    } 

  if (!bfd_check_format (abfd, bfd_object))
    {
      fprintf (stderr, "%s: can't load %s: %s\n",
         myname, name, bfd_errmsg (bfd_get_error ()));
      exit (1);
    }


  for (s = abfd->sections; s; s = s->next)
    if (strcmp (bfd_get_section_name (abfd, s), ".text") == 0)
      {
        iss->textSection = s;
        iss->textSectionStart = bfd_get_section_vma (abfd, s);
        iss->textSectionEnd = bfd_get_section_vma (abfd, s) + bfd_section_size (abfd, s);
        break;
      }

  for (s = abfd->sections; s; s = s->next) 
  {
    if (s->flags & SEC_LOAD)
    {
      bfd_size_type size;

      size = bfd_get_section_size (s);
      if (size > 0)
      {
        unsigned char *buffer;
        bfd_vma lma;

        buffer = (unsigned char *)malloc (size);
        if (buffer == NULL)
        {
          printf("%s: insufficient memory to load \"%s\"\n", myname, name);
          return -1;
        }
        if (lma_p)
          lma = bfd_section_lma (abfd, s);
        else
          lma = bfd_section_vma (abfd, s);
        if (verbose_p)
        {
          printf("Loading section %s, size 0x%lx %s ",
           bfd_get_section_name (abfd, s),
           (unsigned long) size,
           (lma_p ? "lma" : "vma"));
          printf("%lx", lma);
          printf("\n");
        }
        data_count += size;
        bfd_get_section_contents (abfd, s, buffer, 0, size);
        memcpy(iss->mem_array + lma, buffer, size);
        found_loadable_section = 1;
        free (buffer);
      }
    }
  }

  if (!found_loadable_section)
  {
    printf(
     "%s: no loadable sections \"%s\"\n",
     myname, name);
    return -1;
  }

  if (verbose_p)
  {
    printf("Start address 0x%lx\n", bfd_get_start_address (abfd));
  }

  if (!handle_argc_argc(iss, abfd, argv)) {
  printf("Failed to initialize argc/argv\n"); return -1;
  }
  iss->abfd = abfd;

  if (bootaddr)
    *bootaddr = bfd_get_start_address (abfd);

  return 0;
}
