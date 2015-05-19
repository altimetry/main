#!/usr/bin/env perl

use strict;
my $LibRE   = '^(?:/[^/]*)*/lib(.+)\.a$';
my $Command = join(" ",@ARGV);
my $Text    = `$Command`;
die if $@;
foreach (split m/\s+/,$Text)
{
  s/$LibRE/-l$1/;
  print $_, " ";
}
# Needed otherwise cmake don't see anything
print "\n";
