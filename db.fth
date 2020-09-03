include dbtest.fth
: setup
( create database with hash 101 ... )
  101 db-create db-handle !

( ... now load some data... )
  db-handle @ dump-file $db-load abort" Failed to load data"

( ... and print the status )
   db-handle @ db-status
;

variable match-count

: make-call
      dup
      db-handle @ swap name-buffer db-get-name drop
      db-handle @ swap def-buffer db-get-def drop
      name-buffer first-name -trailing type
      bl emit
      name-buffer surname -trailing type
      tab emit
      def-buffer  std-code -trailing type
      ." -"
      def-buffer  phone-number -trailing type
      cr
;


: $call ( str len --- rec-cnt )
  0 match-count !
  2dup
  db-handle @ -rot db-find-first
  if
    drop 2drop
    cr ." No records found" cr
  else
    make-call
    1 match-count +!
    begin
      2dup db-handle @ -rot db-find-next 
      0=
    while
      1 match-count +!
      make-call
    repeat
    2drop
  then
  match-count @
;

: call ( --- )
  cr
  bl word count
  $call
  cr ." Found " . ."  records" cr
;

setup 

