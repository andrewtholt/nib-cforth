quiet
( example of file handling - nib - 1993-07-03 )
hex
variable read
variable write
1000 constant blocksize
variable buff blocksize allot

( use cp file1 file2 )
: cp
  parse fopenr dup NULL = abort" cannot open input file" read !
  parse fopenw dup NULL = abort" cannot make output file" write !
  begin
    read @ buff blocksize fread
    write @ buff rot fwrite 0=
  until
  read @ fclose drop write @ fclose drop
;
verbose
