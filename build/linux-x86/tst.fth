
variable fp
variable ptr

: file-tst
    20 allocate abort" Failed to allocate memory" ptr !
    s" test.txt" r/w open-file abort" Failed to open file"
    fp !
;

: read-test
    ptr @ 5 fp @ read-file  abort" read failed"
    ptr @ swap dump
;

file-tst
read-test
