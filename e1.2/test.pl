#!/usr/bin/perl

my $mod = './my_sc.ko';

unless (-e $mod) {
    die "File <$mod> could not be found in the current directory.";
} 

my $res = system "kldload $mod";
if ($res != 0) {
    die "Kernel module failed to load correctly (exit $res).\n";
} 

system './test "bunnies are the best"';
my $tail = `tail /var/log/messages`;

if ($tail !~ /bunnies are the best/) {
    die "Could not find syscall message in log file.\n";
}

my $res = system "kldunload $mod";
if ($res != 0) {
    die "Kernel module failed to unload correctly (exit $res).\n";
} 

print "All tests passed.\n";
