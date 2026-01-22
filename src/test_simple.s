; Super simple test ROM
; Just writes a marker to WRAM and loops

.include "snes.inc"

.segment "ROMHDR"

; ROM Registration Data
.byte   $00, $00                ; Maker code
.byte   "TEST"                  ; Game code
.byte   $00, $00, $00, $00, $00, $00, $00  ; Reserved
.byte   $00                     ; Expansion RAM size
.byte   $00                     ; Special version
.byte   $00                     ; Cartridge sub-type

; ROM Specification
.byte   "SIMPLE TEST          "  ; Game title (21 chars)
.byte   $20                     ; Map mode: LoROM
.byte   $00                     ; Cartridge type: ROM only
.byte   $08                     ; ROM size: 256KB
.byte   $00                     ; RAM size: 0KB
.byte   $01                     ; Destination: NA
.byte   $00                     ; Old licensee
.byte   $00                     ; ROM version
.word   $FFFF                   ; Checksum complement
.word   $0000                   ; Checksum

.segment "VECTORS"

; Native Mode Vectors
.word   $0000                   ; $FFE0: Reserved
.word   $0000                   ; $FFE2: Reserved
.word   nmi_dummy               ; $FFE4: COP
.word   nmi_dummy               ; $FFE6: BRK
.word   nmi_dummy               ; $FFE8: ABORT
.word   nmi_dummy               ; $FFEA: NMI
.word   $0000                   ; $FFEC: Reserved
.word   nmi_dummy               ; $FFEE: IRQ

; Emulation Mode Vectors
.word   $0000                   ; $FFF0: Reserved
.word   $0000                   ; $FFF2: Reserved
.word   nmi_dummy               ; $FFF4: COP
.word   $0000                   ; $FFF6: Reserved
.word   nmi_dummy               ; $FFF8: ABORT
.word   nmi_dummy               ; $FFFA: NMI (emulation)
.word   Reset                   ; $FFFC: RESET
.word   nmi_dummy               ; $FFFE: IRQ/BRK


.segment "STARTUP"

Reset:
    sei                         ; Disable interrupts
    clc
    xce                         ; Native mode

    ; 8-bit A, 8-bit X/Y
    sep     #$30
    .a8
    .i8

    ; Write marker immediately to $0700
    lda     #$42
    sta     $0700
    sta     $0701

    ; Write another marker to $0720
    lda     #$EF
    sta     $0720
    lda     #$BE
    sta     $0721

    ; Write 0xDEAD to $0730 (little-endian: low byte first)
    lda     #$AD                ; Low byte of 0xDEAD
    sta     $0730
    lda     #$DE                ; High byte of 0xDEAD
    sta     $0731

    ; Infinite loop
@loop:
    bra     @loop


.segment "CODE"

nmi_dummy:
    rti
