/*
 * File: mprfgen.c                                                          
 * Description: Multi-port register file (LUT/Block-RAM based) generator 
 * for Spartan-3, Virtex-4, Virtex-5 FPGAs.                            
 * Author: Nikolaos Kavvadias <nikolaos.kavvadias@gmail.com>                
 * Copyright: (C) Nikolaos Kavvadias 2007, 2008, 2009, 2010                 
 * Website: http://www.nkavvadias.co.cc                            
 *                                                                          
 * This file is part of mprfgen, and is distributed under the terms of the  
 * GNU Lesser General Public License.
 *
 * Copies of the GNU General Public License and the GNU Lesser General Public
 * License are included with this distrubution in the files COPYING and
 * COPYING.LESSER, respectively.
 *
 * mprfgen is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * mprfgen is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with mprfgen. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef enum {
  READ_ASYNC = 0,
  READ_FIRST = 1,
  WRITE_FIRST = 2,
  READ_THROUGH = 3
} READ_MODE;

int enable_infer=0, enable_read_mode=READ_ASYNC;
unsigned int nwp=1, nrp=2, bw=16, nregs=1024;


/* print_spaces:
 * Print a configurable number of space characters to an output file (specified 
 * by the given filename; the file is assumed already opened).
 */
void print_spaces(FILE *f, int nspaces)
{
  int i;
  
  for (i = 0; i < nspaces; i++)
  {
    fprintf(f, " ");
  }
}

/* pfprintf: 
 * fprintf prefixed by a number of space characters. 
 */
void pfprintf(FILE *f, int nspaces, char *fmt, ...)
{
  va_list args;
  print_spaces(f, nspaces);
  va_start(args, fmt);
  vfprintf(f, fmt, args);
  va_end(args);
}

/* log2c: 
 * Integer logarithm-2 function (rounds to ceiling).
 */
unsigned int log2c(int inp)
{
  int i, temp, log;

  log = 0;
  temp = 1;

  for (i=0; i<=inp; i++)
  {
    if (temp < inp)
    {
      log  = log + 1;
      temp = temp * 2;
    }
  }

  return (log);
}

/* decode_read_mode:
 * Returns the corresponding string representation of the given block 
 * RAM read mode.
 */
char *decode_read_mode(READ_MODE val)
{
  char *temp_s;
  
  switch (val)
  {
    case READ_ASYNC:
      temp_s = malloc((strlen("READ_ASYNC")+1) * sizeof(char));
      strcpy(temp_s, "READ_ASYNC");
      break;
    case READ_FIRST:
      temp_s = malloc((strlen("READ_FIRST")+1) * sizeof(char));
      strcpy(temp_s, "READ_FIRST");
      break;
    case WRITE_FIRST:
      temp_s = malloc((strlen("WRITE_FIRST")+1) * sizeof(char));
      strcpy(temp_s, "WRITE_FIRST");
      break;
    case READ_THROUGH:
      temp_s = malloc((strlen("READ_THROUGH")+1) * sizeof(char));
      strcpy(temp_s, "READ_THROUGH");
      break;
    default:
      fprintf(stderr, "Error: Unknown block RAM read mode.\n");
      exit(1);
      break;
  }
  
  return (temp_s);
}

/* print_mprf_tu_prologue:
 * Prints the prologue for the generated memory model file. 
 */
void print_mprf_tu_prologue(FILE *infile)
{
  pfprintf(infile, 0, "library IEEE;\n");
  pfprintf(infile, 0, "use IEEE.std_logic_1164.all;\n");
  pfprintf(infile, 0, "use IEEE.std_logic_unsigned.all;\n");
  pfprintf(infile, 0, "use IEEE.std_logic_arith.all;\n");
  pfprintf(infile, 0, "use WORK.useful_functions_pkg.all;\n");

  if (enable_infer == 0)
  {
    pfprintf(infile, 0, "library UNISIM;\n");
    pfprintf(infile, 0, "use UNISIM.vcomponents.all;\n");
  }

  fprintf(infile, "\n");
}

/* print_mprf_tu_prologue:
 * Prints the entity of the generated memory model. 
 */
void print_mprf_entity(FILE *infile, unsigned int num_ni, unsigned int num_no)
{
  pfprintf(infile, 0, "entity regfile is\n");
  pfprintf(infile, 2, "generic (\n");
  pfprintf(infile, 4, "NWP           : integer := %d;\n", num_no);
  pfprintf(infile, 4, "NRP           : integer := %d;\n", num_ni);
  pfprintf(infile, 4, "AW            : integer := %d;\n", log2c(nregs));
  pfprintf(infile, 4, "DW            : integer := %d\n", bw);
  pfprintf(infile, 2, ");\n");
  pfprintf(infile, 2, "port (\n");
  pfprintf(infile, 4, "clock         : in  std_logic;\n");
  pfprintf(infile, 4, "reset         : in  std_logic;\n");
  pfprintf(infile, 4, "enable        : in  std_logic;\n");
  pfprintf(infile, 4, "we_v          : in  std_logic_vector(NWP-1 downto 0);\n");
  pfprintf(infile, 4, "re_v          : in  std_logic_vector(NRP-1 downto 0);\n");
  pfprintf(infile, 4, "waddr_v       : in  std_logic_vector(NWP*AW-1 downto 0);\n");
  pfprintf(infile, 4, "raddr_v       : in  std_logic_vector(NRP*AW-1 downto 0);\n");
  pfprintf(infile, 4, "input_data_v  : in  std_logic_vector(NWP*DW-1 downto 0);\n");
  pfprintf(infile, 4, "ram_output_v  : out std_logic_vector(NRP*DW-1 downto 0)\n");
  pfprintf(infile, 2, ");\n");
  pfprintf(infile, 0, "end regfile;\n\n");
}

/* print_mprf_architecture_prologue:
 * Prints the declaration part of the architecture for the generated memory 
 * model. 
 */
void print_mprf_architecture_prologue(FILE *infile, unsigned int num_ni, unsigned int num_no)
{
  pfprintf(infile, 0, "architecture rtl of regfile is\n");

  if (enable_infer == 1)
  {
    pfprintf(infile, 2, "component regfile_core\n");
    pfprintf(infile, 4, "generic (\n");
    pfprintf(infile, 6, "AW            : integer :=  5;\n");
    pfprintf(infile, 6, "DW            : integer := 32\n");
    pfprintf(infile, 4, ");\n");
    pfprintf(infile, 4, "port (\n");
    pfprintf(infile, 6, "clock         : in  std_logic;\n");
    pfprintf(infile, 6, "reset         : in  std_logic;\n");
    pfprintf(infile, 6, "enable        : in  std_logic;\n");
    pfprintf(infile, 6, "we            : in  std_logic;\n");
    pfprintf(infile, 6, "re            : in  std_logic;\n");
    pfprintf(infile, 6, "waddr         : in  std_logic_vector(AW-1 downto 0);\n");
    pfprintf(infile, 6, "raddr         : in  std_logic_vector(AW-1 downto 0);\n");
    pfprintf(infile, 6, "input_data    : in  std_logic_vector(DW-1 downto 0);\n");
    pfprintf(infile, 6, "ram_output    : out std_logic_vector(DW-1 downto 0)\n");
    pfprintf(infile, 4, ");\n");
    pfprintf(infile, 2, "end component;\n");
  }
  
  pfprintf(infile, 2, "constant NREGS : integer := 2**AW;\n");
  pfprintf(infile, 2, "type banksel_type is array (NRP-1 downto 0) of std_logic_vector(log2c(NWP)-1 downto 0);\n");

  if (num_no != 1)
  {
    pfprintf(infile, 2, "signal banksel_v    : std_logic_vector(NRP*log2c(NWP)-1 downto 0);\n");
    pfprintf(infile, 2, "signal ia_sel       : banksel_type;\n");
  }
  
  pfprintf(infile, 2, "signal ram_output_i : std_logic_vector((NRP*NWP*DW)-1 downto 0);\n");

  pfprintf(infile, 0, "begin\n");
}

/* print_mprf_bram_gen:
 * Prints the actual block RAM instances. 
 */
void print_mprf_bram_gen(FILE *infile, unsigned int num_ni, unsigned int num_no)
{
  int i, j, k;

  if (enable_infer == 1)
  {
    for (j = 0; j < num_ni; j++)
    {
      for (i = 0; i < num_no; i++)
      {
        pfprintf(infile, 2, "nwp_nrp_bram_instance_%d : entity WORK.regfile_core(%s)\n", 
          i*num_ni + j, decode_read_mode(enable_read_mode));
        pfprintf(infile, 4, "generic map (\n");
        pfprintf(infile, 6, "AW            => AW-log2c(NWP),\n");
        pfprintf(infile, 6, "DW            => DW\n");
        pfprintf(infile, 4, ")\n");
        pfprintf(infile, 4, "port map (\n");
        pfprintf(infile, 6, "clock         => clock,\n");
        pfprintf(infile, 6, "reset         => reset,\n");
        pfprintf(infile, 6, "enable        => enable,\n");
        pfprintf(infile, 6, "we            => we_v(%d),\n", i);
        pfprintf(infile, 6, "re            => re_v(%d),\n", j);
        pfprintf(infile, 6, "waddr         => waddr_v(AW*(%d+1)-log2c(NWP)-1 downto AW*%d),\n", i, i);
        pfprintf(infile, 6, "raddr         => raddr_v(AW*(%d+1)-log2c(NWP)-1 downto AW*%d),\n", j, j);
        pfprintf(infile, 6, "input_data    => input_data_v(DW*(%d+1)-1 downto DW*%d),\n", i, i);
        pfprintf(infile, 6, "ram_output    => ram_output_i(DW*((%d*NRP+%d)+1)-1 downto DW*(%d*NRP+%d))\n", i, j, i, j);
        pfprintf(infile, 4, ");\n\n");
      }
    }
  }
  else
  {
    if ((enable_read_mode == READ_ASYNC) || (enable_read_mode == READ_THROUGH))
    {
      fprintf(stderr, "Error: Unsupported read mode for an instantiated block RAM.\n");
      exit(1);
    }
    
    for (j = 0; j < num_ni; j++)
    {
      for (i = 0; i < num_no; i++)
      {
        pfprintf(infile, 2, "nwp_nrp_bram_instance_%d : RAMB16_S36_S36\n", i*num_ni + j);
        pfprintf(infile, 4, "generic map (\n");
        pfprintf(infile, 6, "WRITE_MODE_A => \"%s\",\n", decode_read_mode(enable_read_mode));
        pfprintf(infile, 6, "WRITE_MODE_B => \"%s\",\n", decode_read_mode(enable_read_mode));
        for (k = 0; k <= 0x3F; k++)
        {
          pfprintf(infile, 6, "INIT_%02X => X\"0000000000000000000000000000000000000000000000000000000000000000\",\n", k);
        }
        for (k = 0; k <= 0x07; k++)
        { 
          pfprintf(infile, 6, "INITP_%02X => X\"0000000000000000000000000000000000000000000000000000000000000000\"", k);
          if (k < 0x07)
          {
            fprintf(infile, ",");
		  }
		  fprintf(infile, "\n");
        }
        pfprintf(infile, 4, ")\n");
        pfprintf(infile, 4, "port map (\n");
        pfprintf(infile, 6, "DIA           => input_data_v(DW*(%d+1)-1 downto DW*%d),\n", i, i);
        pfprintf(infile, 6, "DIPA          => (others => '0'),\n");
        pfprintf(infile, 6, "ADDRA         => waddr_v(AW*(%d+1)-log2c(NWP)-1 downto AW*%d),\n", i, i);
        pfprintf(infile, 6, "ENA           => enable,\n");
        pfprintf(infile, 6, "WEA           => we_v(%d),\n", i);
        pfprintf(infile, 6, "SSRA          => reset,\n");
        pfprintf(infile, 6, "CLKA          => clock,\n");
        pfprintf(infile, 6, "DOA           => open,\n");
        pfprintf(infile, 6, "DOPA          => open,\n");
        pfprintf(infile, 6, "DIB           => (others => '0'),\n");
        pfprintf(infile, 6, "DIPB          => (others => '0'),\n");
        pfprintf(infile, 6, "ADDRB         => raddr_v(AW*(%d+1)-log2c(NWP)-1 downto AW*%d),\n", j, j);
        pfprintf(infile, 6, "ENB           => enable,\n");
        pfprintf(infile, 6, "WEB           => '0',\n");
        pfprintf(infile, 6, "SSRB          => reset,\n");
        pfprintf(infile, 6, "CLKB          => clock,\n");
        pfprintf(infile, 6, "DOB           => ram_output_i(DW*((%d*NRP+%d)+1)-1 downto DW*(%d*NRP+%d)),\n", i, j, i, j);
        pfprintf(infile, 6, "DOPB          => open\n");
        pfprintf(infile, 4, ");\n\n");
      }
    }
  }
}

/* print_mprf_banksel_gen:
 * Prints the concurrent assignments for bank selection. 
 */
void print_mprf_banksel_gen(FILE *infile, unsigned int num_ni, unsigned int num_no)
{
  int i, j;

  if (num_no != 1)
  {
    for (i = 0; i < num_ni; i++)
    {
      pfprintf(infile, 2, "banksel_v(log2c(NWP)*(%d+1)-1 downto log2c(NWP)*%d) <= raddr_v(AW*(%d+1)-1 downto AW*(%d+1)-log2c(NWP));\n",
      i, i, i, i);
    }
    fprintf(infile,"\n");
  }
}

/* print_mprf_outmuxes_gen:
 * Prints the necessary code for handling output port multiplexers. 
 */
void print_mprf_outmuxes_gen(FILE *infile, unsigned int num_ni, unsigned int num_no)
{
  int i, j;

  if (num_no == 1)
  {
    for (i = 0; i < num_ni; i++)
    {
      pfprintf(infile, 2, "ram_output_v(DW*(%d+1)-1 downto DW*%d) <= ram_output_i(DW*(%d+1)-1 downto DW*%d);\n",
      i, i, i, i);
    }
  }
  else
  {
    for (j = 0; j < num_ni; j++)
    {
      pfprintf(infile, 2, "process (ram_output_i, banksel_v)\n");
      pfprintf(infile, 4, "variable ia_sel_part : integer range 0 to NWP-1;\n");
      pfprintf(infile, 2, "begin\n");
      pfprintf(infile, 4, "ia_sel(%d) <= banksel_v(log2c(NWP)*(%d+1)-1 downto log2c(NWP)*%d);\n", j, j, j);
      pfprintf(infile, 4, "ia_sel_part := conv_integer(ia_sel(%d));\n", j);
      pfprintf(infile, 4, "case ia_sel_part is\n");

      for (i = 0; i < num_no; i++)
      {
        pfprintf(infile, 6, "when %d      => ram_output_v(DW*(%d+1)-1 downto DW*%d) <= ram_output_i(DW*(%d+%d*NRP+1)-1 downto DW*(%d+%d*NRP));\n",
        i, j, j, j, i, j, i);
      }

      pfprintf(infile, 6, "when others => ram_output_v(DW*(%d+1)-1 downto DW*%d) <= (others => '0');\n", j, j);
      pfprintf(infile, 4, "end case;\n");
      pfprintf(infile, 2, "end process;\n\n");
    }
  }
}

/* print_mprf_architecture_body:
 * Generates the architecture body of the memory model. 
 */
void print_mprf_architecture_body(FILE *infile, unsigned int num_ni, unsigned int num_no)
{
  print_mprf_bram_gen(infile, num_ni, num_no);
  print_mprf_banksel_gen(infile, num_ni, num_no);
  print_mprf_outmuxes_gen(infile, num_ni, num_no);
}

/* print_mprf_epilogue:
 * Prints the epilogue of the architecture for the generated memory model. 
 */
void print_mprf_epilogue(FILE *infile)
{
  fprintf(infile, "end rtl;\n");
}

/* print_usage:
 * Print usage instructions for the "mprfgen" program.
 */
static void print_usage()
{
  printf("\n");
  printf("* Usage:\n");
  printf("* mprfen [options] <out.vhd>\n");
  printf("* Example: ./mprfgen -infer -read-first -nwp 2 -nrp 3 file.vhd\n");
  printf("* \n");
  printf("* Options:\n");
  printf("*   -h:           Print this help.\n");
  printf("*   -infer:       Use generic RAM storage that can be inferred as\n");
  printf("*                 block RAM(s).\n");
  printf("*   -<read-mode>: Read mode supported by the generated RAM. Valid\n");
  printf("*                 options: {read-async, read-first, write-first,\n");
  printf("*                 read-through}. \"read-through\" cannot be used for\n");
  printf("*                 for block RAM instantiation. Default is \"read-async\"\n");
  printf("*   -nwp <num>:   Number of write ports for the register file.\n");
  printf("*   -nrp <num>:   Number of read ports for the register file.\n");
  printf("*   -bw <num>:    Bitwidth for each memory entry (default: 16).\n");
  printf("*   -nregs <num>: Memory size (default: 1024).\n");
  printf("* \n");
  printf("* For further information, please refer to the website:\n");
  printf("* http://electronics.physics.auth.gr/tomeas/en/kavvadias.html\n\n");
}

/* main:
 * Program entry.
 */
int main(int argc, char **argv)
{
  FILE *file_i, *file_o;
  int i, j, count;
  int file_args=0, byte_offset=0;
  unsigned int temp_word;

  /* Command-line argument passing. */
  if (argc < 2)
  {
    print_usage();
    return 0;
  }

  for (i = 1; i < argc; i++)
  {
    if (strcmp("-h",argv[i]) == 0)
    {
      print_usage();
      exit(1);
    }
    else if (strcmp("-infer",argv[i]) == 0)
    {
      enable_infer = 1;
    }
    else if (strcmp("-read-async",argv[i]) == 0)
    {
      enable_read_mode = READ_ASYNC;
    }
    else if (strcmp("-read-first",argv[i]) == 0)
    {
      enable_read_mode = READ_FIRST;
    }
    else if (strcmp("-write-first",argv[i]) == 0)
    {
      enable_read_mode = WRITE_FIRST;
    }
    else if (strcmp("-read-through",argv[i]) == 0)
    {
      enable_read_mode = READ_THROUGH;
    }
    else if (strcmp("-nwp",argv[i]) == 0)
    {
      if ((i+1) < argc)
      {
        i++;
        nwp = atoi(argv[i]);
      }
    }
    else if (strcmp("-nrp",argv[i]) == 0)
    {
      if ((i+1) < argc)
      {
        i++;
        nrp = atoi(argv[i]);
      }
    }
    else if (strcmp("-bw",argv[i]) == 0)
    {
      if ((i+1) < argc)
      {
        i++;
        bw = atoi(argv[i]);
      }
    }
    else if (strcmp("-nregs",argv[i]) == 0)
    {
      if ((i+1) < argc)
      {
        i++;
        nregs = atoi(argv[i]);
      }
    }
    else
    {
      if (argv[i][0] != '-')
      {
        file_o = fopen(argv[i], "wb");
        if (file_o == NULL)
        {
          fprintf(stderr,"Error: Can't write %s!\n", argv[i]);
          return -1;
        }
      }
    }
  }

  /* The actual logic of the "mprfgen" program. */
  print_mprf_tu_prologue(file_o);
  print_mprf_entity(file_o, nrp, nwp);
  print_mprf_architecture_prologue(file_o, nrp, nwp);
  print_mprf_architecture_body(file_o, nrp, nwp);
  print_mprf_epilogue(file_o);

  fclose(file_o);

  return 0;
}
