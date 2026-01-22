/**
 * sxtest - A GoogleTest-compatible verification harness for SNES ROMs
 *
 * This library wraps the Snes9x emulator core (via libretro interface),
 * providing a clean C++ interface for headless execution and memory
 * instrumentation.
 *
 * THREAD SAFETY WARNING:
 * The Snes9x emulator uses global state throughout its implementation.
 * This means:
 *   - Multiple SX::Emulator instances CANNOT run concurrently in threads
 *   - For parallel test execution, use process-based parallelism (fork())
 *
 * Basic usage example:
 *
 *   #include <sxtest.h>
 *
 *   class MyGameTest : public SX::Test {
 *   protected:
 *       void SetUp() override {
 *           LoadRom("game.sfc");
 *           RunFrames(60);  // Skip intro
 *       }
 *   };
 *
 *   TEST_F(MyGameTest, CheckMemoryState) {
 *       RunFrames(1);
 *       EXPECT_EQ(ReadByte(0x7E0000), 0x42);
 *   }
 */

#ifndef SXTEST_H
#define SXTEST_H

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <memory>

namespace SX {

/**
 * Input state for a SNES controller
 */
struct Input {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool a = false;
    bool b = false;
    bool x = false;
    bool y = false;
    bool l = false;
    bool r = false;
    bool start = false;
    bool select = false;

    void Clear() {
        up = down = left = right = false;
        a = b = x = y = false;
        l = r = false;
        start = select = false;
    }
};

/**
 * Emulator wrapper class providing the test harness interface
 */
class Emulator {
public:
    Emulator();
    ~Emulator();

    // Prevent copying
    Emulator(const Emulator&) = delete;
    Emulator& operator=(const Emulator&) = delete;

    /**
     * Load a ROM file from disk
     * @param path Path to the ROM file (.sfc, .smc)
     * @return true if loaded successfully
     */
    bool LoadRom(const std::string& path);

    /**
     * Load a ROM from memory buffer
     * @param data ROM data
     * @param size Size of ROM in bytes
     * @return true if loaded successfully
     */
    bool LoadRom(const uint8_t* data, size_t size);

    /**
     * Reset the emulated system (soft reset)
     */
    void Reset();

    /**
     * Run the emulator for a specified number of frames
     * @param frames Number of frames to execute
     */
    void RunFrames(int frames);

    /**
     * Run until a memory condition is met or max frames reached
     * @param address Memory address to monitor (SNES address space)
     * @param expected Expected value
     * @param max_frames Maximum frames before timeout
     * @return Frame number when condition was met, or -1 if timeout
     */
    int RunUntilMemoryEquals(uint32_t address, uint8_t expected, int max_frames);

    /**
     * Run until a callback returns true or max frames reached
     */
    int RunUntil(std::function<bool()> condition, int max_frames);

    // -------------------------------------------------------------------------
    // Memory Access (SNES address space)
    // -------------------------------------------------------------------------

    /** Read a byte from SNES address space */
    uint8_t ReadByte(uint32_t address) const;

    /** Read a 16-bit word (little-endian, native SNES) */
    uint16_t ReadWord(uint32_t address) const;

    /** Write a byte to SNES address space */
    void WriteByte(uint32_t address, uint8_t value);

    /** Write a 16-bit word */
    void WriteWord(uint32_t address, uint16_t value);

    // -------------------------------------------------------------------------
    // Direct RAM Access
    // -------------------------------------------------------------------------

    /** Direct access to WRAM (128KB at $7E0000-$7FFFFF) */
    uint8_t* GetWRAM();
    const uint8_t* GetWRAM() const;

    /** Direct access to VRAM (64KB) */
    uint8_t* GetVRAM();
    const uint8_t* GetVRAM() const;

    // -------------------------------------------------------------------------
    // Input Control
    // -------------------------------------------------------------------------

    /** Set input state for player 1 or 2 (0-indexed) */
    void SetInput(int player, const Input& input);

    /** Get current input state */
    const Input& GetInput(int player) const;

    /** Press a button for one frame, then release */
    void PressButton(int player, const std::string& button);

    // -------------------------------------------------------------------------
    // State Management
    // -------------------------------------------------------------------------

    /** Save current state to buffer */
    std::vector<uint8_t> SaveState() const;

    /** Load state from buffer */
    bool LoadState(const std::vector<uint8_t>& state);

    // -------------------------------------------------------------------------
    // Info
    // -------------------------------------------------------------------------

    /** Get current frame count since reset */
    uint64_t GetFrameCount() const;

    /** Get ROM header info */
    std::string GetRomName() const;

    /** Check if ROM is loaded */
    bool IsRomLoaded() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * GoogleTest fixture base class for SNES ROM testing
 */
class Test : public ::testing::Test {
protected:
    Emulator emu;

    // Convenience wrappers
    bool LoadRom(const std::string& path) { return emu.LoadRom(path); }
    bool LoadRom(const uint8_t* data, size_t size) { return emu.LoadRom(data, size); }
    void Reset() { emu.Reset(); }
    void RunFrames(int frames) { emu.RunFrames(frames); }

    uint8_t ReadByte(uint32_t addr) const { return emu.ReadByte(addr); }
    uint16_t ReadWord(uint32_t addr) const { return emu.ReadWord(addr); }

    void WriteByte(uint32_t addr, uint8_t val) { emu.WriteByte(addr, val); }
    void WriteWord(uint32_t addr, uint16_t val) { emu.WriteWord(addr, val); }

    void SetInput(int player, const Input& input) { emu.SetInput(player, input); }
    void PressButton(int player, const std::string& button) { emu.PressButton(player, button); }

    uint64_t GetFrameCount() const { return emu.GetFrameCount(); }
};

} // namespace SX

#endif // SXTEST_H
