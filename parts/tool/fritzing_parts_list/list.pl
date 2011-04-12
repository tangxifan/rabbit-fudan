#!/usr/bin/perl -w
#:Mike Zou <zxhmike@gmail.com>
#List the Fritzing modules and their module files.
#Format: <Module Name>*<Module File>
#
#Copyright (C) 2011
#
#Rabbit Project of Fudan University
#
#This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
#
#This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>. / 

use strict;

if (-1 == $#ARGV){
	die "Usage: list FOLDER_TO_LIST\n";
}

#Prepare the files input to parse
my @files = <$ARGV[0]/*.fzp>;
my $count;

#Prepare the ourput file
my $OUT;
open (OUT, ">parts.lst")
	or die "Cannot create the output file \"parts.lst\"";

#Parse every file
my ($FILE, $full_path, $filename);

for (@files){
	$full_path = $_;
	if (/.*\/(.*)/){
		$filename = $1;
	}

	open (FILE, $full_path) 
		or die "Cannot open file $full_path!\n";

	while (<FILE>){
		if (/\s*\<title\>(.*)\<\/title\>/){
			print OUT $1, "*", $filename, "\n";
			print "$filename\n";
			$count++;
			last;
		}
	}

	close (FILE);
}

print "ALL DONE! $count files. See parts.lst\n";

1;
