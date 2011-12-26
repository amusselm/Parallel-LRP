#!/usr/bin/perl
# This script is intended to parse the raw output from time(1) into a CSV.
# Specifically it's intended to parse the time output from my benchmark 
# shell script.

# Andrew J. Musselman
use strict;
use Data::Dumper;

my $line;
my $dataHash;

while($line = <STDIN>) {
   chomp($line);
   if($line =~ /^Run/){
      if( $line =~ /[0-9]{1,3}/ ){
         $dataHash->{'mile'} = $1;
      }
   }
   else{
      die("Invalid Format");
   }
   #Read the blank line...
   $line=<STDIN>;
   #Read wall (real) time...
   $line=<STDIN>;
   if($line =~ /^real/){
      if( $line =~ /\w$/ ){
         $dataHash->{'real'} = $1;
      }
   }
   else{
      die("Invalid Format: couldn't read real");
   }
   #Read proscess (user) time...
   $line=<STDIN>;
   if($line =~ /^user/){
      if( $line =~ /\w/ ){
         $dataHash->{'user'} = $1;
      }
   }
   else{
      die("Invalid Format");
   }
   #Read sys time...
   $line=<STDIN>;
   if($line =~ /^sys/){
      if( $line =~ /\w/ ){
         $dataHash->{'sys'} = $1;
      }
   }
   else{
      die("Invalid Format");
   }
   print Dumper($dataHash);
   print("$dataHash->{'mile'},$dataHash->{'real'},$dataHash->{'user'}\n");
}
