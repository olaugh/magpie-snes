; SNES Minimal Startup Code for cc65
; Prime Sieve Test ROM

.include "snes.inc"

; Import main from C code
.import _main

; Import cc65 runtime zeropage variables
.importzp sp

; Export required symbols
.export __STARTUP__

.segment "ROMHDR"

; ROM Registration Data ($FFB0-$FFBF)
.byte   $00, $00
.byte   "PRIM"
.byte   $00, $00, $00, $00, $00, $00, $00
.byte   $00
.byte   $00
.byte   $00

; ROM Specification ($FFC0-$FFDF)
.byte   "PRIME SIEVE TEST     "  ; 21 chars
.byte   $20                     ; LoROM
.byte   $00                     ; ROM only
.byte   $08                     ; 256KB
.byte   $00                     ; No RAM
.byte   $01                     ; North America
.byte   $00
.byte   $00
.word   $FFFF
.word   $0000

.segment "VECTORS"

; Native Mode Vectors
.word   $0000
.word   $0000
.word   Reset
.word   Reset
.word   Reset
.word   Reset
.word   $0000
.word   Reset

; Emulation Mode Vectors
.word   $0000
.word   $0000
.word   Reset
.word   $0000
.word   Reset
.word   Reset
.word   Reset               ; RESET
.word   Reset

.segment "STARTUP"

__STARTUP__:

.proc Reset
    sei
    clc
    xce                     ; Native mode

    rep     #$30
    .a16
    .i16

    lda     #$0000
    tcd                     ; Direct page = 0

    lda     #$1FFF
    tcs                     ; Stack = $1FFF

    sep     #$20
    .a8

    lda     #$00
    pha
    plb                     ; Data bank = 0

    ; Force blank
    lda     #$80
    sta     INIDISP

    ; Clear WRAM (first 8KB)
    rep     #$30
    .a16
    .i16
    ldx     #$0000
    lda     #$0000
@clear:
    sta     $0000,x
    inx
    inx
    cpx     #$2000
    bne     @clear

    ; Initialize cc65 software stack
    sep     #$20
    .a8
    lda     #$00
    sta     sp              ; sp low
    lda     #$1F
    sta     sp+1            ; sp high = $1F00

    ; Call C main
    ; cc65 expects 8-bit A and 8-bit X/Y
    sep     #$30            ; 8-bit A and X/Y
    .a8
    .i8
    jsr     _main

    ; Infinite loop after main returns
@loop:
    wai
    bra     @loop
.endproc

.segment "CODE"
; Main code goes here
