
variable fp
variable ptr

: file-tst
    20 allocate abort" Failed to allocate memory" ptr !
    s" test.txt" r/w create-file abort" Failed to open file"
    fp !
;

: write-test
    s" This is a test" fp @ write-line abort" Write line failed"
;

: finish
    fp @ close-file abort" close failed"
;

file-tst
( write-test )
