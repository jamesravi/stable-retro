#include "libretro_audio_stream.h"
#include "libretro_host_interface.h"
#include <algorithm>
#include <array>

#ifndef AUDIO_CHANNELS
#define AUDIO_CHANNELS 2
#endif

LibretroAudioStream::LibretroAudioStream() = default;

LibretroAudioStream::~LibretroAudioStream() = default;

void LibretroAudioStream::UploadToFrontend()
{
  std::array<SampleType, MaxSamples> output_buffer;
  u32 total_samples = 0;
  while (const auto num_samples = m_buffer.GetContiguousSize()) {
    const auto write_pos = output_buffer.begin() + total_samples;
    std::copy_n(m_buffer.GetReadPointer(), num_samples, write_pos);
    m_buffer.Remove(num_samples);
    total_samples += num_samples;
  }
  g_retro_audio_sample_batch_callback(output_buffer.data(), total_samples / AUDIO_CHANNELS);
}

void LibretroAudioStream::FramesAvailable()
{
  if (!g_settings.audio_fast_hook)
  {
    for (;;)
    {
      const u32 num_samples = m_buffer.GetContiguousSize();
      if (num_samples == 0)
        break;

      const u32 num_frames = num_samples / AUDIO_CHANNELS;
      g_retro_audio_sample_batch_callback(m_buffer.GetReadPointer(), num_frames);
      m_buffer.Remove(num_samples);
    }
  }
}
