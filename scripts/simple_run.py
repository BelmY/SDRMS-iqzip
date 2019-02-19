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
This script runs FLAC and Shorten on all files in directory at relative path ../data.
Other compression algorithms will be added in later versions.
Block size and LPC filter order can be passed as command line arguments. Print help
for more information.
Default block size for both algorithms 8192 samples and filter size is 12 coefficients.
Compression ratio is measured as: 
    compressed_size / original_size
measured with system commands as output of each algorithm may differ.
The compressed output files are deleted after we have measured the compression ratio.
The script saves 4 plots in the data directory:
    1) Compressed Ration for each file
    2) Execution Time for each file
    3) Average Compression Ratio for each algorithm
    3) Average Execution Time for each algorithm
'''
import os
import glob
import subprocess
import time
import numpy as np
import matplotlib.pyplot as plt
import sys
import getopt

block_values = (192, 576, 1152, 2304, 4608, 256, 512, 1024, 2048,
                4096, 8192, 16384)

def main(argv):
    # Change directory to data directory
    os.chdir("../data")
    # Arrays to save measurements
    flac_ratios = []
    flac_times = []
    shorten_ratios =[]
    shorten_times = []
    filenames = []
    block_size = 8192
    filter_order = 12

    #Initialize options
    try:
        opts, args = getopt.getopt(argv, "b:l:h", ["block_size=", "filter_order=","help"])
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
            if block_size not in block_values:
                print("block size must be one of:")
                print(block_values)
                print_help()
                sys.exit(2)
        elif opt in ('-l', '--filter_order'):
            filter_order = arg
            if filter_order < 0 or filter_order > 32:
                print("filter order must be in [0,32]")
                print_help()
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
        # Calculate Compression ratio and execution time -- FLAC --
        ratio = float(os.stat(output_file).st_size) / os.stat(filename).st_size
        flac_ratios.append(ratio)
        flac_times.append(t_end - t_start)
        
        # Run Shorten
        t_start = time.perf_counter() 
        subprocess.run(["shorten", "-b", str(block_size), "-c", "1", "-t", "s16",
                        "-p", "12", filename, output_file])
        t_end = time.perf_counter()
        
        # Calculate Compression ratio and execution time -- Shorten --
        ratio = float(os.stat(output_file).st_size) / os.stat(filename).st_size
        shorten_ratios.append(ratio)
        shorten_times.append(t_end - t_start)
        
        filenames.append(filename)

        # Remove compressed file 
        subprocess.run(["rm", "-f", output_file])

    # Compression Ratio plot for each file
    fig1 = plt.figure(1)
    fig1.suptitle("Compression Ratio")
    plt.title('Block size = ' + str(block_size) + ' Filter Order = ' + str(filter_order))

    plt.xlabel('File')
    plt.ylabel('Ratio')
    plt.xticks(range(1, len(filenames)), filenames, rotation='60')
    axes = plt.gca()
    axes.set_ylim([0.4, 1])

    plt.plot(filenames, flac_ratios, label='FLAC' )
    plt.plot(filenames, shorten_ratios, label='Shorten')
    plt.legend()

    fig1.show()
    fig1.savefig("Ratio_block" + str(block_size) + "_filter" + str(filter_order) + ".png", bbox_inches = "tight")

    # Execution time plot for each file
    fig2 = plt.figure(2)
    fig2.suptitle("Execution Time")

    plt.xlabel('File')
    plt.ylabel('Time (s)')
    plt.xticks(range(1, len(filenames)), filenames, rotation='60')

    plt.plot(filenames, flac_times, label='FLAC' )
    plt.plot(filenames, shorten_times, label='Shorten')
    plt.legend()

    fig2.show()
    fig2.savefig("Time_block" + str(block_size) + "_filter" + str(filter_order) + ".png", bbox_inches = "tight")

    # Average Compression Ratio plot for each algorithm
    fig3 = plt.figure(3)
    fig3.suptitle("Average Compression Ratio")

    plt.xlabel('Algorithms')
    plt.ylabel('Average Ratio')

    means = [np.mean(flac_ratios), np.mean(shorten_ratios)]
    plt.bar([1,2], means, tick_label=['FLAC', 'Shorten'])

    fig3.show()
    fig3.savefig("MeanRatio_block" + str(block_size) + "_filter" + str(filter_order) + ".png") 

    # Average Execution Time plot for each algorithm
    fig4 = plt.figure(4)
    fig4.suptitle("Average Execution Time")

    plt.xlabel('Algorithms')
    plt.ylabel('Average Time (s)')

    means = [np.mean(flac_times), np.mean(shorten_times)]
    plt.bar([1,2], means, tick_label=['FLAC', 'Shorten'])

    plt.show()
    fig4.savefig("MeanTime_block" + str(block_size) + "_filter" + str(filter_order) + ".png")

def print_help():
    print("-h, --help\n" +
            "\tPrints this message.")
    print()
    print("-b #, --block_size=#\n" +
            "\tSpecify the block size in samples. Block size must be in " + 
            str(block_values) + ".")
    print()
    print("-l #, --filter_order=#\n" +
            "\tSpecify the LPC order. Filter order must be in range [0, 32]")

if __name__ == "__main__":
    main(sys.argv[1:])
