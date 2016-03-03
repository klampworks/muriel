#!/usr/bin/perl

my $mod = './hello.ko';

unless (-e $mod) {
    die "File <$mod> could not be found in the current directory.";
} 

my $res = system "kldload $mod";
if ($res != 0) {
    die "Kernel module failed to load correctly (exit $res).\n";
} 

my $tail = `tail /var/log/messages`;

if ($tail !~ /Hello, world!/) {
    die "Could not find load message in log file.\n";
}

my $res = system "kldunload $mod";
if ($res != 0) {
    die "Kernel module failed to unload correctly (exit $res).\n";
} 

my $tail = `tail /var/log/messages`;

if ($tail !~ /Good-bye, cruel world!/) {
    die "Could not find unload message in log file.\n";
}

print "All tests passed.\n";
