import argparse
from datetime import datetime
import pysam
import logging

import numpy as np

from bio.align import filter_reads, gen_read_pairs


def generate_bins(bam, bin_size):
    for ref in bam.header.references:
        ref_len = bam.header.get_reference_length(ref)
        bin_bounds = [(i, i + bin_size) for i in range(0, ref_len, bin_size)]
        if bin_bounds[-1][1] != ref_len:
            bin_bounds.append((ref_len, bin_bounds[-1][1]))
        yield ref, bin_bounds


def count_reads_binned(bam_filename, binsize):
    with pysam.AlignmentFile(bam_filename) as bam:
        bins = dict(generate_bins(bam, binsize))
        counts = {
            contig: np.zeros(len(bins[contig]), dtype=np.uint64) for contig in bins
        }
        rcg = gen_read_pairs(filter_reads(bam, include=2, exclude=1804))
        f_read, r_read = sorted(next(rcg), key=lambda read: read.is_reverse)
        start, stop = f_read.pos, r_read.reference_end
        # always pick first bin
        curr_bin = start // binsize
        # proper pair reads, no need to validate
        curr_contig = bam.get_reference_name(f_read.reference_id)
        count = counts[curr_contig][curr_bin]

        for read_pair in rcg:
            # TODO: same as above
            f_read, r_read = sorted(read_pair, key=lambda read: read.is_reverse)
            start, stop = f_read.pos, r_read.reference_end
            count_bin = start // binsize
            count_contig = bam.get_reference_name(f_read.reference_id)

            if count_bin != curr_bin or count_contig != curr_contig:
                counts[curr_contig][curr_bin] = count
                count = counts[count_contig][count_bin]
                curr_bin = count_bin
                curr_contig = count_contig
            count += 1

        return counts


def _parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--in-bam", "-i", required=True, help="input bam file to count reads on"
    )
    parser.add_argument("--out-npz", "-o", required=True, help="npz to write to")
    parser.add_argument(
        "--bin-size", "-b", required=True, type=int, help="bin size to use"
    )

    return parser.parse_args()


def main():
    logging.basicConfig(
        level=logging.INFO, format="%(asctime)s %(name)-12s %(levelname)-8s %(message)s"
    )
    args = _parse_args()

    logging.info(f"counting reads for {args.in_bam}")
    start_time = datetime.now()

    readcounts = count_reads_binned(args.in_bam, args.bin_size)

    logging.info(
        f"counting took {start_time - datetime.now()}"
    )

    np.savez(args.out_npz, **readcounts)
    logging.info(f"wrote {args.out_npz}")
