# Python 3.7 script
'''
iqzip
Copyright (C) 2019  librespacefoundation / sdrmakerspace

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
'''

'''
This script runs FLAC, Shorten, LZ77 and LZMA on all files named *.dat in current
directory unless another one is specified with the -d argument. File names must
end with .dat otherwise there are skipped. For x-axis to correclty report the 
observation number file names should follow the format 
something_<observationNumber>.dat or <observationNumber>.dat where 
<observationNumber> is the observation number.
Default block size for FLAC and Shorten algorithms is 8192 samples and filter 
size is 12 coefficients. Block size and LPC filter order can be passed as command 
line arguments. For the LZ77 and LZMA the --best option is passed.
Print help for more information.
Compression ratio is measured as: 
    compressed_size / original_size
measured with system commands as output of each algorithm may differ.
The compressed output files are deleted after we have measured the compression ratio.
The script saves 4 plots in the data directory:
    1) Compressed Ratio for each file
    2) Execution Time for each file
    3) Average Compression Ratio for each algorithm
    4) Average Execution Time for each algorithm
Also 2 csv files are generated:
    1) Compressed Ratio for each file
        * Columns are each algorithm
        * Rows are each observation
    2) Execution Time for each file
        * Columns are each algorithm
        * Rows are each observation
'''
import os
import glob
import subprocess
import time
import numpy as np
import matplotlib.pyplot as plt
import sys
import getopt
import copy
import pandas as pd

def main(argv):
    # Arrays to save measurements
    compress_ratios = {'FLAC':[], 'Shorten':[], 'LZ77': [], 'LZMA': []}
    compress_times = copy.deepcopy(compress_ratios)
    filenames = []
    block_size = 8192
    filter_order = 12

    #Initialize options
    try:
        opts, args = getopt.getopt(argv, "b:l:d:h", ["block_size=", "filter_order=", 
                                                     "directory=", "help"])
    except getopt.GetoptError as err:
        print("Invalid arguments: " + str(err))
        print_help()
        sys.exit(2)
    for opt, arg in opts:
        if opt in ('-h', '--help'):
            print_help()
            sys.exit(0)
        elif opt in ('-b', '--block_size'):
            block_size = arg
            if int(block_size) < 8 or int(block_size) > 65535:
                print("block size must be in [8, 65535]")
                print_help()
                sys.exit(2)
        elif opt in ('-l', '--filter_order'):
            filter_order = arg
            if int(filter_order) < 0 or int(filter_order) > 32:
                print("filter order must be in [0,32]")
                print_help()
                sys.exit(2)
        elif opt in ('-d', '--directory'):
            directory = arg
            try:
                os.chdir(directory)
            except OSError as err:
                print(str(err))
                sys.exit(2)


    # Read every file from data directory
    for filename in glob.glob("*.dat"):  
        output_file = filename + "_l" + str(filter_order) + "_b" + str(block_size) + ".out"
        # Run FLAC
        t_start = time.perf_counter()
        subprocess.run(["flac", "--endian=little", "--channels=1", "--bps=16", 
                        "--sample-rate=20e6", "--sign=signed", "-l", str(filter_order), 
                        "-b", str(block_size), "--force-raw-format", "--lax", "-f",
                        filename, "-o", output_file])
        t_end = time.perf_counter()
        # Calculate compression ratio and execution time -- FLAC --
        ratio = float(os.stat(output_file).st_size) / os.stat(filename).st_size
        compress_ratios['FLAC'].append(ratio)
        compress_times['FLAC'].append(t_end - t_start)
        
        # Run Shorten
        t_start = time.perf_counter() 
        subprocess.run(["shorten", "-b", str(block_size), "-c", "1", "-t", "s16",
                        "-p", str(filter_order), filename, output_file])
        t_end = time.perf_counter()
        
        # Calculate compression ratio and execution time -- Shorten --
        ratio = float(os.stat(output_file).st_size) / os.stat(filename).st_size
        compress_ratios['Shorten'].append(ratio)
        compress_times['Shorten'].append(t_end - t_start)
         
        # Run Lempel-Ziv (LZ77)
        t_start = time.perf_counter()
        subprocess.run(["gzip", "-k", "--best", filename])
        t_end = time.perf_counter()

        # Calculate compression ratio and execution time -- Gzip --
        ratio = float(os.stat(filename + ".gz").st_size) / os.stat(filename).st_size
        compress_ratios['LZ77'].append(ratio)
        compress_times['LZ77'].append(t_end - t_start)
        subprocess.run(["rm", "-f", filename + ".gz"])

        # Run LZMA
        t_start = time.perf_counter()
        subprocess.run(["lzma", "-k", "--best", filename])
        t_end = time.perf_counter()

        # Calculate compression ratio and execution time -- Gzip --
        ratio = float(os.stat(filename + ".lzma").st_size) / os.stat(filename).st_size
        compress_ratios['LZMA'].append(ratio)
        compress_times['LZMA'].append(t_end - t_start)
        subprocess.run(["rm", "-f", filename + ".lzma"])
        
        # Split filename to take only 
        split_dash = filename.split('_')
        observation = split_dash[-1].split('.')[0]
        filenames.append(observation)

        # Remove compressed file 
        subprocess.run(["rm", "-f", output_file])

    # Compression Ratio plot for each file
    index = np.arange(len(filenames))
    bar_width = 0.15

    fig1, axes = plt.subplots(1)
    plt.title("Compression Ratio")
    
    flac_label = 'FLAC -b ' + str(block_size) + ' -l ' + str(filter_order)
    shorten_label = 'Shorten -b ' + str(block_size) + ' -l ' + str(filter_order)
    
    plt.bar(index, compress_ratios['FLAC'], bar_width, align='edge', label=flac_label)
    plt.bar(index + bar_width, compress_ratios['Shorten'], bar_width, align='edge', label=shorten_label)
    plt.bar(index + 2*bar_width, compress_ratios['LZ77'], bar_width, align='edge', label='LZ77')
    plt.bar(index + 3*bar_width, compress_ratios['LZMA'], bar_width, align='edge', label='LZMA')

    plt.xlabel('Observation')
    plt.ylabel('Ratio')
    plt.xticks(index + bar_width, filenames, rotation='30')
    plt.legend()
    plt.tight_layout()
    fig1.show()
    name1 = "Ratio_block" + str(block_size) + "_filter" + str(filter_order)
    fig1.savefig(name1 + ".svg", format="svg", dpi=1000)
    save_csv(compress_ratios, filenames, name1 + ".csv")

    # Execution time plot for each file
    fig2, axes = plt.subplots(1)
    plt.title("Execution Time")

    plt.xlabel('File')
    plt.ylabel('Time (s)')
    plt.xticks(index + bar_width, filenames, rotation='30')
    
    plt.bar(index, compress_times['FLAC'], bar_width, align='edge', label=flac_label)
    plt.bar(index + bar_width, compress_times['Shorten'], bar_width, align='edge', label=shorten_label)
    plt.bar(index + 2*bar_width, compress_times['LZ77'], bar_width, label='LZ77', align='edge')
    plt.bar(index + 3*bar_width, compress_times['LZMA'], bar_width, label='LZMA', align='edge')
 
    plt.legend()
    plt.tight_layout()
    fig2.show()
    name2 = "Time_block" + str(block_size) + "_filter" + str(filter_order)
    save_csv(compress_times, filenames, name2 + ".csv")
    fig2.savefig(name2 + ".svg", format="svg", dpi=1000)

    # Average Compression Ratio plot for each algorithm
    fig3 = plt.figure(3)
    fig3.suptitle("Average Compression Ratio")

    plt.xlabel('Algorithms')
    plt.ylabel('Average Ratio')

    means = [np.mean(compress_ratios['FLAC']), np.mean(compress_ratios['Shorten']),
             np.mean(compress_ratios['LZ77']), np.mean(compress_ratios['LZMA'])]
    plt.bar(range(len(compress_ratios)), means, tick_label=list(compress_ratios.keys()))
    plt.tight_layout()
    fig3.show()
    fig3.savefig("MeanRatio_block" + str(block_size) + "_filter" + str(filter_order) + ".svg",
                 format="svg", dpi=1000) 

    # Average Execution Time plot for each algorithm
    fig4 = plt.figure(4)
    fig4.suptitle("Average Execution Time")

    plt.xlabel('Algorithms')
    plt.ylabel('Average Time (s)')

    means = [np.mean(compress_times['FLAC']), np.mean(compress_times['Shorten']),
             np.mean(compress_times['LZ77']), np.mean(compress_times['LZMA'])]
    plt.bar(range(len(compress_times)), means, tick_label=list(compress_times.keys()))
    plt.tight_layout()
    fig4.savefig("MeanTime_block" + str(block_size) + "_filter" + str(filter_order) + ".svg",
                 format="svg", dpi=1000)

    plt.show()
    

def save_csv(d, rows, name):
    I = pd.Index(rows, name='files')
    d1 = pd.DataFrame(d, index=I)
    d1.to_csv(name)

def print_help():
    print("-h, --help\n" +
            "\tPrints this message.")
    print()
    print("-b #, --block_size=#\n" +
            "\tSpecify the block size in samples. Block size must be in " + 
            "range [8, 65536].")
    print()
    print("-l #, --filter_order=#\n" +
            "\tSpecify the LPC order. Filter order must be in range [0, 32].")
    print()
    print("-d dir_name, --directory=dir_name\n" +
            "\tSpecify the directory of the dataset. Default is current directory.")

if __name__ == "__main__":
    main(sys.argv[1:])
