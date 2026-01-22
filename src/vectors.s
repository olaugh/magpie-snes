; SNES ROM Header and Interrupt Vectors
; Magpie Scrabble AI - SNES Port
;
; LoROM mapping: Header at file offset $7FB0

.include "snes.inc"

; Import C main function
.import _main

; Import cc65 runtime zeropage variables
.importzp sp

; Export symbols
.export _frame_counter
.export _vblank_handler
.export __STARTUP__

.segment "ROMHDR"

; ROM Registration Data ($FFB0-$FFBF) - Extended header
.byte   $00, $00                ; Maker code
.byte   "MPIE"                  ; Game code (4 chars)
.byte   $00, $00, $00, $00, $00, $00, $00  ; Reserved (7 bytes)
.byte   $00                     ; Expansion RAM size
.byte   $00                     ; Special version
.byte   $00                     ; Cartridge sub-type

; ROM Specification ($FFC0-$FFDF)
.byte   "SCRABBLE AI          "  ; Game title (21 chars, space padded)
.byte   $20                     ; Map mode: $20 = LoROM
.byte   $00                     ; Cartridge type: ROM only
.byte   $08                     ; ROM size: 2^8 * 1KB = 256KB
.byte   $00                     ; RAM size: 0KB (no SRAM)
.byte   $01                     ; Destination: North America
.byte   $00                     ; Fixed value (old licensee)
.byte   $00                     ; ROM version
.word   $FFFF                   ; Checksum complement
.word   $0000                   ; Checksum

.segment "VECTORS"

; Native Mode Vectors ($FFE0-$FFEF)
.word   $0000                   ; $FFE0: Reserved
.word   $0000                   ; $FFE2: Reserved
.word   int_dummy               ; $FFE4: COP
.word   int_dummy               ; $FFE6: BRK
.word   int_dummy               ; $FFE8: ABORT
.word   int_nmi                 ; $FFEA: NMI (VBlank)
.word   $0000                   ; $FFEC: Reserved
.word   int_dummy               ; $FFEE: IRQ

; Emulation Mode Vectors ($FFF0-$FFFF)
.word   $0000                   ; $FFF0: Reserved
.word   $0000                   ; $FFF2: Reserved
.word   int_dummy               ; $FFF4: COP
.word   $0000                   ; $FFF6: Reserved
.word   int_dummy               ; $FFF8: ABORT
.word   int_dummy               ; $FFFA: NMI (emulation mode)
.word   Reset                   ; $FFFC: RESET - Entry point!
.word   int_dummy               ; $FFFE: IRQ/BRK


.segment "STARTUP"

; Entry point label required by cc65 runtime
__STARTUP__:

; Reset handler - this is the entry point
; Simplified to match working prime_sieve startup
.proc Reset
    sei                         ; Disable interrupts
    clc
    xce                         ; Switch to native mode

    ; 16-bit mode for setup (matching prime_sieve)
    rep     #$30
    .a16
    .i16

    lda     #$0000
    tcd                         ; Direct page = $0000

    lda     #$1FFF
    tcs                         ; Stack = $1FFF

    ; 8-bit A for register writes
    sep     #$20
    .a8

    lda     #$00
    pha
    plb                         ; Data bank = $00

    ; Force blank
    lda     #$80
    sta     INIDISP

    ; Clear WRAM (first 8KB) - inline like prime_sieve
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

    ; Initialize cc65 software stack pointer
    sep     #$20
    .a8
    lda     #$00
    sta     sp
    lda     #$1F
    sta     sp+1

    ; Call C main (cc65 expects 8-bit A, 8-bit X/Y)
    sep     #$30                ; 8-bit A and X/Y
    .a8
    .i8
    jsr     _main

    ; Infinite loop after main returns
@loop:
    wai
    bra     @loop
.endproc


; Initialize PPU to known state
.proc InitPPU
    php
    sep     #$20
    .a8

    ; Force blank
    lda     #$80
    sta     INIDISP

    ; Clear all PPU registers
    stz     OBSEL
    stz     OAMADDL
    stz     OAMADDH
    stz     BGMODE
    stz     MOSAIC
    stz     BG1SC
    stz     BG2SC
    stz     BG3SC
    stz     BG4SC
    stz     BG12NBA
    stz     BG34NBA

    ; Clear scroll registers (write twice)
    stz     BG1HOFS
    stz     BG1HOFS
    stz     BG1VOFS
    stz     BG1VOFS
    stz     BG2HOFS
    stz     BG2HOFS
    stz     BG2VOFS
    stz     BG2VOFS
    stz     BG3HOFS
    stz     BG3HOFS
    stz     BG3VOFS
    stz     BG3VOFS
    stz     BG4HOFS
    stz     BG4HOFS
    stz     BG4VOFS
    stz     BG4VOFS

    ; VRAM settings
    stz     VMAIN
    stz     VMADDL
    stz     VMADDH

    ; Mode 7 (not used)
    stz     M7SEL
    stz     M7A
    stz     M7A
    stz     M7B
    stz     M7B
    stz     M7C
    stz     M7C
    stz     M7D
    stz     M7D
    stz     M7X
    stz     M7X
    stz     M7Y
    stz     M7Y

    ; CGRAM
    stz     CGADD

    ; Window settings
    stz     W12SEL
    stz     W34SEL
    stz     WOBJSEL
    stz     WH0
    stz     WH1
    stz     WH2
    stz     WH3
    stz     WBGLOG
    stz     WOBJLOG

    ; Screen designation
    stz     TM
    stz     TS
    stz     TMW
    stz     TSW

    ; Color math
    stz     CGWSEL
    stz     CGADSUB
    lda     #$E0
    sta     COLDATA
    stz     SETINI

    plp
    rts
.endproc


; Initialize RAM
.proc InitRAM
    php
    sep     #$20
    .a8
    rep     #$10
    .i16

    ; Clear direct page area ($0000-$00FF)
    ldx     #$0000
    lda     #$00
@clear_dp:
    sta     $00,x
    inx
    cpx     #$0100
    bne     @clear_dp

    ; Clear BSS area ($0100-$1FFF)
    ldx     #$0100
@clear_bss:
    sta     $0000,x
    inx
    cpx     #$2000
    bne     @clear_bss

    plp
    rts
.endproc


.segment "CODE"

; NMI (VBlank) handler
.proc int_nmi
    rep     #$30
    .a16
    .i16
    pha
    phx
    phy
    phb

    ; Increment frame counter
    sep     #$20
    .a8
    inc     _frame_counter
    bne     @no_carry
    inc     _frame_counter+1
@no_carry:

    ; Acknowledge NMI by reading RDNMI
    lda     RDNMI

    rep     #$30
    .a16
    .i16
    plb
    ply
    plx
    pla
    rti
.endproc


; Dummy interrupt handler
.proc int_dummy
    rti
.endproc


.segment "BSS"

_frame_counter:
    .res    2

_vblank_handler:
    .res    2
