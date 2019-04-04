/**
 * @file aec.c
 *
 * @section LICENSE
 * Copyright 2012 - 2019
 *
 * Mathis Rosenhauer, Moritz Hanke, Joerg Behrens
 * Deutsches Klimarechenzentrum GmbH
 * Bundesstr. 45a
 * 20146 Hamburg Germany
 *
 * Luis Kornblueh
 * Max-Planck-Institut fuer Meteorologie
 * Bundesstr. 53
 * 20146 Hamburg
 * Germany
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @section DESCRIPTION
 *
 * CLI frontend for Adaptive Entropy Coding library
 *
 *
 *  IQzip
 *
 *  Copyright (C) 2019, Libre Space Foundation <https://libre.space/>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libaec.h>
#include <iqzip/iqzip_compression_header.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#define CHUNK 10485760

int
get_param (unsigned int *param, int *iarg, char *argv[])
{
  if (strlen (argv[*iarg]) == 2) {
    (*iarg)++;
    if (argv[*iarg][0] == '-')
      return 1;
    else
      *param = atoi (argv[*iarg]);
  }
  else {
    *param = atoi (&argv[*iarg][2]);
  }
  return 0;
}

int
main (int argc, char *argv[])
{
  struct aec_stream strm;
  unsigned char *in;
  unsigned char *out;
  size_t total_out;
  unsigned int chunk;
  int status;
  int input_avail, output_avail;
  char *infn, *outfn;
  FILE *infp, *outfp;
  int dflag;
  char *opt;
  int iarg;
  size_t iqzip_header_size;

  iqzip_compression_header *iqzip_header;

  chunk = CHUNK;
  strm.bits_per_sample = 8;
  strm.block_size = 8;
  strm.rsi = 2;
  strm.flags = AEC_DATA_PREPROCESS;
  dflag = 0;
  iarg = 1;

  uint8_t enable_preprocessing = 1;
  uint8_t endianness = 1;
  uint8_t data_sense = 1;
  uint8_t restricted_codes = 0;

  while (iarg < argc - 2) {
    opt = argv[iarg];
    if (opt[0] != '-') {
      goto FAIL;
    }
    switch (opt[1])
      {
      case '3':
        strm.flags |= AEC_DATA_3BYTE;
        break;
      case 'F':
        strm.flags |= AEC_NOT_ENFORCE;
        break;
      case 'N':
        strm.flags &= ~AEC_DATA_PREPROCESS;
        enable_preprocessing = 0;
        break;
      case 'b':
        if (get_param (&chunk, &iarg, argv)) {
          goto FAIL;
        }
        break;
      case 'd':
        dflag = 1;
        break;
      case 'j':
        if (get_param (&strm.block_size, &iarg, argv)) {
          goto FAIL;
        }
        break;
      case 'm':
        strm.flags |= AEC_DATA_MSB;
        endianness = 0;
        break;
      case 'n':
        if (get_param (&strm.bits_per_sample, &iarg, argv)) {
          goto FAIL;
        }
        break;
      case 'p':
        strm.flags |= AEC_PAD_RSI;
        break;
      case 'r':
        if (get_param (&strm.rsi, &iarg, argv)) {
          goto FAIL;
        }
        break;
      case 's':
        strm.flags |= AEC_DATA_SIGNED;
        data_sense = 0;
        break;
      case 't':
        strm.flags |= AEC_RESTRICTED;
        restricted_codes = 1;
        break;
      default:
        goto FAIL;
      }
    iarg++;
  }

  if (argc - iarg < 2) {
    goto FAIL;
  }

  infn = argv[iarg];
  outfn = argv[iarg + 1];

  if ((infp = fopen (infn, "rb")) == NULL) {
    fprintf (stderr, "ERROR: cannot open input file %s\n", infn);
    return 1;
  }

  iqzip_header = new iqzip_compression_header (0, 0, 0, 7, 0, 0, 0, 0, 1, strm.rsi, enable_preprocessing, 1, 3,
                                               strm.block_size, data_sense, strm.bits_per_sample, 1, restricted_codes, endianness);

  if (dflag) {
    //FIXME: Byte to bypass according to CIP header size
    iqzip_header = new iqzip_compression_header ();
    iqzip_header_size = iqzip_header->parse_header_from_file (infn);
    strm.block_size = iqzip_header->decode_block_size();
    enable_preprocessing = iqzip_header->decode_preprocessor_status ();
    if (!enable_preprocessing) {
      strm.flags &= ~AEC_DATA_PREPROCESS;
    }

    data_sense = iqzip_header->decode_preprocessor_data_sense ();
    if (!data_sense) {
      strm.flags |= AEC_DATA_SIGNED;
    }
    restricted_codes = iqzip_header->decode_extended_parameters_restricted_code_option();
    if (restricted_codes) {
      strm.flags |= AEC_RESTRICTED;
    }
    strm.bits_per_sample = (size_t) iqzip_header->decode_preprocessor_sample_resolution ();
    strm.rsi = (size_t) iqzip_header->decode_reference_sample_interval();

    if ((outfp = fopen (outfn, "wb")) == NULL) {
      fprintf (stderr, "ERROR: cannot open output file %s\n", infn);
      return 1;
    }
    std::cout << "Header size: " << (int)iqzip_header_size << std::endl;
    std::cout << "Block size: " << (int)strm.block_size << std::endl;
    std::cout << "Data sense: " << (int)data_sense << std::endl;
    std::cout << "Preprocessing: " <<(int) enable_preprocessing << std::endl;
    std::cout << "Restricted: " << (int)restricted_codes << std::endl;
    std::cout << "Resolution: " << (int)strm.bits_per_sample << std::endl;
    std::cout << "reference samples: " << (int)strm.rsi << std::endl;

    fseek (infp, iqzip_header_size, SEEK_SET);
    status = aec_decode_init (&strm);
  }
  else {
    if ((outfp = fopen (outfn, "ab")) == NULL) {
      fprintf (stderr, "ERROR: cannot open output file %s\n", infn);
      return 1;
    }
    iqzip_header->write_header_to_file (outfn);
    status = aec_encode_init (&strm);
  }

  if (strm.bits_per_sample > 16) {
    if (strm.bits_per_sample <= 24 && strm.flags & AEC_DATA_3BYTE)
      chunk *= 3;
    else
      chunk *= 4;
  }
  else if (strm.bits_per_sample > 8) {
    chunk *= 2;
  }

  out = (unsigned char *) malloc (chunk);
  in = (unsigned char *) malloc (chunk);

  if (in == NULL || out == NULL) {
    exit (-1);
  }

  total_out = 0;
  strm.avail_in = 0;
  strm.avail_out = chunk;
  strm.next_out = out;

  input_avail = 1;
  output_avail = 1;

  if (status != AEC_OK) {
    fprintf (stderr, "ERROR: initialization failed (%d)\n", status);
    return 1;
  }

  while (input_avail || output_avail) {
    if (strm.avail_in == 0 && input_avail) {
      strm.avail_in = fread (in, 1, chunk, infp);
      if (strm.avail_in != chunk)
        input_avail = 0;
      strm.next_in = in;
    }

    if (dflag) {
      status = aec_decode (&strm, AEC_NO_FLUSH);
    }
    else {
      status = aec_encode (&strm, AEC_NO_FLUSH);
    }

    if (status != AEC_OK) {
      fprintf (stderr, "ERROR: %i\n", status);
      return 1;
    }

    if (strm.total_out - total_out > 0) {
      fwrite (out, strm.total_out - total_out, 1, outfp);
      total_out = strm.total_out;
      output_avail = 1;
      strm.next_out = out;
      strm.avail_out = chunk;
    }
    else {
      output_avail = 0;
    }

  }

  if (dflag) {
    aec_decode_end (&strm);
  }
  else {
    if ((status = aec_encode (&strm, AEC_FLUSH)) != AEC_OK) {
      fprintf (stderr, "ERROR: while flushing output (%i)\n", status);
      return 1;
    }
    if (strm.total_out - total_out > 0) {
      fwrite (out, strm.total_out - total_out, 1, outfp);
    }
    aec_encode_end (&strm);
  }

  fclose (infp);
  fclose (outfp);
  free (in);
  free (out);
  delete iqzip_header;
  return 0;

  FAIL: fprintf (stderr, "NAME\n\taec - encode or decode files ");
  fprintf (stderr, "with Adaptive Entropy Coding\n\n");
  fprintf (stderr, "SYNOPSIS\n\taec [OPTION]... SOURCE DEST\n");
  fprintf (stderr, "\nOPTIONS\n");
  fprintf (stderr, "\t-3\n\t\t24 bit samples are stored in 3 bytes\n");
  fprintf (stderr, "\t-N\n\t\tdisable pre/post processing\n");
  fprintf (stderr, "\t-b size\n\t\tinternal buffer size in bytes\n");
  fprintf (stderr, "\t-d\n\t\tdecode SOURCE. If -d is not used: encode.\n");
  fprintf (stderr, "\t-j samples\n\t\tblock size in samples\n");
  fprintf (stderr,
           "\t-F\n\t\tdo not enforce standard regarding legal block sizes\n");
  fprintf (stderr, "\t-m\n\t\tsamples are MSB first. Default is LSB\n");
  fprintf (stderr, "\t-n bits\n\t\tbits per sample\n");
  fprintf (stderr, "\t-p\n\t\tpad RSI to byte boundary\n");
  fprintf (stderr, "\t-r blocks\n\t\treference sample interval in blocks\n");
  fprintf (stderr, "\t-s\n\t\tsamples are signed. Default is unsigned\n");
  fprintf (stderr, "\t-t\n\t\tuse restricted set of code options\n\n");
  return 1;
}
