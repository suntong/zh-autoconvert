#!/usr/bin/perl

# This is the Big5<->GB E-mail conversion Perl script that acts as a
# gateway between debian-chinese-big5 and debian-chinese-gb.
#
# Based on Anthony Wong <ypwong@debian.org> .procmailrc
# and Yu Guanghui <ygh@debian.org> zh-AutoConvert program.
#
# By Anthony Fok <foka@debian.org>, Tue, 23 May 2000 20:21:08 -0600
# Updated: Wed, 18 Oct 2000 00:09:43 -0600

use strict;
use Fcntl;
use MIME::Parser;
use MIME::Entity;
use MIME::Words qw(:all);

#
# Variable definitions
#

my $home = "$ENV{HOME}";
my $bin = "$home/bin";
my $gateway;
my $fallback = 'foka@debian.org';
my $encoding = '8bit';

my %big5 = ( list => 'debian-chinese-big5@lists.debian.org',
	     request => 'debian-chinese-big5-request@lists.debian.org',
	     charset => 'big5',
	     converter => "$bin/autob5" );

my %gb2312 = ( list => 'debian-chinese-gb@lists.debian.org',
	       request => 'debian-chinese-gb-request@lists.debian.org',
	       charset => 'gb2312',
	       converter => "$bin/autogb" );

my $tmphdr = "$home/mimemail/msg-$$.hdr";
my $tmpbdy = "$home/mimemail/msg-$$.bdy";

my $sigstripper = "| perl -p -e '\$/ = \"\"; s/(--  )\\n.*\\n.*\\s*\$//;' ";

my $src_charset;

my $discard = 0;

#
# Parse command-line arguments
#

# Get args:
my $nosend = 0;
ARG: while (@ARGV) {
    $_ = shift @ARGV;
    /^-n/ and do {
	$nosend = 1;
	next ARG;
    };
#    /^-t(.*)/ and do {
#	$to = $1 || shift @ARGV;
#	next ARG;
#    };
    die "$0: bad usage: <$_>.\n";
}

sysopen(LOG, "$home/filter.log", O_WRONLY | O_APPEND | O_CREAT, 0600)
    or die "$0: Can't open filter.log: $!";

## Verify destination:
#$to or die "$0: missing [-t to]\n";

#if (defined(&{"init_".$to})) {
#    &{"init_".$to};
#} else {
#    die "$0: $to is not a valid encoding";
#}

#
# Parse the message
#

my $parser = new MIME::Parser	output_dir	=> "$home/mimemail",
				output_prefix	=> "part",
				output_to_core 	=> 20000;

my $top = $parser->read(\*STDIN) or die "couldn't parse MIME stream";

#
# Determine its final destination
#

my $head = $top->head;
my $src_From = $head->get('From', 0);
my $src_Subject = $head->get('Subject', 0);
my $src_X_Mailing_List = $head->get('X-Mailing-List', 0);
my $src_Resent_Sender = $head->get('Resent-Sender', 0);
my $src_X_Loop = $head->get('X-Loop', 0);
my $src_X_Loop_Convert = $head->get('X-Loop-Convert', 0);
my $dest;
if (defined($src_X_Mailing_List) and defined($src_Resent_Sender)) {
    $src_X_Mailing_List =~ /^<(.+?)>/;
    $src_X_Mailing_List = $1;
    chomp $src_Resent_Sender;
#    print LOG $src_X_Mailing_List, ", ", $src_Resent_Sender, "\n";
    print LOG `/bin/date --iso-8601=seconds` . "  From: $src_From";
    print LOG "    Subject: $src_Subject\n";
    if ($src_X_Mailing_List eq $big5{list}
	and $src_Resent_Sender eq $big5{request}) {
	$src_charset = "big5";
	$dest = \%gb2312;
	$gateway = 'foka-gateway-big5@debian.org';
    }
    elsif ($src_X_Mailing_List eq $gb2312{list}
	   and $src_Resent_Sender eq $gb2312{request}) {
	$src_charset = "gb2312";
	$dest = \%big5;
	$gateway = 'foka-gateway-gb@debian.org';
    }
    else {
	undef $dest;
    }
} else {
    undef $dest;
}

# undef $dest if defined($src_X_Loop) and $src_X_Loop eq $gateway;
# undef $dest if defined($src_X_Loop_Convert) and $src_X_Loop_Convert eq $gateway;

if (defined($src_X_Loop_Convert)) {
    undef $dest;
    $discard = 1;
    print LOG "    X_Loop_Convert: $src_X_Loop_Convert\n";
}

if (defined($dest)) {
    # i.e., this message was sent to either debian-chinese-big5 or
    #       debian-chinese-gb
    #
    # Convert the header: Big5<->GB

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


    # Convert the message body and parts iff "Content-Type: text/plain"
    my $body = $top->bodyhandle;
    recode_body($top) if defined($body);
    $sigstripper = "";
    my $num_parts = $top->parts;
    my $i;
    for ($i = 0; $i < $num_parts; $i++) {
	recode_body($top->parts($i));
    }

    # Change "To:" and "Cc:" to the destination mailing list, if applicable
    foreach (qw/To Cc/) {
	my $tmpstr = $head->get($_);
	if ($tmpstr =~ /$src_X_Mailing_List/) {
	    $tmpstr =~ s/$src_X_Mailing_List/$dest->{list}/;
	    $top->head->replace($_, $tmpstr);
	}
    }

    # Add "X-Loop-Convert";
    $top->head->replace('X-Loop-Convert', $gateway);

    foreach (qw/X-Mailing-List Precedence Resent-Date Resent-BCC Resent-CC
	     Resent-Message-Id Resent-From Resent-Sender/) {
	$top->head->delete($_);

	$top->sync_headers(Length=>'COMPUTE');
    }
}


# Launch mailer and send message?
if ($discard) {
}
elsif ($nosend) {
    $top->print(\*STDOUT);
}
else {
    my $recipient = defined($dest) ? $dest->{list} : $fallback;

    open SENDMAIL, "|/usr/lib/sendmail -oi -oem $recipient"
	or die "$0: open sendmail: $!\n";
    $top->print(\*SENDMAIL);
    close SENDMAIL;
    die "sendmail failed" if ($? >> 255);
    print LOG "    Resent to $recipient\n";
}

# Clean up
$top->purge;
close LOG;


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
	print TMPBDY "-- \n";
	print TMPBDY "| This message was re-posted from $src_X_Mailing_List\n";
	print TMPBDY "| and converted from $src_charset to $dest->{charset} by an automatic gateway.";
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
