#!/usr/bin/perl
print "Content-type: text/plain", "\n\n";
$query_string = $ENV{'QUERY_STRING'};
($field_name, $command) = split(/=/, $query_string);
if ($command eq "ls") {
	print `ls`;
} elsif ($command eq "pwd") {
	print `pwd`;
} else {
	print "$command : Command not allow";
}
exit (0);
