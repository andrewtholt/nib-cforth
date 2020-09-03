include lib.fth
include struct.fth

decimal
( instance specific constants )
2000 constant max-records
10 constant name
20 constant definition

s" ./t1.db" 2constant dump-file
s" /dev/tty" 2constant dump-display

( offsets into name field )
struct
10 chars field first-name
20 chars field  surname
endstruct name



struct
20 chars field address
10 chars field area
10 chars field town
10 chars field county
 8 chars field post-code
 8 chars field std-code
12 chars field phone-number
30 chars field e-mail
endstruct definition

( generic database constants )

 1 constant FIXED                ( records are of fixed length )
 2 constant DUPLICATE            ( Duplicate records allowed )
 4 constant FIXED_DB_SIZE        ( pre-allocate database )
 8 constant NEVER_SHRINK         ( deleted entries to free list. )
16 constant STAMP                ( Apply time stamp to each record )

( variables and data buffers )

name buffer: name-buffer
definition  buffer: def-buffer
variable db-handle
-1 db-handle !
variable exit-flag
0 exit-flag !

: acknowledge
  cr
  ." ... Done, press any key to continue >"
  key drop cr
;

: init
  name-buffer name bl fill
  def-buffer  definition bl fill
;



: create-database
  db-handle @ -1 =
  if
    101 db-create db-handle !
    
    db-handle @ FIXED FIXED_DB_SIZE or  DUPLICATE or STAMP or
    db-set-flags
  
    db-handle @ name db-set-name-size
    db-handle @ definition db-set-def-size
  
    db-handle @ max-records db-set-max-records
  
    db-handle @ 90 db-set-alarm
    db-handle @ db-status
  else
    cr
    ." Database exists" cr
  then
;


( This looks a bit odd, pfe apends a null to the end of the string )
( that accept returns.  The upshot of this that if I ask for ten )
( characters, I could get 11. )

: enter-record
  init
  ( page )
  ." First Name : " name-buffer first-name accept drop
  cr
  ." Surname    : " name-buffer surname accept drop
  cr
  ." Street     : " def-buffer address accept drop
  cr
  ." Area       : " def-buffer area accept drop
  cr
  ." Post Town  : " def-buffer town accept drop
  cr
  ." County     : " def-buffer county accept drop
  cr
  ." Post Code  : " def-buffer post-code accept drop
  cr
  ." Phone      : " def-buffer std-code accept drop
  ." -" def-buffer phone-number accept drop
  cr
  ." e-mail     : " def-buffer e-mail accept drop
  cr
  db-handle @
  name-buffer name def-buffer definition db-install abort" Failed to update"
;

: update-file
  ." Updating file..." cr
  db-handle @ dump-file $db-dump
  acknowledge
;


: load-from-file
  ." Loading .." cr
  db-handle @ dump-file $db-load abort" Cant open file"
  acknowledge
;

: query
  ." Name >> "
  name-buffer dup name accept
  db-handle @ -rot db-find-first
  if
    cr ." Record not found" cr
  else
    db-handle @ swap db-display-record
  then
  acknowledge
;

: update-file?
  dup 49 =     ( ascci 1 )
  if
    update-file
  then
;

: enter-record?
  dup 50 =
  if
    enter-record
  then
;

: query?
  dup 51 =
  if
    query
  then
;

: load-from-file?
  dup 52 =
  if
    load-from-file
  endif
;

: dump-to-screen?
  dup 53 =
  if
    db-handle @ dump-display $db-dump
    acknowledge
  then
;

: quit?
  dup dup 113 =
  swap  81 = or   ( q or Q )
  if
    ." Quit" cr
    -1 exit-flag !
  then
;

: status?
  dup 54 =
  if
    db-handle @ db-status
    acknowledge
  then
;

: last drop ;

: process-options
update-file?
enter-record?
query?
load-from-file?
dump-to-screen?
status?
quit?
last
;

: menu
  begin
    cr cr cr
    ." 1. Update file" cr
    ." 2. Enter data" cr
    ." 3. Query" cr
    ." 4. Load from file" cr
    ." 5. Dump to screen" cr
    ." 6. Status" cr
    ." q. Quit" cr
    0 exit-flag !
    cr cr cr ." >>> " key
    cr
    process-options
    exit-flag @
  until
;




: test
( create database with hash 101 ... )
  101 db-create db-handle !

( ... now load some data... )
  db-handle @ dump-file $db-load abort" Failed to load data"

( ... and print the status )
  db-handle @ db-status

( Find ... )
  db-handle @ s" Holt" db-find-first
  if
    cr ." Record not found" cr
  else
( ... and display a record )
    db-handle @ swap db-display-record
  then
;

: t1
  create-database
  enter-record

  name-buffer name dump 
  cr
  def-buffer definition dump
;


: t2
  init
  create-database
  menu
;
