#!/usr/bin/perl
# This script is intended to parse the results from gprof
# into a CSV so that I can grah the results in Open Office

# Andrew J. Musselman
use strict;
use Data::Dumper;

my $line;
my $dataHash;


while($line= <STDIN>){
   chomp($line);
   if($line =~ /^Run/){
      if( $line =~ s/\D//g ){
            $dataHash->{'mile'} = $line;
      }
      
   }


   print Dumper($dataHash);
}
