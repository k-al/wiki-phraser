
sub recursivAllFiles {
    defined(my $root = shift) or die "Died in __FILE__::recursiveAllFiles()\n  No root given!\n";
    defined(my $func = shift) or die "Died in __FILE__::recursiveAllFiles()\n  No execution function given!\n";
    my @para_list = @_;
    
    -e $root or die "Died in __FILE__::recursiveAllFiles()\n  Root dosnt exist!\n";
    
    my @todo = ($root);
    my $element;
    
    while (@todo) {
        $element = shift(@todo);
        if (-d $element) {
            my @files = glob($element.'/*');
            
            for my $k (@files) {
                push(@todo, $root.'/'.$k);
            }
        } else {
            &$func($element, @para_list);
        }
    }
}

#! untested pattern matching
sub get_dir {
    defined(my $file = shift) or die "Died in __FILE__::getDir()\n  No path given!\n";
    -e $file or die "Died in __FILE__::getDir()\n  Given path is not valid!\n";
    
    # if it is already a directory, we are ready
    return $file if -d $file;
    
    if ($file =~ m/\//) { # check for normal paths
        # delete from the last unescaped '/' unitl the end of the string and leave the '/' (to keep this working, even in the root directory)
        $file =~ s/\/((?:[^\/]|\\(?:\\\\)*\/)*)$/\//;
    } else { # if the file is in the active directory, the './' doesnt have to exist. so if $file has no '/' the file is probably in ./
        $file = './';
    }
    
    return $file;
}

# takes two directorys (or files) and creates a relative path from the first directory to the second directory (or file)
# if the first one is a file it takes the the directory in wich the file is with get_dir()
sub create_rel_path {
    my $start = shift;
    my $dest = shift;
    
    defined($start) or die 'start point of relative path is not defined';
    defined($dest) or die 'destination point of relative path is not defined';
    
    # if start is a file (with ending) delete the filename
    $start = get_dir($start);
    
    # if directory names end with '/', delete it; so it doesnt generate '//' in paths
    $start =~ s/\/$//;
    $dest =~ s/\/$//;
    
    my @start_ar = split('/', $start);
    my @dest_ar = split('/', $dest);
    
    my $rel_path = "";
    
    # same directorys from the root cancel out each other
    while ($start_ar[0] eq $dest_ar[0]) {
        shift(@start_ar);
        shift(@dest_ar);
        
        last if not defined($start_ar[0]);
        last if not defined($dest_ar[0]);
    }
    
    # go up all the directorys that are left in $start_ar
    while (defined($start_ar[0])) {
        shift(@start_ar);
        $rel_path .= '../';
    }
    
    # go down all the directorys that are left in $dest_ar
    while (defined($dest_ar[0])) {
        $rel_path .= shift(@dest_ar).'/';
    }
    
    #delete the last '/'
    $rel_path =~ s/\/$//;
    
    return $rel_path;
}

1;
