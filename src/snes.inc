; SNES Hardware Register Definitions
; For ca65 assembler

; PPU Registers ($2100-$213F)
INIDISP     = $2100     ; Display control
OBSEL       = $2101     ; Object size and base
OAMADDL     = $2102     ; OAM address low
OAMADDH     = $2103     ; OAM address high
OAMDATA     = $2104     ; OAM data write
BGMODE      = $2105     ; BG mode and tile size
MOSAIC      = $2106     ; Mosaic settings
BG1SC       = $2107     ; BG1 tilemap address
BG2SC       = $2108     ; BG2 tilemap address
BG3SC       = $2109     ; BG3 tilemap address
BG4SC       = $210A     ; BG4 tilemap address
BG12NBA     = $210B     ; BG1/2 tile data address
BG34NBA     = $210C     ; BG3/4 tile data address
BG1HOFS     = $210D     ; BG1 horizontal scroll
BG1VOFS     = $210E     ; BG1 vertical scroll
BG2HOFS     = $210F     ; BG2 horizontal scroll
BG2VOFS     = $2110     ; BG2 vertical scroll
BG3HOFS     = $2111     ; BG3 horizontal scroll
BG3VOFS     = $2112     ; BG3 vertical scroll
BG4HOFS     = $2113     ; BG4 horizontal scroll
BG4VOFS     = $2114     ; BG4 vertical scroll
VMAIN       = $2115     ; VRAM address increment mode
VMADDL      = $2116     ; VRAM address low
VMADDH      = $2117     ; VRAM address high
VMDATAL     = $2118     ; VRAM data write low
VMDATAH     = $2119     ; VRAM data write high
M7SEL       = $211A     ; Mode 7 settings
M7A         = $211B     ; Mode 7 matrix A
M7B         = $211C     ; Mode 7 matrix B
M7C         = $211D     ; Mode 7 matrix C
M7D         = $211E     ; Mode 7 matrix D
M7X         = $211F     ; Mode 7 center X
M7Y         = $2120     ; Mode 7 center Y
CGADD       = $2121     ; CGRAM address
CGDATA      = $2122     ; CGRAM data write
W12SEL      = $2123     ; Window 1/2 mask settings for BG1/2
W34SEL      = $2124     ; Window 1/2 mask settings for BG3/4
WOBJSEL     = $2125     ; Window 1/2 mask settings for OBJ/Color
WH0         = $2126     ; Window 1 left position
WH1         = $2127     ; Window 1 right position
WH2         = $2128     ; Window 2 left position
WH3         = $2129     ; Window 2 right position
WBGLOG      = $212A     ; Window mask logic for BG
WOBJLOG     = $212B     ; Window mask logic for OBJ/Color
TM          = $212C     ; Main screen designation
TS          = $212D     ; Sub screen designation
TMW         = $212E     ; Window mask for main screen
TSW         = $212F     ; Window mask for sub screen
CGWSEL      = $2130     ; Color addition select
CGADSUB     = $2131     ; Color math designation
COLDATA     = $2132     ; Fixed color data
SETINI      = $2133     ; Screen mode/video select

; PPU Status Registers ($2134-$213F)
MPYL        = $2134     ; Multiplication result low
MPYM        = $2135     ; Multiplication result mid
MPYH        = $2136     ; Multiplication result high
SLHV        = $2137     ; Latch H/V counter
OAMDATAREAD = $2138     ; OAM data read
VMDATALREAD = $2139     ; VRAM data read low
VMDATAHREAD = $213A     ; VRAM data read high
CGDATAREAD  = $213B     ; CGRAM data read
OPHCT       = $213C     ; Horizontal scanline location
OPVCT       = $213D     ; Vertical scanline location
STAT77      = $213E     ; PPU status flag 1
STAT78      = $213F     ; PPU status flag 2

; APU Communication ($2140-$2143)
APUIO0      = $2140     ; APU I/O register 0
APUIO1      = $2141     ; APU I/O register 1
APUIO2      = $2142     ; APU I/O register 2
APUIO3      = $2143     ; APU I/O register 3

; WRAM Access ($2180-$2183)
WMDATA      = $2180     ; WRAM data read/write
WMADDL      = $2181     ; WRAM address low
WMADDM      = $2182     ; WRAM address mid
WMADDH      = $2183     ; WRAM address high (bank)

; CPU Registers ($4200-$421F)
NMITIMEN    = $4200     ; NMI/IRQ enable
WRIO        = $4201     ; Programmable I/O port
WRMPYA      = $4202     ; Multiplicand A
WRMPYB      = $4203     ; Multiplicand B
WRDIVL      = $4204     ; Dividend low
WRDIVH      = $4205     ; Dividend high
WRDIVB      = $4206     ; Divisor
HTIMEL      = $4207     ; H-count timer low
HTIMEH      = $4208     ; H-count timer high
VTIMEL      = $4209     ; V-count timer low
VTIMEH      = $420A     ; V-count timer high
MDMAEN      = $420B     ; DMA enable
HDMAEN      = $420C     ; HDMA enable
MEMSEL      = $420D     ; ROM speed

; CPU Status ($4210-$421F)
RDNMI       = $4210     ; NMI flag and CPU version
TIMEUP      = $4211     ; IRQ flag
HVBJOY      = $4212     ; PPU status
RDIO        = $4213     ; Programmable I/O port (input)
RDDIVL      = $4214     ; Division result low
RDDIVH      = $4215     ; Division result high
RDMPYL      = $4216     ; Multiplication result low
RDMPYH      = $4217     ; Multiplication result high
JOY1L       = $4218     ; Joypad 1 data low
JOY1H       = $4219     ; Joypad 1 data high
JOY2L       = $421A     ; Joypad 2 data low
JOY2H       = $421B     ; Joypad 2 data high
JOY3L       = $421C     ; Joypad 3 data low
JOY3H       = $421D     ; Joypad 3 data high
JOY4L       = $421E     ; Joypad 4 data low
JOY4H       = $421F     ; Joypad 4 data high

; DMA Registers ($4300-$437F, channels 0-7)
; Channel 0 shown, add $10 for each subsequent channel
DMAP0       = $4300     ; DMA parameters
BBAD0       = $4301     ; DMA B-bus address
A1T0L       = $4302     ; DMA A-bus address low
A1T0H       = $4303     ; DMA A-bus address high
A1B0        = $4304     ; DMA A-bus address bank
DAS0L       = $4305     ; DMA size low
DAS0H       = $4306     ; DMA size high
DASB0       = $4307     ; HDMA indirect bank
A2A0L       = $4308     ; HDMA table address low
A2A0H       = $4309     ; HDMA table address high
NTRL0       = $430A     ; HDMA line counter

; Joypad button masks
JOY_B       = $8000
JOY_Y       = $4000
JOY_SELECT  = $2000
JOY_START   = $1000
JOY_UP      = $0800
JOY_DOWN    = $0400
JOY_LEFT    = $0200
JOY_RIGHT   = $0100
JOY_A       = $0080
JOY_X       = $0040
JOY_L       = $0020
JOY_R       = $0010
