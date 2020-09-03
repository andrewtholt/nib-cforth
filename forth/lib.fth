quiet

( -------------------------------- )
( Commonly used library functions  )
( -------------------------------- )
hex

9 constant tab

: nip
  swap drop
;

: tuck
  swap over
;

: 2dup
  over over 
;

: 2drop
  drop drop
;

: align
dup bytes/cell mod
if
  bytes/cell + FFFFFFFc and
endif
;

( Return the number of bytes for a given number of cells )

: cells ( n1 --- n2 )
  bytes/cell * align
;

: cell+ ( n1 --- n2)
    bytes/cell +
;

: 2constant
  create
    swap 
    , ,
  does>
  dup @ swap cell+ @ ;

: chars
;



( create a buffer for n chars      )
( -------------------------------- )

: cbuffer: ( n --- )
create
    chars allot
does>
;

( -------------------------------- )
( create a buffer for n cells      )
( -------------------------------- )

: buffer:
create
     cells allot
does>
;

: erase ( addr n --- )
  0 fill
;

: blank ( addr n --- )
  bl fill
;

: ascii bl word here 1+ c@ ; immediate
verbose

