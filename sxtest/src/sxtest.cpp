/**
 * sxtest - Implementation of the Snes9x test harness
 */

#include "sxtest.h"
#include <cstring>
#include <fstream>

// Libretro API
#include "libretro.h"

// Snes9x internals for direct memory access
extern "C" {
// Forward declarations for snes9x globals we need
extern uint8_t* Memory_RAM;      // WRAM (128KB)
extern uint8_t* Memory_VRAM;     // VRAM (64KB)
}

namespace SX {

// ---------------------------------------------------------------------------
// Libretro Callbacks (headless stubs)
// ---------------------------------------------------------------------------

// Input state storage
static Input g_inputs[2];
static uint64_t g_frame_count = 0;
static bool g_rom_loaded = false;
static std::vector<uint8_t> g_rom_data;

// Stub callbacks for headless operation
static void stub_video_refresh(const void* data, unsigned width, unsigned height, size_t pitch) {
    // Discard video output in headless mode
    (void)data; (void)width; (void)height; (void)pitch;
}

static void stub_audio_sample(int16_t left, int16_t right) {
    // Discard audio in headless mode
    (void)left; (void)right;
}

static size_t stub_audio_sample_batch(const int16_t* data, size_t frames) {
    // Discard audio in headless mode
    (void)data;
    return frames;
}

static void stub_input_poll() {
    // Nothing to poll in headless mode
}

static int16_t stub_input_state(unsigned port, unsigned device, unsigned index, unsigned id) {
    (void)index;
    if (port >= 2) return 0;
    if (device != RETRO_DEVICE_JOYPAD) return 0;

    const Input& inp = g_inputs[port];

    switch (id) {
        case RETRO_DEVICE_ID_JOYPAD_UP:     return inp.up ? 1 : 0;
        case RETRO_DEVICE_ID_JOYPAD_DOWN:   return inp.down ? 1 : 0;
        case RETRO_DEVICE_ID_JOYPAD_LEFT:   return inp.left ? 1 : 0;
        case RETRO_DEVICE_ID_JOYPAD_RIGHT:  return inp.right ? 1 : 0;
        case RETRO_DEVICE_ID_JOYPAD_A:      return inp.a ? 1 : 0;
        case RETRO_DEVICE_ID_JOYPAD_B:      return inp.b ? 1 : 0;
        case RETRO_DEVICE_ID_JOYPAD_X:      return inp.x ? 1 : 0;
        case RETRO_DEVICE_ID_JOYPAD_Y:      return inp.y ? 1 : 0;
        case RETRO_DEVICE_ID_JOYPAD_L:      return inp.l ? 1 : 0;
        case RETRO_DEVICE_ID_JOYPAD_R:      return inp.r ? 1 : 0;
        case RETRO_DEVICE_ID_JOYPAD_START:  return inp.start ? 1 : 0;
        case RETRO_DEVICE_ID_JOYPAD_SELECT: return inp.select ? 1 : 0;
        default: return 0;
    }
}

static bool stub_environment(unsigned cmd, void* data) {
    switch (cmd) {
        case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
            // Don't provide logging - silent operation
            return false;

        case RETRO_ENVIRONMENT_GET_CAN_DUPE:
            if (data) *(bool*)data = true;
            return true;

        case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
            // Accept any pixel format
            return true;

        case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
        case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
            if (data) *(const char**)data = ".";
            return true;

        case RETRO_ENVIRONMENT_SET_VARIABLES:
        case RETRO_ENVIRONMENT_GET_VARIABLE:
        case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
            return false;

        default:
            return false;
    }
}

// ---------------------------------------------------------------------------
// Emulator::Impl - Private implementation
// ---------------------------------------------------------------------------

class Emulator::Impl {
public:
    bool initialized = false;

    Impl() {
        // Set up libretro callbacks
        retro_set_environment(stub_environment);
        retro_set_video_refresh(stub_video_refresh);
        retro_set_audio_sample(stub_audio_sample);
        retro_set_audio_sample_batch(stub_audio_sample_batch);
        retro_set_input_poll(stub_input_poll);
        retro_set_input_state(stub_input_state);

        // Initialize the core
        retro_init();
        initialized = true;
        g_rom_loaded = false;
        g_frame_count = 0;
    }

    ~Impl() {
        if (g_rom_loaded) {
            retro_unload_game();
        }
        if (initialized) {
            retro_deinit();
        }
    }

    bool LoadRomData(const uint8_t* data, size_t size) {
        if (size == 0) return false;

        // Unload previous ROM if any
        if (g_rom_loaded) {
            retro_unload_game();
            g_rom_loaded = false;
        }

        // Store ROM data (libretro may reference it)
        g_rom_data.assign(data, data + size);

        // Set up game info
        retro_game_info game_info = {};
        game_info.path = nullptr;
        game_info.data = g_rom_data.data();
        game_info.size = g_rom_data.size();
        game_info.meta = nullptr;

        // Load the ROM
        if (!retro_load_game(&game_info)) {
            g_rom_data.clear();
            return false;
        }

        g_rom_loaded = true;
        g_frame_count = 0;
        g_inputs[0].Clear();
        g_inputs[1].Clear();

        return true;
    }

    void RunFrame() {
        if (!g_rom_loaded) return;
        retro_run();
        g_frame_count++;
    }
};

// ---------------------------------------------------------------------------
// Emulator - Public interface implementation
// ---------------------------------------------------------------------------

Emulator::Emulator() : pImpl(new Impl()) {}

Emulator::~Emulator() {
    delete pImpl;
}

bool Emulator::LoadRom(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return false;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        return false;
    }

    return pImpl->LoadRomData(buffer.data(), buffer.size());
}

bool Emulator::LoadRom(const uint8_t* data, size_t size) {
    return pImpl->LoadRomData(data, size);
}

void Emulator::Reset() {
    if (g_rom_loaded) {
        retro_reset();
        g_frame_count = 0;
    }
}

void Emulator::RunFrames(int frames) {
    for (int i = 0; i < frames; i++) {
        pImpl->RunFrame();
    }
}

int Emulator::RunUntilMemoryEquals(uint32_t address, uint8_t expected, int max_frames) {
    for (int i = 0; i < max_frames; i++) {
        if (ReadByte(address) == expected) {
            return i;
        }
        pImpl->RunFrame();
    }
    return -1;
}

int Emulator::RunUntil(std::function<bool()> condition, int max_frames) {
    for (int i = 0; i < max_frames; i++) {
        if (condition()) {
            return i;
        }
        pImpl->RunFrame();
    }
    return -1;
}

// ---------------------------------------------------------------------------
// Memory Access
// ---------------------------------------------------------------------------

uint8_t Emulator::ReadByte(uint32_t address) const {
    // Use libretro memory interface
    // For WRAM: addresses $7E0000-$7FFFFF map to libretro RETRO_MEMORY_SYSTEM_RAM
    if (address >= 0x7E0000 && address <= 0x7FFFFF) {
        size_t wram_size = retro_get_memory_size(RETRO_MEMORY_SYSTEM_RAM);
        uint8_t* wram = static_cast<uint8_t*>(retro_get_memory_data(RETRO_MEMORY_SYSTEM_RAM));
        if (wram && (address - 0x7E0000) < wram_size) {
            return wram[address - 0x7E0000];
        }
    }
    // TODO: Add support for other memory regions via memory map
    return 0;
}

uint16_t Emulator::ReadWord(uint32_t address) const {
    // SNES is little-endian
    return static_cast<uint16_t>(ReadByte(address)) |
           (static_cast<uint16_t>(ReadByte(address + 1)) << 8);
}

void Emulator::WriteByte(uint32_t address, uint8_t value) {
    if (address >= 0x7E0000 && address <= 0x7FFFFF) {
        size_t wram_size = retro_get_memory_size(RETRO_MEMORY_SYSTEM_RAM);
        uint8_t* wram = static_cast<uint8_t*>(retro_get_memory_data(RETRO_MEMORY_SYSTEM_RAM));
        if (wram && (address - 0x7E0000) < wram_size) {
            wram[address - 0x7E0000] = value;
        }
    }
}

void Emulator::WriteWord(uint32_t address, uint16_t value) {
    WriteByte(address, value & 0xFF);
    WriteByte(address + 1, value >> 8);
}

uint8_t* Emulator::GetWRAM() {
    return static_cast<uint8_t*>(retro_get_memory_data(RETRO_MEMORY_SYSTEM_RAM));
}

const uint8_t* Emulator::GetWRAM() const {
    return static_cast<const uint8_t*>(retro_get_memory_data(RETRO_MEMORY_SYSTEM_RAM));
}

uint8_t* Emulator::GetVRAM() {
    return static_cast<uint8_t*>(retro_get_memory_data(RETRO_MEMORY_VIDEO_RAM));
}

const uint8_t* Emulator::GetVRAM() const {
    return static_cast<const uint8_t*>(retro_get_memory_data(RETRO_MEMORY_VIDEO_RAM));
}

// ---------------------------------------------------------------------------
// Input Control
// ---------------------------------------------------------------------------

void Emulator::SetInput(int player, const Input& input) {
    if (player >= 0 && player < 2) {
        g_inputs[player] = input;
    }
}

const Input& Emulator::GetInput(int player) const {
    static Input empty;
    if (player >= 0 && player < 2) {
        return g_inputs[player];
    }
    return empty;
}

void Emulator::PressButton(int player, const std::string& button) {
    if (player < 0 || player > 1) return;

    Input& inp = g_inputs[player];

    if (button == "up") inp.up = true;
    else if (button == "down") inp.down = true;
    else if (button == "left") inp.left = true;
    else if (button == "right") inp.right = true;
    else if (button == "a" || button == "A") inp.a = true;
    else if (button == "b" || button == "B") inp.b = true;
    else if (button == "x" || button == "X") inp.x = true;
    else if (button == "y" || button == "Y") inp.y = true;
    else if (button == "l" || button == "L") inp.l = true;
    else if (button == "r" || button == "R") inp.r = true;
    else if (button == "start" || button == "Start") inp.start = true;
    else if (button == "select" || button == "Select") inp.select = true;

    pImpl->RunFrame();
    inp.Clear();
}

// ---------------------------------------------------------------------------
// State Management
// ---------------------------------------------------------------------------

std::vector<uint8_t> Emulator::SaveState() const {
    size_t size = retro_serialize_size();
    if (size == 0) return {};

    std::vector<uint8_t> buffer(size);
    if (!retro_serialize(buffer.data(), size)) {
        return {};
    }

    return buffer;
}

bool Emulator::LoadState(const std::vector<uint8_t>& state) {
    if (state.empty()) return false;
    return retro_unserialize(state.data(), state.size());
}

// ---------------------------------------------------------------------------
// Info
// ---------------------------------------------------------------------------

uint64_t Emulator::GetFrameCount() const {
    return g_frame_count;
}

std::string Emulator::GetRomName() const {
    if (!g_rom_loaded || g_rom_data.size() < 0x8000) return "";

    // SNES ROM name is at offset $FFC0 (LoROM) or $00FFC0 (HiROM) in the header
    // For simplicity, try LoROM first (header at $7FC0)
    size_t header_offset = 0x7FC0;
    if (g_rom_data.size() > header_offset + 21) {
        char name[22] = {0};
        memcpy(name, &g_rom_data[header_offset], 21);
        // Trim trailing spaces
        for (int i = 20; i >= 0 && name[i] == ' '; i--) {
            name[i] = '\0';
        }
        return std::string(name);
    }

    return "";
}

bool Emulator::IsRomLoaded() const {
    return g_rom_loaded;
}

} // namespace SX
