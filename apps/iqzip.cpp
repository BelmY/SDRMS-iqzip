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

#include <iqzip/ccsds_types.h>
#include <iqzip/compressor.h>
#include <iqzip/decompressor.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace iqzip::compression;

#define CHUNK 10485760

template<class T>
int
get_param(T *param, int *iarg, char *argv[])
{
    if (strlen(argv[*iarg]) == 2) {
        (*iarg)++;
        if (argv[*iarg][0] == '-') {
            return 1;
        }
        else {
            *param = atoi(argv[*iarg]);
        }
    }
    else {
        *param = atoi(&argv[*iarg][2]);
    }
    return 0;
}

int
main(int argc, char *argv[])
{
    char *infn, *outfn;
    int dflag;
    char *opt;
    int iarg;

    dflag = 0;
    iarg = 1;

    uint8_t enable_preprocessing = 1;
    uint8_t endianness = 1;
    uint8_t data_sense = 1;
    uint8_t restricted_codes = 0;
    uint8_t sample_resolution = 8;
    uint8_t reference_sample_interval = 1;
    uint16_t block_size = 64;

    while (iarg < argc - 2) {
        opt = argv[iarg];
        if (opt[0] != '-') {
            goto FAIL;
        }
        switch (opt[1]) {
        case 'N':
            enable_preprocessing = 0;
            break;
        case 'd':
            dflag = 1;
            break;
        case 'j':
            if (get_param(&block_size, &iarg, argv)) {
                goto FAIL;
            }
            break;
        case 'm':
            endianness = 0;
            break;
        case 'n':
            if (get_param(&sample_resolution, &iarg, argv)) {
                goto FAIL;
            }
            break;
        case 'r':
            if (get_param(&reference_sample_interval, &iarg, argv)) {
                goto FAIL;
            }
            break;
        case 's':
            data_sense = 0;
            break;
        case 't':
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

    if (dflag) {
        iqzip::compression::decompressor_sptr sptr =
            iqzip::compression::create_decompressor();
        /* Initialize decompressor */
        sptr->decompress_init(infn, outfn);
        /* Decompress file */
        sptr->decompress();
        /* Finalize decompression */
        sptr->decompress_fin();
    }
    else {
        compressor_sptr sptr =
            iqzip::compression::create_compressor(
                (uint8_t)header::PACKET_VERSION::CCSDS_PACKET_VERSION_1,
                (uint8_t)header::PACKET_TYPE::CCSDS_TELECOMMAND,
                (uint8_t)header::PACKET_SECONDARY_HEADER_FLAG::SEC_HDR_PRESENT,
                (uint16_t)header::PACKET_APPLICATION_PROCESS_IDENTIFIER::IDLE_PACKET,
                (uint8_t)header::PACKET_SEQUENCE_FLAGS::CONTINUATION_SEGMENT,
                (uint16_t)0xdffe,
                (uint16_t)0x7efe,
                (uint16_t)0xffff,
                (uint8_t)header::COMPRESSION_TECHNIQUE_IDENTIFICATION::CCSDS_LOSSLESS_COMPRESSION,
                (uint8_t)reference_sample_interval,
                (uint8_t)enable_preprocessing,
                (uint8_t)header::PREPROCESSOR_PREDICTOR_TYPE::APPLICATION_SPECIFIC,
                (uint8_t)header::PREPROCESSOR_MAPPER_TYPE::PREDICTION_ERROR,
                (uint16_t)block_size,
                (uint8_t)data_sense,
                (uint8_t)sample_resolution,
                (uint8_t)1,
                (uint8_t)restricted_codes,
                (uint8_t)endianness);
        /* Initialize compressor */
        sptr->compress_init(infn, outfn);
        /* Compress file */
        sptr->compress();
        /* Finalize compression */
        sptr->compress_fin();
    }
    return 0;

FAIL:
    fprintf(stderr, "NAME\n\taec - encode or decode files ");
    fprintf(stderr, "with Adaptive Entropy Coding\n\n");
    fprintf(stderr, "SYNOPSIS\n\taec [OPTION]... SOURCE DEST\n");
    fprintf(stderr, "\nOPTIONS\n");
    fprintf(stderr, "\t-N\n\t\tdisable pre/post processing\n");
    fprintf(stderr, "\t-d\n\t\tdecode SOURCE. If -d is not used: encode.\n");
    fprintf(stderr, "\t-j samples\n\t\tblock size in samples\n");
    fprintf(stderr,
            "\t-F\n\t\tdo not enforce standard regarding legal block sizes\n");
    fprintf(stderr, "\t-m\n\t\tsamples are MSB first. Default is LSB\n");
    fprintf(stderr, "\t-n bits\n\t\tbits per sample\n");
    fprintf(stderr, "\t-r blocks\n\t\treference sample interval in blocks\n");
    fprintf(stderr, "\t-s\n\t\tsamples are signed. Default is unsigned\n");
    fprintf(stderr, "\t-t\n\t\tuse restricted set of code options\n\n");
    return 1;
}
