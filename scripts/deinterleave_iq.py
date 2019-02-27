##################################################
# iqzip
# Copyright (C) 2019  librespacefoundation / sdrmakerspace
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
##################################################

import os
import glob
import argparse
import subprocess

from gnuradio import blocks
from gnuradio import gr
import pmt

'''
This script deinterleaves a dataset of SatNOGS observations.
The initial structure of an interleaved SatNOGS raw IQ file
consists of 16bit integers representing the real (I) and 
imaginary (Q) part of a complex signal sample in a [I Q I Q ]
form. The resulting observation file after executing this script
has a [I I Q Q] form.

In addition, by setting the appropriate parameter, this script
invokes the execution of the external experimentation script
simple_run.py.
'''


class deinterleave_iq(gr.top_block):

    def __init__(self, file_source='0', isink='0', qsink=0, experiment=False):
        gr.top_block.__init__(self, "Deinterleave IQ dataset")

        ##################################################
        # Parameters
        ##################################################
        self.file_source = file_source
        self.isink = isink
        self.qsink = qsink
        self.experiment = experiment

        ##################################################
        # Variables
        ##################################################
        # self.samp_rate = samp_rate = 1e6

        ##################################################
        # Blocks
        ##################################################
        # self.blocks_throttle_0 = blocks.throttle(gr.sizeof_short*1,
        #
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_short*1,
                                                       file_source, False)
        self.blocks_file_source_0.set_begin_tag(pmt.PMT_NIL)
        self.blocks_file_sink_0 = blocks.file_sink(gr.sizeof_short*1, isink,
                                                   False)
        self.blocks_file_sink_0.set_unbuffered(False)
        self.blocks_file_sink_1 = blocks.file_sink(gr.sizeof_short*1, qsink,
                                                   False)
        self.blocks_file_sink_1.set_unbuffered(False)
        self.blocks_deinterleave_0 = blocks.deinterleave(gr.sizeof_short*1, 1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_deinterleave_0, 0),
                     (self.blocks_file_sink_0, 0))
        self.connect((self.blocks_deinterleave_0, 1),
                     (self.blocks_file_sink_1, 0))
        self.connect((self.blocks_file_source_0, 0),
                     (self.blocks_deinterleave_0, 0))

    def get_isink(self):
        return self.isink

    def set_isink(self, isink):
        self.isink = isink
        self.blocks_file_sink_0.open(self.isink)
    
    def get_qsink(self):
        return self.qsink

    def set_qsink(self, qsink):
        self.qsink = qsink
        self.blocks_file_sink_1.open(self.qsink)

    def get_experiment(self):
        return self.experiment

    def set_experiment(self, experiment):
        self.experiment = experiment

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_0.set_sample_rate(self.samp_rate/8)


def argument_parser():
    parser = argparse.ArgumentParser(prog='deinterleave.py',
                                     formatter_class=argparse
                                     .ArgumentDefaultsHelpFormatter)
    parser.add_argument('-s', '--source_dir',
                        dest='source_dir',
                        default='.',
                        help='The source dataset directory',
                        required=False)
    parser.add_argument('-d', '--destination_dir',
                        dest='dest_dir',
                        default='.',
                        help='The destination dataset directory',
                        required=False)
    parser.add_argument('-e', '--experiment', dest='experiment',
                        default=False, action='store_true',
                        help='Run the experimentation script',
                        required=False)
    parser.add_argument('-b', '--block-size', dest='block_size',
                        type=str,
                        default=32,
                        help='Set block size for the experimentation script.\
                            Block size must be in range [8, 65536]',
                        required=False)
    parser.add_argument('-l', '--filter-order', dest='filter_order',
                        type=str,
                        default=1,
                        help='Specify the LPC order for the experimentation script.\
                             Filter order must be in range [0, 32]',
                        required=False)
    return parser


def main(top_block_cls=deinterleave_iq, args=None):
    if args is None:
        args = argument_parser().parse_args()
    # Read every file from data directory
    for fn in glob.glob(args.source_dir.rstrip('/') + '/' + "*.dat"):
        fnprefix, fnsuffix = os.path.splitext(fn)
        fnprefix = os.path.basename(fn)
        fnprefix, fnsuffix = os.path.splitext(fnprefix)
        fisink = args.dest_dir.rstrip('/') + '/' + fnprefix + '_I' + fnsuffix
        fqsink = args.dest_dir.rstrip('/') + '/' + fnprefix + '_Q' + fnsuffix
        fdsink = args.dest_dir.rstrip('/') + '/' + fnprefix + 'D' + fnsuffix
        tb = top_block_cls(file_source=fn, isink=fisink, qsink=fqsink)
        tb.start()
        tb.wait()
        subprocess.call(['touch', fdsink])
        subprocess.call(['cat ' + fisink + ' ' + fqsink + ' >> ' + fdsink],
                        shell=True)
        subprocess.call(['rm', fisink])
        subprocess.call(['rm', fqsink])
    if args.experiment:
        try:
            subprocess.call(['python3', 'simple_run.py',
                             '-b', str(args.block_size), '-l',
                             str(args.filter_order), '-d',
                             args.dest_dir],
                            cwd=os.getcwd())
        except subprocess.CalledProcessError as e:
            print(e.output)


if __name__ == '__main__':
    main()
