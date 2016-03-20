#!/usr/bin/perl

my $mod = './process_hiding.ko';

unless (-e $mod) {
    die "File <$mod> could not be found in the current directory.";
} 

my $res = system "kldload $mod";
if ($res != 0) {
    die "Kernel module failed to load correctly (exit $res).\n";
} 

system 'killall cat';

my $ps = `ps`;
if ($ps =~ /cat/) {
    die "Could not kill cat.\n";
}

system 'cat &';

my $ps = `ps`;
if ($ps !~ /cat/) {
    die "Could not start cat.\n";
}

syscall(210, "cat");

my $ps = `ps`;
if ($ps =~ /cat/) {
    die "Cat was not hidden.\n";
}

my $res = system "kldunload $mod";
if ($res != 0) {
    die "Kernel module failed to unload correctly (exit $res).\n";
} 

my $ps = `ps`;
if ($ps !~ /cat/) {
    die "Cat died during the test; invalid results.\n";
}

print "All tests passed.\n";
