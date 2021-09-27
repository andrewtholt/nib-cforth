( These words have been added for the database in forth project )
( Results from a search can be placed in one list )
( Results from a subsequent search placed in another, and then )
( a boolean function [ and, or etc ] perfromed between the two )
( and a third list hold the results )

( rec list --- )
( )
( Pushes the record <rec> on to the list. )
( )
: push-node ( &New &List -- )
  >r   ( rec )
  r@ @ 0=   ( rec flag )
  if
    dup cell+ 0 swap !
    r> !
  else
    r@ @ over ( rec head rec )
    cell+ !   ( rec )
    r> !
  then
;

( list )
(  )
( If there is a record on the list, place a pointer to it on the stack )
( and a false flag.  If list is empty the return true. i.e. a fail flag )
(  )
: pop-node ( head --- rec false | true )
  dup >r  ( head )
  @ if
    r@ @ dup  ( first first )
    cell+ @   ( first link )
    r> !      ( first )
    0
  else
    r> drop -1
  then
;

( given the address of a node, return the address of the next )
( returns zero if at last node )
( )
: next-node ( node --- next-node )
  cell+ @
;


variable result-list
0 result-list

variable first-condition
0 first-condition !

variable second-condition
0 second-condition !

( This is the only list that is required.  When a node is removed )
( from a list it is placed here.  New nodes are only allocated when this )
( is empty. )
(  )
variable free-list
0 free-list !

( If there is a node on the free list, remove it )
( if not create one. )
: mk-node ( --- node )
  free-list @ 
  0= if
    here 2 cells allot 0
  else
    free-list pop-node
  then
;

( Take a node from the list and push it to the free list )
( )
: rm-node ( &list --- )
  pop-node 0= if
    free-list push-node
  then
;

