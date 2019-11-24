"""
Utilities for working with pysam alignments
"""


def filter_reads(bam, include=0, exclude=0):
    for read in bam:
        flag = read.flag
        if flag & include == include and flag & exclude == 0:
            yield read


def gen_read_pairs(bam):
    pairs = dict()
    for read in bam:
        qname = read.qname
        if qname in pairs:
            if read.is_read1:
                yield read, pairs.pop(qname)
            elif read.is_read2:
                yield pairs.pop(qname), read
        else:
            pairs[qname] = read
