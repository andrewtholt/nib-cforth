( )
( Forth data structures from Forth Dimension UK )
( )
: dofield
does> ( c d )
 dup @  ( c d o )
 swap   ( c o d )
 cell+ @      ( c o s )
 -rot   ( s c o )
 + swap 
;

: field   ( offset size )
  create 
    2dup  
    swap 
    ,    
    ,   
    +  
  dofield
;

0 constant struct

: endstruct
  create
    ,
  does>
    @
;
