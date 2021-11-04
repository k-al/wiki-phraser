#!/usr/bin/perl -w
use strict;

use lib 'wiki-phraser';

use util::RecursiveFileEdit;

use File::Copy;

sub main {
    
}


# returns %arg with:
#   replace => bool
#   ask     => bool
#   update  => bool
#   silent  => bool
#   indatn  => string (path)
#   outdatn => string (path)
sub get_args {
    my $ar;
    my @arlist;
    my %arg = ('replace' => 0,
            'ask'     => 0,
            'update'  => 0,
            'silent'  => 0,
            'indatn'  => '',
            'outdatn' => '');

    while ($ARGV[0] =~ m/^-/) {
        $ar = shift(@ARGV);
        if ($ar =~ m/^--/) {
            if ($ar eq '--replace') {
                $arg{'replace'} = 1;
            } elsif ($ar eq '--ask') {
                $arg{'ask'} = 1;
            } elsif ($ar eq '--update') {
                $arg{'update'} = 1;
                warn "Updatefunction doesnt work!\n";
            } elsif ($ar eq '--silent') {
                $arg{'silent'} = 1;
            } else {
                die "Wrong Argument: '$ar'\n";
            }
        } else {
            @arlist = split(m//, $ar);
            shift(@arlist); # delet the '-' at the beginning
            
            while (defined($arlist[0])) {
                $ar = shift(@arlist);
                if ($ar eq 'r') {
                    $arg{'replace'} = 1;
                } elsif ($ar eq 'a') {
                    $arg{'ask'} = 1;
                } elsif ($ar eq 'u') {
                    $arg{'update'} = 1;
                    warn "Updatefunction doesnt work!\n";
                } elsif ($ar eq 's') {
                    $arg{'silent'} = 1;
                } else {
                    die "Wrong Argument: '-$ar'\n";
                }
            }
        }
    }
    
    defined($ARGV[0]) or die "no infile given!";
    defined($ARGV[1]) or die "no outile given!";

    $arg{'indatn'} = $ARGV[0];
    $arg{'outdatn'} = $ARGV[1];
    
    return %arg;
}

sub phrase {
    defined(my $file_name = shift) or die "Died in __FILE__::phrase()\n  No file path given!\n";
    defined(my $phrase_tree = shift) or die "Died in __FILE__::phrase()\n  Phrase tree is lost!\n";
    defined(my $home = shift) or die "Died in __FILE__::phrase()\n  Home is not defined! (mark no home with empty string)\n";
    
    open(INFILE, $file_name) or die "Died in __FILE__::phrase()\n  Could not open file '$file_name'.\n$!";
    
    my %meta = read_metadata(\*INFILE, $file_name);
    
    close INFILE;
}

sub read_metadata {
    defined(my $fh = shift) or die "Died in __FILE__::get_till_bracket_end()\n  No file handle given!\n";
    defined(my $file_name = shift) or die "Died in __FILE__::get_till_bracket_end()\n  No file name given!\n";
    
    my %meta;
    my @sp;
    my $line;
    
    while (my $line = <$fh>) {
        chomp($line);
        if ($line eq 'start') {
            return %meta;
        } elsif ($line eq 'sideboard') {
            warn("$file_name: Sideboard is not implemented yet!\n");
#             must be implemented
        } else {
            @sp = split(':', $line, 2);
            
            if (not defined($sp[1])) {
                warn "broken header in $file_name in line $. (missing 'start'?)\n";
                next;
            }
            
            chomp($sp[0]);
            chomp($sp[1]);
            
            #? phrase $sp[1] for maximum flex
            $meta{$sp[0]} = $sp[1];
        }
    }
    
    warn "$file_name is header only. (missing 'start'?)\n";
    return %meta;
}



sub get_till_bracket_end {
    defined(my $string = shift) or die "Died in __FILE__::get_till_bracket_end()\n  No string given!\n";
    
    my $start_char = chop($string);
    my $end_char;
    
    if ($start_char == '}') {
        $end_char = '{';
    } elsif ($start_char == '{') {
        $end_char = '}';
    } elsif ($start_char == ']') {
        $end_char = '[';
    } elsif ($start_char == '[') {
        $end_char = ']';
    } elsif ($start_char == '>') {
        $end_char = '<';
    } elsif ($start_char == '<') {
        $end_char = '>';
    } elsif ($start_char == ')') {
        $end_char = '(';
    } elsif ($start_char == '(') {
        $end_char = ')';
    } else {
        die "Died in __FILE__::get_till_bracket_end()\n  '$start_char' ist not a valid bracket begin!\n";
    }
    
    my $res_string;
    
    
    for (my $i = 1; $i;) { #
        if ($string =~ s/^(|*?[^\\](?:\\\\)*?)([$start_char]|[$end_char])//) {
            $res_string .= $1;
            
            if ($2 == $start_char) {
                my ($ret_val, $tmp_string) = get_till_bracket_end($start_char.$string);
                if ($ret_val) {
                    return 0, $string;
                }
                $res_string .= $start_char . $tmp_string . $end_char;
            } else {
                $i = 0; #break
            }
        } else {
            return 0, $string;
        }
    }
    
    return 1, $res_string, $';
}

sub get_till_bracket_start {
    defined(my $string = shift) or die "Died in __FILE__::get_till_bracket_start()\n  No string given!\n";
    
    my $rev_string = reverse_with_escapes($string);
    my ($ret_val, $in_bracket, $rest_string) = get_till_bracket_end($rev_string);
    
    return ($ret_val, $string) if $ret_val;
    
    $in_bracket = reverse_with_escapes($in_bracket);
    $rest_string = reverse_with_escapes($rest_string);
    
    return (1, $in_bracket, $rest_string);
}

sub reverse_with_escapes {
    defined(my $string = shift) or die "Died in __FILE__::reverse_with_escapes()\n  No string given!\n";
    $string =~ s/(?:^|[^\\])(?:\\\\)*\\([^\\])/$1\\/g; # reverse the escape cahracter '\'
    $string = reverse($string);
    return $string;
}




