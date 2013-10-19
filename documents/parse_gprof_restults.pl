#!/usr/bin/perl
# This script is intended to parse the results from gprof
# into a CSV so that I can grah the results in Open Office

# Andrew J. Musselman
use strict;
use Data::Dumper;

sub parseCallLine{
   my $line = $_[0];
   my $output;
   my @splitLine = split(/\s+/,$line);
   my $name;
   my $nameIdxStart;
   
   $output->{'timePercent'}=$splitLine[1]; 
   $output->{'timeCumSeconds'}=$splitLine[2]; 
   $output->{'timeSelfSeconds'}=$splitLine[3]; 
   if($splitLine[4] =~ /^\d/){
      $output->{'calls'}=$splitLine[4];
      $output->{'selfcallssecond'}=$splitLine[5];
      $output->{'totalcallssecond'}=$splitLine[6];
      $nameIdxStart = 7;
   }
   else{
      $nameIdxStart = 4;
   }
   $name = join(' ',@splitLine[$nameIdxStart..$#splitLine]);
   $output->{'name'}=$name; 

   return $output;
}

my $line;
my $dataHash;
my $parseCallProfile;
my $mile;
my $results;
my $maxMile = 0;


while($line= <STDIN>){
   chomp($line);
   if($line =~ /^Run/){
      if( $line =~ s/\D//g ){
            $mile = $line;
      }
      
   } 

   if($line =~ /^ %/){
      $parseCallProfile = 0;
   }
   if($parseCallProfile == 1){
      $dataHash = parseCallLine($line);
      $results->{$dataHash->{'name'}}[$mile]->{'selfcallssecond'}=$dataHash->{'selfcallssecond'};
      $results->{$dataHash->{'name'}}[$mile]->{'calls'}=$dataHash->{'calls'};
      $results->{$dataHash->{'name'}}[$mile]->{'timeCumSeconds'}=$dataHash->{'timeCumSeconds'};
      $results->{$dataHash->{'name'}}[$mile]->{'totalcallssecond'}=$dataHash->{'totalcallssecond'};
      $results->{$dataHash->{'name'}}[$mile]->{'timePercent'}=$dataHash->{'timePercent'};
      $results->{$dataHash->{'name'}}[$mile]->{'timeSelfSeconds'}=$dataHash->{'timeSelfSeconds'};
   }
   if($line =~ /^ time/){
      $parseCallProfile = 1;
   }

   if($mile > $maxMile){
      $maxMile = $mile;
   }
}

print("\"Radius\"");
print(",");
foreach my $key (keys %$results){
   print("\"$key\"");
   print(",");
}
print("\n");
for(my $i = 1; $i <= $maxMile; $i++){
   print($i);
   print(",");
   foreach my $key (keys %$results){
      print($results->{$key}[$i]->{'timeSelfSeconds'});
      print(",");
   }
   print("\n");
}

#print Dumper($results);
