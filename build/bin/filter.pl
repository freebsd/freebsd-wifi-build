#!/usr/local/bin/perl -w

use strict;
use IO::File;

# This is a simple filter for the metalog stuff from FreeBSD's
# install target.

# When doing a user-only build, g"make installworld/make installkernel"
# will populate METALOG in the DESTDIR of your build.  That will contain
# the files/devices/directories/etc from the above make targets with their
# eventual ownership/mode and such.
#
# However, if one wishes to customise things after that build, you'll
# likely want to modify / append to METALOG.
#
# So, to make this a little easier, this is a very simple filter that
# will take a file to add things to (and only add for now!) and it'll
# append/replace entries in the original METALOG.
#
# Ie, if the original metalog has:
#
# ./a
# ./b
#
# And you wish to modify './b' (say, it has a new file size) and add './c',
# then we need to both replace the './b' entry with the new one and append
# any entries that we haven't replaced.
#
# (I'm not really sure why this hasn't come up before as part of all of
# this nonsense, but whatever.)
#

my ($src_file) = shift;
my ($modify_file) = shift;
my ($result_file) = shift;

my (%modify_hash);

# Suck in modify_file into a hash; each hash entry has the original string.

my ($mfh) = new IO::File;
$mfh->open($modify_file, "r") || die "Couldn't read $modify_file: $!\n";

while (<$mfh>) {
	my $s = $_;
	my @param = split(/ /, $s);
	# skip over comments
	next if $s =~ m/^\#/;
	$modify_hash{$param[0]} = $s;
}

$mfh->close();
undef $mfh;

# Ok, we've sucked in the modify list.  Now, use it as a filter over
# the src file.

my ($sfh) = new IO::File;
$sfh->open($src_file, "r") || die "Couldn't read $src_file: $!\n";

my ($dfh) = new IO::File;
$dfh->open($result_file, "w") || die "Couldn't write to $result_file: $!\n";

while (<$sfh>) {
	my ($s) = $_;
	my (@param) = split(/ /, $s);
	if (! defined $modify_hash{$param[0]}) {
		# Not in modify_hash; just print it and continue
		print $dfh $_;
		next;
	}

	# It's in modify_hash; bait and switch
	print $dfh $modify_hash{$param[0]};

	# Now we've seen it; remove that key and its value.
	delete $modify_hash{$param[0]};
}

$sfh->close();

#.. now, print out any modify_hash entries we've not yet seen

foreach (keys %modify_hash) {
	print $dfh $modify_hash{$_};
}

$dfh->close();

exit(0);
