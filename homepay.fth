
forth definitions

: bounds over + swap ;

: nip swap drop ;


vocabulary home
vocabulary payment
vocabulary display 
vocabulary keypad

forth definitions

( : init home definitions ; )

home definitions

: where s" home>" set-prompt ;

: help 
    where
    cr cr
    ." home: help" cr 

    ." Test    : Run tests." cr 
    cr
;

payment definitions

: where s" payment>" set-prompt ;

: help 
    where
    cr ." payment: help" cr
;

keypad definitions

: where s" keypad>" set-prompt ;

: help 
    where
    cr ." Keypad: help" cr
;

display definitions
1B constant ESC

: where s" display>" set-prompt ;

: (home)
    where
    ESC emit ." [H"
;

: cls
    where
    (home)
    ESC emit ." [2J"
;


: help
    where
    cr ." Display: help" cr
    ." cls       : clear screen." cr
;


home
verbose
