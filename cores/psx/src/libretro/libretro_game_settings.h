#pragma once
#include "core/types.h"
#include <bitset>
#include <optional>
#include <string>
#include <memory>

class ByteStream;

namespace GameSettings {
enum class Trait : u32
{
  ForceInterpreter,
  ForceSoftwareRenderer,
  ForceSoftwareRendererForReadbacks,
  ForceInterlacing,
  DisableTrueColor,
  DisableUpscaling,
  DisableAnalogModeForcing,
  DisableScaledDithering,
  DisableForceNTSCTimings,
  DisableWidescreen,
  DisablePGXP,
  DisablePGXPCulling,
  DisablePGXPTextureCorrection,
  DisablePGXPColorCorrection,
  DisablePGXPDepthBuffer,
  ForcePGXPVertexCache,
  ForcePGXPCPUMode,
  ForceRecompilerMemoryExceptions,
  ForceRecompilerICache,
  ForceRecompilerLUTFastmem,
  ForceOldAudioHook,

  Count
};

struct Entry
{
  std::bitset<static_cast<int>(Trait::Count)> traits{};
  std::optional<s16> display_active_start_offset;
  std::optional<s16> display_active_end_offset;
  std::optional<s8> display_line_start_offset;
  std::optional<s8> display_line_end_offset;
  std::optional<u32> dma_max_slice_ticks;
  std::optional<u32> dma_halt_ticks;
  std::optional<u32> gpu_fifo_size;
  std::optional<u32> gpu_max_run_ahead;
  std::optional<float> gpu_pgxp_tolerance;
  std::optional<float> gpu_pgxp_depth_threshold;

  // user settings
  std::optional<u32> runahead_frames;
  std::optional<u32> cpu_overclock_numerator;
  std::optional<u32> cpu_overclock_denominator;
  std::optional<bool> cpu_overclock_enable;
  std::optional<bool> enable_8mb_ram;
  std::optional<u32> cdrom_read_speedup;
  std::optional<u32> cdrom_seek_speedup;
  std::optional<DisplayCropMode> display_crop_mode;
  std::optional<DisplayAspectRatio> display_aspect_ratio;
  std::optional<GPURenderer> gpu_renderer;
  std::optional<bool> gpu_use_software_renderer_for_readbacks;
  std::optional<GPUDownsampleMode> gpu_downsample_mode;
  std::optional<bool> display_force_4_3_for_24bit;
  std::optional<u16> display_aspect_ratio_custom_numerator;
  std::optional<u16> display_aspect_ratio_custom_denominator;
  std::optional<u32> gpu_resolution_scale;
  std::optional<u32> gpu_multisamples;
  std::optional<bool> gpu_per_sample_shading;
  std::optional<bool> gpu_true_color;
  std::optional<bool> gpu_scaled_dithering;
  std::optional<bool> gpu_force_ntsc_timings;
  std::optional<GPUTextureFilter> gpu_texture_filter;
  std::optional<bool> gpu_widescreen_hack;
  std::optional<bool> gpu_pgxp;
  std::optional<bool> gpu_pgxp_projection_precision;
  std::optional<bool> gpu_pgxp_depth_buffer;
  std::optional<MultitapMode> multitap_mode;
  std::optional<ControllerType> controller_1_type;
  std::optional<ControllerType> controller_2_type;
  std::optional<MemoryCardType> memory_card_1_type;
  std::optional<MemoryCardType> memory_card_2_type;

  ALWAYS_INLINE bool HasTrait(Trait trait) const { return traits[static_cast<int>(trait)]; }
  ALWAYS_INLINE void AddTrait(Trait trait) { traits[static_cast<int>(trait)] = true; }

  void ApplySettings(bool display_osd_messages) const;
};

}; // namespace GameSettings

std::unique_ptr<GameSettings::Entry> GetSettingsForGame(const std::string& game_code);
