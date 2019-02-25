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
This script runs FLAC and Shorten on all files named *.dat in current directory
unlsess another one is specified with -d argument. Block size values that are 
power of 2 in [16, 65535] are passed to both algorithms. 
LPC filter order can be passed as command line argument. Default value is 1.
Print help for more information.
Compression ratio is measured as: 
    compressed_size / original_size
measured with system commands as output of each algorithm may differ.
The compressed output files are deleted after we have measured the compression ratio.
The script saves 4 plots in the data directory:
    1) Average Compression Ratio for each algorithm and block size
    2) Average Execution Time for each algorithm and block size
    3) Compression Ratio for each algorithm and block size
    4) Execution Time for each algorithm and block size
Also 2 csv files are generated:
    1) Compressed Ratio for each file
        * Columns are block sizes
        * Rows are each observation
    2) Execution Time for each file
        * Columns are each block sizes
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
import pandas as pd

def main(argv):
    # Dictionaries to save measurements
    flac_ratios = dict()
    flac_times = dict()
    shorten_ratios = dict()
    shorten_times = dict()
    filenames = []
    filter_order = 1
    # Generate array with block sizes [16,65535]
    block_sizes = [2**i for i in range(4,17)]
    block_sizes[12] = block_sizes[12] - 1
    # Initialize dictionaries with block sizes as keys
    flac_ratios = {k: [] for k in block_sizes}
    flac_times = {k: [] for k in block_sizes}
    shorten_ratios = {k: [] for k in block_sizes}
    shorten_times = {k: [] for k in block_sizes}

    #Initialize options
    try:
        opts, args = getopt.getopt(argv, "l:d:h", ["filter_order=", "directory=", "help"])
    except getopt.GetoptError as err:
        print("Invalid arguments: " + str(err))
        print_help()
        sys.exit(2)
    for opt, arg in opts:
        if opt in ('-h', '--help'):
            print_help()
            sys.exit(0)
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
        for block_size in block_sizes:
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
            flac_ratios[block_size].append(ratio)
            flac_times[block_size].append(t_end - t_start)
            
            # Run Shorten
            t_start = time.perf_counter() 
            subprocess.run(["shorten", "-b", str(block_size), "-c", "1", "-t", "s16",
                            "-p", str(filter_order), filename, output_file])
            t_end = time.perf_counter()
            
            # Calculate compression ratio and execution time -- Shorten --
            ratio = float(os.stat(output_file).st_size) / os.stat(filename).st_size
            shorten_ratios[block_size].append(ratio)
            shorten_times[block_size].append(t_end - t_start)
            
            # Remove compressed file 
            subprocess.run(["rm", "-f", output_file])
            
        # Split filename to take only 
        split_dash = filename.split('_')
        observation = split_dash[-1].split('.')[0]
        filenames.append(observation)

    # Save csvs
    name1 = "Flac_ratio_all_blocks_filter_" + str(filter_order)
    save_csv(flac_ratios, filenames, name1 + ".csv")
    name2 = "Shorten_ratio_all_blocks_filter_" + str(filter_order)
    save_csv(shorten_ratios, filenames, name2 + ".csv")
    name3 = "Flac_time_all_blocks_filter_" + str(filter_order)
    save_csv(flac_times, filenames, name3 + ".csv")
    name4 = "Shorten_time_all_blocks_filter_" + str(filter_order)
    save_csv(shorten_times, filenames, name4 + ".csv")
    
    # Compression Ratio boxplot for each file
    bar_width = 0.25
    index = list(range(1, len(block_sizes) + 1))
    index2 = [x+bar_width+0.25 for x in index]

    fig1, axes = plt.subplots(1)
    #fig1.suptitle("Compression Ratio per Block Size")
    plt.title("Compression Ratio per Block Size\n" +
              "Filter Order " + str(filter_order))
    
    # Flac ratio boxplot
    labels, data = [*zip(*flac_ratios.items())]
    bp = plt.boxplot(data, positions=index, widths=bar_width)
    set_box_color(bp, 'blue')
    # Shorten ratio boxplot
    labels, data = [*zip(*shorten_ratios.items())]
    index2 = [x+bar_width+0.25 for x in index]
    bp = plt.boxplot(data, positions=index, widths=bar_width)
    set_box_color(bp, 'orange')

    plt.ylabel('Ratio')
    plt.xticks(range(1, len(labels) + 1), labels)
    # draw temporary red and blue lines and use them to create a legend
    plt.plot([], c='blue', label='Flac')
    plt.plot([], c='orange', label='Shorten')
    plt.legend()
    plt.tight_layout()        
    #fig1.show()
    fig1.savefig("Flac_Shorten_ratio_box_all_blocks_filter_" + str(filter_order) + ".svg", 
                bbox_inches = "tight", format="svg", dpi=1000)

    # Execution time plot for each file
    fig2, axes = plt.subplots(1)
    #fig2.suptitle("Execution Time per Block size")
    plt.title("Execution Time per Block size\n" +
              "Filter Order " + str(filter_order))
    
    # Flac time boxplot
    labels, data = [*zip(*flac_times.items())]
    bp = plt.boxplot(data, positions=index, widths=bar_width)
    set_box_color(bp, 'blue')
    
    # Shorten time boxplot
    labels, data = [*zip(*shorten_times.items())]
    bp = plt.boxplot(data, positions=index, widths=bar_width)
    set_box_color(bp, 'orange')

    plt.ylabel('Time (s)')
    plt.xticks(range(1, len(labels) + 1), labels)
    # draw temporary red and blue lines and use them to create a legend
    plt.plot([], c='blue', label='Flac')
    plt.plot([], c='orange', label='Shorten')
    plt.legend()
    
    plt.tight_layout()        
    fig2.savefig("Flac_Shorten_time_box_all_blocks_filter_" + str(filter_order) + ".svg", 
                 bbox_inches = "tight", format="svg", dpi=1000)
    #fig2.show()

    # Average Ratio barplot
    index = np.arange(len(block_sizes))
    bar_width = 0.25

    fig3 = plt.figure(3)
    #fig3.suptitle("Average Compression Ratio per Block")
    plt.title("Average Compression Ratio per Block\n" +
              "Filter Order " + str(filter_order))

    flac_label = 'FLAC -l ' + str(filter_order)
    shorten_label = 'Shorten -l ' + str(filter_order)

    flac_means = [np.mean(flac_ratios[k]) for k in flac_ratios.keys()]
    shorten_means = [np.mean(shorten_ratios[k]) for k in shorten_ratios.keys()]
    
    plt.bar(index, flac_means, bar_width, align='edge', label=flac_label)
    plt.bar(index + bar_width, shorten_means, bar_width, align='edge', label=shorten_label)
    
    plt.xlabel('Block Size')
    plt.ylabel('Average Ratio')
    plt.xticks(index + bar_width, block_sizes, rotation='60')
    plt.legend()

    plt.tight_layout()        
    fig3.savefig("Flac_Shorten_average_ratio_all_blocks_filter_" + str(filter_order) + ".svg",
                 format="svg", dpi=1000)
    #fig3.show()
    
    # Average Time barplot
    fig4 = plt.figure(4)
    #fig4.suptitle("Average Execution Time per Block")
    plt.title("Average Execution Time per Block\n" +
              "Filter Order " + str(filter_order))

    flac_label = 'FLAC -l ' + str(filter_order)
    shorten_label = 'Shorten -l ' + str(filter_order)

    flac_means = [np.mean(flac_times[k]) for k in flac_times.keys()]
    shorten_means = [np.mean(shorten_times[k]) for k in shorten_times.keys()]
    
    plt.bar(index, flac_means, bar_width, align='edge', label=flac_label)
    plt.bar(index + bar_width, shorten_means, bar_width, align='edge', label=shorten_label)
    
    plt.xlabel('Block Size')
    plt.ylabel('Average Time (s)')
    plt.xticks(index + bar_width, block_sizes, rotation='60')
    plt.legend()

    plt.tight_layout()        
    fig4.savefig("Flac_Shorten_average_time_all_blocks_filter_" + str(filter_order) + ".svg",
                 format="svg", dpi=1000)
    #plt.show()

def set_box_color(bp, color):
    plt.setp(bp['boxes'], color=color)
    plt.setp(bp['whiskers'], color=color)
    plt.setp(bp['caps'], color=color)
    plt.setp(bp['medians'], color=color)
    plt.setp(bp['fliers'], markerfacecolor=color)
    plt.setp(bp['means'], color=color)

def save_csv(d, rows, name):
    I = pd.Index(rows, name='files')
    d1 = pd.DataFrame(d, index=I)
    d1.to_csv(name)

def print_help():
    print("-h, --help\n" +
            "\tPrints this message.")
    print()
    print("-l #, --filter_order=#\n" +
            "\tSpecify the LPC order. Filter order must be in range [0, 32].")
    print()
    print("-d dir_name, --directory=dir_name\n" +
            "\tSpecify the directory of the dataset. Default is current directory.")

if __name__ == "__main__":
    main(sys.argv[1:])
