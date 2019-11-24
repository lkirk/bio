import pysam
import numpy as np


def generate_bins(bam_header, bin_size):
    for ref in bam_header.references:
        ref_len = bam_header.get_reference_length(ref)
        bin_bounds = [(i, i + bin_size) for i in range(0, ref_len, bin_size)]
        if bin_bounds[-1][1] != ref_len:
            bin_bounds.append((bin_bounds[-1][1], ref_len))
        yield ref, bin_bounds


def count_reads_binned(bam_filename, binsize):
    with pysam.AlignmentFile(bam_filename) as bam:
        bins = dict(generate_bins(bam.header, binsize))
        counts = {
            contig: np.zeros(len(bins[contig]), dtype=np.uint64) for contig in bins
        }
        rcg = gen_read_pairs(filter_reads(bam, include=2, exclude=1804))
        f_read, r_read = sorted(next(rcg), key=lambda read: read.is_reverse)
        start, stop = f_read.pos, r_read.reference_end
        # always pick first bin
        curr_bin = start // 50_000
        # proper pair reads, no need to validate
        curr_contig = bam.get_reference_name(f_read.reference_id)
        count = counts[curr_contig][curr_bin]

        for read_pair in rcg:
            # TODO: same as above
            f_read, r_read = sorted(read_pair, key=lambda read: read.is_reverse)
            start, stop = f_read.pos, r_read.reference_end
            count_bin = start // 50_000
            count_contig = bam.get_reference_name(f_read.reference_id)

            if count_bin != curr_bin or count_contig != curr_contig:
                counts[curr_contig][curr_bin] = count
                count = counts[count_contig][count_bin]
                curr_bin = count_bin
                curr_contig = count_contig
            count += 1

        return counts
