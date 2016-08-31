#!/usr/bin/perl -w

# Copyright (C) 2016  SFBTXT Authors

# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA.

use strict;

my $font;
my @lines;
my $char = "none";
my @def = ();
my @bitmaps = ();
my @widths = ();
my @heights = ();

for (my $i = 0; $i < 128; ++$i) {
    $def[$i] = 0;
}

# Read the font from the text file and store each bitmapped character.
while (<>) {
    if (m/^font=([a-zA-Z0-9_]+)/) {
        $font = $1;
    } elsif (m/^=([^\r\n]+)/) {
        process_character();
        $char = $1;
        @lines = ();
    } else {
        chomp;
        push @lines, $_;
    }
}
process_character();

print "// Font: $font\n";

# Print the character bitmaps in order.
for (my $i = 0; $i < 128; ++$i) {
    print "unsigned long long ${font}_bmp_$i\[] = { $bitmaps[$i] };\n";
    print "character_t ${font}_$i = { $widths[$i], $heights[$i], ${font}_bmp_$i };\n";
}
print "character_t *${font}_bmp_set[] = {\n";
for (my $i = 0; $i < 128; ++$i) {
    if ($i == 127) {
        print "  &${font}_$i };\n";
    } else {
        print "  &${font}_$i,\n";
    }
}
print "font_t $font = { \"$font\", ${font}_bmp_set };\n\n";

exit 0;

# Build a bitmap representation of a character.
sub process_character {
    my ($line, $n);
    my @intarray = ();
    my ($w, $h);

    if ($char eq "none") { return; }
    $h = 0;
    foreach $line (@lines) {
        ++$h;
        $n = 0;
        $w = length($line);
        for my $i (0..$w - 1) {
            $n <<= 1;
            if (substr($line, $i, 1) eq "*") { $n += 1; }
        }
        my $str_n = sprintf("0x%XULL", $n);
        push @intarray, $str_n;
    }
    my $bmp = join(", ", @intarray);
    if ($char =~ m/default/) {
        # Default character, usually a box.
        my $i;
        for ($i = 0; $i < 128; ++$i) {
            if ($def[$i] == 0) {
                $widths[$i] = $w;
                $heights[$i] = $h;
                $bitmaps[$i] = "$bmp";
            }
        }
        return;
    }

    if ($char eq "space") { $char = " "; }
    $n = ord $char;
    $bitmaps[$n] = $bmp;
    $widths[$n] = $w;
    $heights[$n] = $h;
    $def[$n] = 1;
}
