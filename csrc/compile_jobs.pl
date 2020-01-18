#!/usr/bin/env perl

use strict; 
use warnings;

my $combinations = (
    (2,	    1,	30),
    (2,	    2,	30),
	(4,	    1,	60),
	(4,	    2,	60),
	(4,	    4,	60),
	(8,	    1,	80),
	(8,	    2,	80),
	(8,	    4,	80),
);

for (@$combinations) {
    for (@$_) {
        print "Got: $_\n";
    }
}

# my $filename = "sbatch_template";
# my $template = do {
#     local $/ = undef;
#     open my $fh, "<", $filename
#         or die "could not open $filename: $!";
#     <$fh>;
# };
# 
# print "File:n$template";
# print "Done!n";
