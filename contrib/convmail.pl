#!/usr/bin/perl

#This is a GB<->Big5 Email convertsion script.
#It can handle the MIME encoding Subject and Attenchment file.
#It bases on Anthony Fok<foka@debian.org>'s debian-chinese-gateway.pl.

#By Yu Guanghui <ygh@dlut.edu.cn>,2000/6/6
#$Header: /home/prog/autoconvert/contrib/convmail.pl,v 1.2 2000/11/27 16:16:33 ygh Exp $

use strict;
use Fcntl;
use MIME::Parser;
use MIME::Entity;
use MIME::Words qw(:all);

use Getopt::Std;

#
# Variable definitions
#

my $home = "$ENV{HOME}";
my $bin = "/usr/bin";
my $mimemail= "$home/mimemail";
my $encoding = '8bit';

my $tmphdr = "$home/mimemail/msg-$$.hdr";
my $tmpbdy = "$home/mimemail/msg-$$.bdy";

my $sigstripper = "| perl -p -e '\$/ = \"\"; s/(--  )\\n.*\\n.*\\s*\$//;' ";
my $src_chasret;
my $dest;

my %big5 = ( charset => 'big5',
             converter => "$bin/autob5" );

my %gb2312 = ( charset => 'gb2312',
               converter => "$bin/autogb" );

my %opts;
#
# Parse the args

getopts('gb',\%opts);

if ($opts{"g"} && $opts{"b"}) {
	usage();
}elsif($opts{"g"}){
	$dest=\%gb2312;
}elsif($opts{"b"}){
	$dest=\%big5;
}else{
	usage();
}

#
# Parse the message
#

my $parser = new MIME::Parser   output_dir      => $mimemail,
                                output_prefix   => "part",
                                output_to_core  => 20000;

my $top = $parser->read(\*STDIN) or die "couldn't parse MIME stream";
my $head = $top->head;

#Convert the message
if (defined($dest)) {
 
    # Decode the header to 8Bit 
    $head->decode; 
 
    my $header = $head->as_string; 
 
    if ($header =~ /[\x80-\xff]/) { 
	# Convert the decoded header to Big5/GB and write to a file 
	open(TMPHDR, "| $dest->{converter} > $tmphdr") 
	    or die "$!"; 
	$head->print(\*TMPHDR); 
	close TMPHDR; 
 
	# Re-read header, encode it as quoted-printable, and write it back. 
	open(TMPHDR, "< $tmphdr") or die "$!"; 
	undef $/; 
	$header = <TMPHDR>; 
	$/ = "\n"; 
	close TMPHDR; 
	$header = encode_mimewords($header, Charset => $dest->{charset}); 
	open(TMPHDR, "> $tmphdr") or die "$!"; 
	print TMPHDR $header; 
	close TMPHDR; 
 
	# Read in the header as the new head. 
	$top->head(MIME::Head->from_file($tmphdr)) or die "$!"; 
	$head = $top->head; 
	unlink $tmphdr or die "$!"; 
    } 
 
 
    # Convert the message body and parts if "Content-Type: text/plain" 
    my $body = $top->bodyhandle; 
    recode_body($top) if defined($body); 
    $sigstripper = ""; 
    my $num_parts = $top->parts; 
    my $i; 
    for ($i = 0; $i < $num_parts; $i++) { 
	recode_body($top->parts($i)); 
    } 
 
} 

#
#out put the message 
#
my $recvdate=gmtime();
my $sender= $head->get('Return-Path',0);
chop($sender);
$sender=~ s/[<>]//g;
printf("From %s %s\n",$sender,$recvdate);
$top->print(\*STDOUT); 
printf("\n");
 
# Clean up 
$top->purge; 
 
# 
# Subroutines 
# 
 
sub recode_body($) { 
    my $entity = shift; 
    my $head = $entity->head; 
    my $body = $entity->bodyhandle; 
    my $mime_type = $head->mime_type; 
    my $charset = 'us-ascii'; 
    if (lc($mime_type) eq "text/plain" and defined($body)) { 
 
	# Convert the body to Big5/GB and write to a file 
	open(TMPBDY, "$sigstripper| $dest->{converter} > $tmpbdy") or die "$!"; 
	print TMPBDY $body->as_lines; 
	close TMPBDY; 
 
	# Read in the body 
	my $IO = $body->open("w") or die "open body: $!"; 
	open(TMPBDY, "< $tmpbdy") or die "$!"; 
	while (<TMPBDY>) { 
	    $IO->print($_); 
	} 
	close TMPBDY; 
	$IO->close or die "close I/O handle: $!"; 
 
	my $bodytext = $body->as_string; 
	$charset = $dest->{charset} if $bodytext =~ /[\x80-\xff]/; 
	$head->replace('Content-Type', $mime_type . '; charset=' . $charset); 
	$head->replace('Content-Transfer-Encoding', $encoding); 
 
	unlink $tmpbdy or die "$!"; 
    } 
} 

sub usage(){
	print "Usage: convmail.pl [-g|-b]\n";
	print "-g\tConvert mail to gb2312\n";
	print "-b\tConvert mail to big5\n";
	exit;
}
