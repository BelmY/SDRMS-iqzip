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
unlsess another one is specified with -d argument. LPC filter oredr values take
all values in [1, 32]. Block size can be passed as command line argument.
Default value is 8192.
Print help for more information.
Compression ratio is measured as: 
    compressed_size / original_size
measured with system commands as output of each algorithm may differ.
The compressed output files are deleted after we have measured the compression ratio.
The script saves 4 plots in the data directory:
    1) Average Compression Ratio for each algorithm per filter order
    2) Average Execution Time for each algorithm per filter order
    3) Compression Ratio for each algorithm and block size
    4) Execution Time for each algorithm and block size
Also 2 csv files are generated:
    1) Compressed Ratio for each file
        * Columns are filter order
        * Rows are each observation
    2) Execution Time for each file
        * Columns are each filter order
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
    # Arrays to save measurements
    flac_ratios = dict()
    flac_times = dict()
    shorten_ratios = dict()
    shorten_times = dict()
    filenames = []
    block_size = 8192
    filters = list(range(1,33))
    flac_ratios = {k: [] for k in filters}
    flac_times = {k: [] for k in filters}
    shorten_ratios = {k: [] for k in filters}
    shorten_times = {k: [] for k in filters}

    #Initialize options
    try:
        opts, args = getopt.getopt(argv, "b:d:h", ["block_size=", "directory=", "help"])
    except getopt.GetoptError as err:
        print("Invalid arguments: " + str(err))
        print_help()
        sys.exit(2)
    for opt, arg in opts:
        if opt in ('-h', '--help'):
            print_help()
            sys.exit(0)
        elif opt in ('-b', '--block_size'):
            filter_order = arg
            if block_size < 16 or block_size > 65535:
                print("block size must be in [16, 65535]")
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
        for filter_order in range(1,33):
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
            flac_ratios[filter_order].append(ratio)
            flac_times[filter_order].append(t_end - t_start)
            
            # Run Shorten
            t_start = time.perf_counter() 
            subprocess.run(["shorten", "-b", str(block_size), "-c", "1", "-t", "s16",
                            "-p", str(filter_order), filename, output_file])
            t_end = time.perf_counter()
            
            # Calculate compression ratio and execution time -- Shorten --
            ratio = float(os.stat(output_file).st_size) / os.stat(filename).st_size
            shorten_ratios[filter_order].append(ratio)
            shorten_times[filter_order].append(t_end - t_start)
           
            # Remove compressed file 
            subprocess.run(["rm", "-f", output_file])

        # Split filename to take only 
        split_dash = filename.split('_')
        observation = split_dash[-1].split('.')[0]
        filenames.append(observation)
    
    # Save csv files
    name1 = "Flac_ratio_all_filters_block_" + str(block_size)
    save_csv(flac_ratios, filenames, name1 + ".csv")
    name2 = "Shorten_ratio_all_filters_block_" + str(block_size)
    save_csv(shorten_ratios, filenames, name2 + ".csv")
    name3 = "Flac_time_all_filters_block_" + str(block_size)
    save_csv(flac_times, filenames, name3 + ".csv")
    name4 = "Shorten_time_all_filters_block_" + str(block_size)
    save_csv(shorten_times, filenames, name4 + ".csv")
    
    # Compression Ratio plot for each file
    index = np.arange(1,33)
    bar_width = 0.25


    fig1, axes = plt.subplots(1)
    #fig1.suptitle("Compression Ratio per Filter Order")
    plt.title("Compression Ratio per Filter Order\n" + 
              "Block Size " + str(block_size))

    # Flac boxplot
    labels, data = [*zip(*flac_ratios.items())]
    bp = plt.boxplot(data, positions=index, widths=bar_width)
    set_box_color(bp, 'blue')
    # Shorten boxplot
    labels, data = [*zip(*shorten_ratios.items())]
    index2 = [x+bar_width+0.25 for x in index]
    bp = plt.boxplot(data, positions=index, widths=bar_width)
    set_box_color(bp, 'orange')

    plt.ylabel('Ratio')
    plt.xticks(range(1, len(labels) + 1), labels, rotation='30')
    # draw temporary red and blue lines and use them to create a legend
    plt.plot([], c='blue', label='Flac')
    plt.plot([], c='orange', label='Shorten')
    plt.legend()
    plt.tight_layout()
        
    fig1.savefig("Flac_Shorten_ratio_box_all_filters_block" + str(block_size) + ".svg", 
                 bbox_inches = "tight", format="svg", dpi=1000)

    # Execution time plot for each file
    fig2, axes = plt.subplots(1)
    #fig2.suptitle("Execution Time per Block size")
    plt.title("Execution Time per Block size\n" +
              "Block Size " + str(block_size))
    
    # Flac boxplot
    labels, data = [*zip(*flac_times.items())]
    bp = plt.boxplot(data, positions=index, widths=bar_width)
    set_box_color(bp, 'blue')
    # Shorten boxplot
    labels, data = [*zip(*shorten_times.items())]
    index2 = [x+bar_width+0.25 for x in index]
    bp = plt.boxplot(data, positions=index, widths=bar_width)
    set_box_color(bp, 'orange')

    plt.ylabel('Time (s)')
    plt.xticks(range(1, len(labels) + 1), labels, rotation='30')
    # draw temporary red and blue lines and use them to create a legend
    plt.plot([], c='blue', label='Flac')
    plt.plot([], c='orange', label='Shorten')
    plt.legend()
    plt.tight_layout()
   
    fig2.savefig("Flac_Shorten_time_box_all_filters_block" + str(block_size) + ".svg", 
                 bbox_inches = "tight", format="svg", dpi=1000)
    
    # Average Ratio barplot
    bar_width = 0.25

    fig3 = plt.figure(3)
    #fig3.suptitle("Average Compression Ratio per Filter")
    plt.title("Average Compression Ratio per Filter\n" + 
              "Block Size " + str(block_size))

    flac_label = 'FLAC -b ' + str(block_size)
    shorten_label = 'Shorten -b ' + str(block_size)

    flac_means = [np.mean(flac_ratios[k]) for k in flac_ratios.keys()]
    shorten_means = [np.mean(shorten_ratios[k]) for k in shorten_ratios.keys()]
    
    plt.bar(index, flac_means, bar_width, align='edge', label=flac_label)
    plt.bar(index + bar_width, shorten_means, bar_width, align='edge', label=shorten_label)
    
    plt.xlabel('Filter Size')
    plt.ylabel('Average Ratio')
    plt.xticks(index + bar_width, index, rotation='60')
    plt.legend()
    plt.tight_layout()

    fig3.savefig("Flac_Shorten_average_ratio_all_filters_block_" + str(block_size) + ".svg",
                 format="svg", dpi=1000)
    fig3.show()
    # Average Time barplot
    bar_width = 0.25

    fig4 = plt.figure(4)
    #fig4.suptitle("Average Execution Time per Block")
    plt.title("Average Execution Time per Block\n" +
              "Block Size " + str(block_size))

    flac_label = 'FLAC -l ' + str(filter_order)
    shorten_label = 'Shorten -l ' + str(filter_order)

    flac_means = [np.mean(flac_times[k]) for k in flac_times.keys()]
    shorten_means = [np.mean(shorten_times[k]) for k in shorten_times.keys()]
    
    plt.bar(index, flac_means, bar_width, align='edge', label=flac_label)
    plt.bar(index + bar_width, shorten_means, bar_width, align='edge', label=shorten_label)

    plt.xlabel('Filter Order')
    plt.ylabel('Average Time (s)')
    plt.xticks(index + bar_width, index, rotation='60')
    plt.legend()
    plt.tight_layout()

    fig4.savefig("Flac_Shorten_average_time_all_filters_block_" + str(block_size) + ".svg",
                 format="svg", dpi=1000)
    plt.show()


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
    print("-b #, --block_size=#\n" +
            "\tSpecify the block size in samples. Block size must be in " + 
            "range [16, 65535].")
    print()
    print("-d dir_name, --directory=dir_name\n" +
            "\tSpecify the directory of the dataset. Default is current directory.")

if __name__ == "__main__":
    main(sys.argv[1:])
