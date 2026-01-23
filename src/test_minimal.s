; Minimal SNES test ROM
; Just sets background to solid blue

.include "snes.inc"

.segment "ROMHDR"

; ROM Registration Data ($FFB0-$FFBF)
.byte   $00, $00
.byte   "TEST"
.byte   $00, $00, $00, $00, $00, $00, $00
.byte   $00
.byte   $00
.byte   $00

; ROM Specification ($FFC0-$FFDF)
.byte   "MINIMAL TEST         "  ; 21 chars
.byte   $20                     ; LoROM
.byte   $00                     ; ROM only
.byte   $05                     ; 32KB
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

.export __STARTUP__
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

    ; Set background color to bright blue
    stz     CGADD           ; Color 0
    lda     #$00            ; Blue low byte (BGR: 31<<10 = $7C00, low = $00)
    sta     CGDATA
    lda     #$7C            ; Blue high byte
    sta     CGDATA

    ; Disable all BG and sprites
    stz     TM
    stz     TS
    stz     BGMODE

    ; Enable display at full brightness
    lda     #$0F
    sta     INIDISP

    ; Enable NMI
    lda     #$80
    sta     NMITIMEN

forever:
    wai                     ; Wait for interrupt
    bra     forever
.endproc

.segment "CODE"
; Dummy to satisfy linker
