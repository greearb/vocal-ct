#!/usr/bin/perl

# ====================================================================
# The Vovida Software License, Version 1.0 
# 
# Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 
# 3. The names "VOCAL", "Vovida Open Communication Application Library",
#    and "Vovida Open Communication Application Library (VOCAL)" must
#    not be used to endorse or promote products derived from this
#    software without prior written permission. For written
#    permission, please contact vocal\@vovida.org.
# 
# 4. Products derived from this software may not be called "VOCAL", nor
#    may "VOCAL" appear in their name, without prior written
#    permission of Vovida Networks, Inc.
# 
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
# NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
# NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
# IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
# DAMAGE.
# 
# ====================================================================
# 
# This software consists of voluntary contributions made by Vovida
# Networks, Inc. and many individuals on behalf of Vovida Networks,
# Inc.  For more information on Vovida Networks, Inc., please see
# <http://www.vovida.org/>.

# build signed linear tone files

# usage:  tone.pl frequency duration

# where frequency is in hertz and duration is in seconds

# this program produces 8Khz samples 8 bit signed samples as its output

# e.g. to convert an output file from this program via sox:

# to convert from raw to wav:
# tone.pl 512 1 > sample.raw 
# sox -traw -r8000 -s -b sample.raw -t wav sample.wav

$freq = $ARGV[0];

$duration = $ARGV[1];

$loudness = $ARGV[2];

print STDERR "frequency: $freq\n";
print STDERR "duration: $duration seconds\n";
print STDERR "loudness: $loudness\n";

$rate = 8000 ;  # 8000 samples / sec for this s/w

$num_samples = $rate * $duration;


# frequency = cycles / sec, and duration is in seconds -- so the # of
# samples is rate * duration.

# the number of cycles during that time is freq * duration

$PI = 3.14159265;
$TWO_PI = 2.0 * $PI;
$max = 127;

$amplitude = $max * $loudness;

print STDERR "$x, $num_samples\n";


for($x = 0.0 ; $x < $num_samples ; $x++) {
    $place = ($x / $rate * $TWO_PI * $freq);
    $y = sin($place);
    $new_y = int($y * $amplitude + .5);
    print pack ("c", $new_y);
}
