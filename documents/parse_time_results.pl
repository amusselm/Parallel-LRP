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
      if( $line =~ s/\D//g ){
         $dataHash->{'mile'} = $line;
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
      if( $line =~ m/[0-9]+m[0-9\.]+s$/ ){
         #Remove the "real" header
         $line =~ s/real//g;
         #Convert the time into something open office can read
         $line =~ s/m/:/g;
         $line =~ s/s//g;
         #Remove whitespace
         $line =~ s/\s//g;
         $dataHash->{'real'} = $line;
      }
   }
   else{
      die("Invalid Format: couldn't read real");
   }
   #Read proscess (user) time...
   $line=<STDIN>;
   if($line =~ /^user/){
      if( $line =~ m/[0-9]+m[0-9\.]+s$/ ){
         #Remove the "real" header
         $line =~ s/user//g;
         #Convert the time into something open office can read
         $line =~ s/m/:/g;
         $line =~ s/s//g;
         #Remove whitespace
         $line =~ s/\s//g;
         $dataHash->{'user'} = $line;
      }
   }
   else{
      die("Invalid Format: couldn't read user");
   }
   #Read sys time...
   $line=<STDIN>;
   if($line =~ /^sys/){
      if( $line =~ m/[0-9]+m[0-9\.]+s$/ ){
         #Remove the "real" header
         $line =~ s/sys//g;
         #Convert the time into something open office can read
         $line =~ s/m/:/g;
         $line =~ s/s//g;
         #Remove whitespace
         $line =~ s/\s//g;
         $dataHash->{'sys'} = $line;
      }
   }
   else{
      die("Invalid Format");
   }
   #print Dumper($dataHash);
   print("$dataHash->{'mile'},$dataHash->{'real'},$dataHash->{'user'}\n");
}
