#!/usr/bin/perl

my $mod = './process_hiding.ko';

unless (-e $mod) {
    die "File <$mod> could not be found in the current directory.";
} 

my $res = system "kldload $mod";
if ($res != 0) {
    die "Kernel module failed to load correctly (exit $res).\n";
} 

system 'killall top';

my $ps = `ps`;
if ($ps =~ /top/) {
    die "Could not kill top.\n";
}

system 'top &';
sleep 1;

my $ps = `ps`;
if ($ps !~ /top/) {
    die "Could not start top.\n";
}

my $to_hide = "top";
syscall 210, $to_hide;

my $ps = `ps`;
if ($ps =~ /top/) {
    die "Cat was not hidden.\n";
}

my $res = system "kldunload $mod";
if ($res != 0) {
    die "Kernel module failed to unload correctly (exit $res).\n";
} 

my $ps = `ps`;
if ($ps !~ /top/) {
    die "Cat died during the test; invalid results.\n";
}

print "All tests passed.\n";
