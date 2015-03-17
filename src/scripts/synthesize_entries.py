#!/usr/bin/python

# Usage: scripts/synthesize_entries.py \
#          third_party/effective_tld_names/effective_tld_names.dat
#
# Author: Jeff Kaufman (jefftk@google.com)
#
# There are two unusual cases that the C code for the domain registry provider
# can't handle properly:
#
# 1. list contains both *.c and a.b.c
# 2. list contains a.b.c.d and c.d but not b.c.d
#
# We can fix this by synthesizing entries for b.c in case 1 and b.c.d in case
# 2.  The first fix is completely ok, just redundant, but the second case means
# categorizing foo.b.c.d as being under public suffic b.c.d instead of the
# technically correct c.d.  This is rare and doesn't seem to be a problem for
# existing entries (like amazonaws.com), so for now we'll just work around the
# broken code with slightly messy data.
#
# See https://code.google.com/p/domain-registry-provider/issues/detail?id=3 for
# more discussion.

import sys
from collections import defaultdict

def start(effective_tld_names_fname):
    entries = set()
    with open(effective_tld_names_fname) as inf:
        for line in inf:
            line = line.split("//")[0].strip()  # remove comments and whitespace
            if not line:
                continue
            entries.add(line)

    # Allow multiple comments per entry, in case we want the same entry multiple
    # times for different reasons.
    new_entries = defaultdict(list) # {new_entry: [comments], ...]

    for entry in entries:
        if entry.count('.') >= 2:
            leaf, candidate_entry = entry.split(".", 1)
            if leaf.startswith("!") or leaf == "*":
                continue
            
            _, parent = candidate_entry.split(".", 1)
            if candidate_entry in entries:
                continue
            star_parent = "*.%s" % parent

            if star_parent in entries or parent in entries:
                # If entry is 'a.b.c' and '*.c' is also an entry, we need to
                # synthesize a new entry 'b.c'.
                #
                # If entry is a.b.c and c is also an entry but b.c is not, we
                # need to synthesize a new entry b.c.  This one isn't
                # technically legal, but it's better than getting it completely
                # wrong which is what the C code would otherwise do with this
                # case.
                new_entries[candidate_entry].append("For %s" % entry)

    if not new_entries:
        return
                
    with open(effective_tld_names_fname, "a") as outf:
        category = "DOMAIN REGISTRY PROVIDER SYNTHESIZED DOMAINS"
        def add_comment(s):
            outf.write("// %s\n" % s)
        add_comment("===BEGIN %s===" % category)
        add_comment("synthesized by scripts/synthesize_entries.py")
        for new_entry in sorted(new_entries):
            for comment in sorted(new_entries[new_entry]):
                add_comment(comment)
            outf.write("%s\n" % new_entry)
        add_comment("===END %s===" % category)
if __name__ == "__main__":
    start(*sys.argv[1:])
