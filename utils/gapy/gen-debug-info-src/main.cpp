/* Main header file for the bfd library -- portable access to object files.

   Copyright (C) 1990-2017 Free Software Foundation, Inc.

   Contributed by Cygnus Support.

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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <bfd/config.h>
#include <bfd/bfd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static asymbol **asymbols = NULL;
static bfd *abfd;

static int dump_debug(FILE *output)
{
    if (asymbols == NULL) {
        long symsize;
        long symbol_count;
        symsize = bfd_get_symtab_upper_bound (abfd);
        if (symsize < 0) return -1;
        asymbols = (asymbol **) malloc (symsize);
        symbol_count = bfd_canonicalize_symtab (abfd, asymbols);
        if (symbol_count < 0) return -1;
    }

    const char *file, *function;
    unsigned int line;
    
    for (asection *s = abfd->sections; s; s = s->next)
    {
        if (s->flags & SEC_CODE)
        {
            unsigned long long section_base = bfd_get_section_vma(abfd, s);

            for (unsigned long long addr = 0;
                addr < bfd_section_size (abfd, s);
                addr+=2)
            {
                if (bfd_find_nearest_line(abfd, s, asymbols, addr, &file, &function, &line))
                {
                    if (output)
                    {
                        fprintf(output, "%llx %s %s %s %d\n", section_base + addr, function, function, file, line);
                    }
                    else
                    {
                        printf("%llx %s %s %s %d\n", section_base + addr, function, function, file, line);
                    }
                }
            }
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    char *input = argv[1], *output=NULL;
    
    if (argc > 2)
    {
        output = argv[2];
    }

    abfd = bfd_openr(input, 0);
    if (abfd == NULL)
    {
        fprintf (stderr, "Can't open %s: %s\n", argv[1], bfd_errmsg (bfd_get_error ()));
        return -1;
    }

    if (!bfd_check_format (abfd, bfd_object))
    {
        fprintf (stderr, "Can't load %s: %s\n", argv[1], bfd_errmsg (bfd_get_error ()));
        return -1;
    }

    FILE *output_file = NULL;
    
    if (output)
    {
        output_file = fopen(output, "w");
        if (output_file == NULL)
        {
            return -1;
        }
    }

    if (dump_debug(output_file))
    {
        return -1;
    }

    if (output_file)
    {
        fclose(output_file);
    }

    return 0;
}