#pragma once
#include "core/host_display.h"
#include <libretro.h>

class LibretroHostDisplay final : public HostDisplay
{
public:
  LibretroHostDisplay();
  ~LibretroHostDisplay();

  RenderAPI GetRenderAPI() const override;
  void* GetRenderDevice() const override;
  void* GetRenderContext() const override;

  bool CreateRenderDevice(const WindowInfo& wi, std::string_view adapter_name, bool debug_device,
                          bool threaded_presentation) override;
  bool InitializeRenderDevice(std::string_view shader_cache_directory, bool debug_device,
                              bool threaded_presentation) override;
  void DestroyRenderDevice() override;

  bool ChangeRenderWindow(const WindowInfo& wi) override;
  void ResizeRenderWindow(s32 new_window_width, s32 new_window_height) override;
  bool CreateResources() override;
  void DestroyResources() override;
  void RenderSoftwareCursor() override;

  std::unique_ptr<HostDisplayTexture> CreateTexture(u32 width, u32 height, u32 layers, u32 levels, u32 samples,
                                                    HostDisplayPixelFormat format, const void* data, u32 data_stride,
                                                    bool dynamic = false) override;
  bool Render() override;

  bool SupportsDisplayPixelFormat(HostDisplayPixelFormat format) const override;

  bool BeginSetDisplayPixels(HostDisplayPixelFormat format, u32 width, u32 height, void** out_buffer,
                             u32* out_pitch) override;
  void EndSetDisplayPixels() override;

private:
  bool CheckPixelFormat(retro_pixel_format new_format);

  std::vector<u32> m_frame_buffer;
  u32 m_frame_buffer_pitch = 0;
  retro_framebuffer m_software_fb = {};
  retro_pixel_format m_current_pixel_format = RETRO_PIXEL_FORMAT_UNKNOWN;
};
