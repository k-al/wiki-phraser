#!/usr/bin/perl -w
use strict;

use lib 'phraser';

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

sub interpreter {
    
    
    
    while (m/(\[.*?\]|\S*[^\]\\])\$(tag|picture|section|)\{(.*?)\}/) { # should match: [a multi word phrase]$tag{something} and one_word(phrase)$tag{some other thing}
        
    }
}

sub get_till_bracket_end {
    my $string = shift or die "Died in __FILE__::get_till_bracket_end()\n  No string given!\n";
    
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
                    return 1, $string;
                }
                $res_string .= $start_char . $tmp_string . $end_char;
            } else {
                $i = 0; #break
            }
        } else {
            return 1, $string;
        }
    }
    
    return 0, $res_string, $';
}

sub get_till_bracket_start {
    my $string = shift or die "Died in __FILE__::get_till_bracket_start()\n  No string given!\n";
    
    my $rev_string = reverse_with_escapes($string);
    my ($ret_val, $in_bracket, $rest_string) = get_till_bracket_end($rev_string);
    
    return ($ret_val, $string) if $ret_val;
    
    $in_bracket = reverse_with_escapes($in_bracket);
    $rest_string = reverse_with_escapes($rest_string);
    
    return (0, $in_bracket, $rest_string);
}

sub reverse_with_escapes {
    my $string = shift or die "Died in __FILE__::reverse_with_escapes()\n  No string given!\n";
    $string =~ s/(?:^|[^\\])(?:\\\\)*\\([^\\])/$1\\/g; # reverse the escape cahracter '\'
    $string = reverse($string);
    return $string;
}




