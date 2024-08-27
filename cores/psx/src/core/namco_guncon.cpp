#include "namco_guncon.h"
#include "common/state_wrapper.h"
#include "gpu.h"
#include "host_display.h"
#include "host_interface.h"
#include "resources.h"
#include "system.h"
#include <array>

NamcoGunCon::NamcoGunCon() = default;

NamcoGunCon::~NamcoGunCon() = default;

ControllerType NamcoGunCon::GetType() const
{
  return ControllerType::NamcoGunCon;
}

void NamcoGunCon::Reset()
{
  m_transfer_state = TransferState::Idle;
}

bool NamcoGunCon::DoState(StateWrapper& sw, bool apply_input_state)
{
  if (!Controller::DoState(sw, apply_input_state))
    return false;

  u16 button_state = m_button_state;
  u16 position_x = m_position_x;
  u16 position_y = m_position_y;
  sw.Do(&button_state);
  sw.Do(&position_x);
  sw.Do(&position_y);
  if (apply_input_state)
  {
    m_button_state = button_state;
    m_position_x = position_x;
    m_position_y = position_y;
  }

  sw.Do(&m_transfer_state);
  return true;
}

void NamcoGunCon::SetButtonState(Button button, bool pressed)
{
  if (button == Button::ShootOffscreen)
  {
    if (m_shoot_offscreen != pressed)
    {
      m_shoot_offscreen = pressed;
      SetButtonState(Button::Trigger, pressed);
    }

    return;
  }

  static constexpr std::array<u8, static_cast<size_t>(Button::Count)> indices = {{13, 3, 14}};
  if (pressed)
    m_button_state &= ~(u16(1) << indices[static_cast<u8>(button)]);
  else
    m_button_state |= u16(1) << indices[static_cast<u8>(button)];
}

void NamcoGunCon::SetButtonState(s32 button_code, bool pressed)
{
  if (button_code < 0 || button_code >= static_cast<s32>(Button::Count))
    return;

  SetButtonState(static_cast<Button>(button_code), pressed);
}

void NamcoGunCon::ResetTransferState()
{
  m_transfer_state = TransferState::Idle;
}

bool NamcoGunCon::Transfer(const u8 data_in, u8* data_out)
{
  static constexpr u16 ID = 0x5A63;

  switch (m_transfer_state)
  {
    case TransferState::Idle:
    {
      *data_out = 0xFF;

      if (data_in == 0x01)
      {
        m_transfer_state = TransferState::Ready;
        return true;
      }
      break;
    }

    case TransferState::Ready:
    {
      if (data_in == 0x42)
      {
        *data_out = Truncate8(ID);
        m_transfer_state = TransferState::IDMSB;
        return true;
      }

      *data_out = 0xFF;
      break;
    }

    case TransferState::IDMSB:
    {
      *data_out = Truncate8(ID >> 8);
      m_transfer_state = TransferState::ButtonsLSB;
      return true;
    }

    case TransferState::ButtonsLSB:
    {
      *data_out = Truncate8(m_button_state);
      m_transfer_state = TransferState::ButtonsMSB;
      return true;
    }

    case TransferState::ButtonsMSB:
    {
      *data_out = Truncate8(m_button_state >> 8);
      m_transfer_state = TransferState::XLSB;
      return true;
    }

    case TransferState::XLSB:
    {
      UpdatePosition();
      *data_out = Truncate8(m_position_x);
      m_transfer_state = TransferState::XMSB;
      return true;
    }

    case TransferState::XMSB:
    {
      *data_out = Truncate8(m_position_x >> 8);
      m_transfer_state = TransferState::YLSB;
      return true;
    }

    case TransferState::YLSB:
    {
      *data_out = Truncate8(m_position_y);
      m_transfer_state = TransferState::YMSB;
      return true;
    }

    case TransferState::YMSB:
    {
      *data_out = Truncate8(m_position_y >> 8);
      m_transfer_state = TransferState::Idle;
      break;
    }

    default:
      break;
  }

  return false;
}

void NamcoGunCon::UpdatePosition()
{
  // get screen coordinates
  const HostDisplay* display = g_host_interface->GetDisplay();
  const s32 mouse_x = display->GetMousePositionX();
  const s32 mouse_y = display->GetMousePositionY();

  // are we within the active display area?
  u32 tick, line;
  if (mouse_x < 0 || mouse_y < 0 ||
      !g_gpu->ConvertScreenCoordinatesToBeamTicksAndLines(mouse_x, mouse_y, m_x_scale, m_y_scale, &tick, &line) ||
      m_shoot_offscreen)
  {
    m_position_x = 0x01;
    m_position_y = 0x0A;
    return;
  }

  // 8MHz units for X = 44100*768*11/7 = 53222400 / 8000000 = 6.6528
  const double divider = static_cast<double>(g_gpu->GetCRTCFrequency()) / 8000000.0;
  m_position_x = static_cast<u16>(static_cast<float>(tick) / static_cast<float>(divider));
  m_position_y = static_cast<u16>(line);
}

std::unique_ptr<NamcoGunCon> NamcoGunCon::Create()
{
  return std::make_unique<NamcoGunCon>();
}

u32 NamcoGunCon::StaticGetVibrationMotorCount()
{
  return 0;
}

void NamcoGunCon::LoadSettings(const char* section)
{
  Controller::LoadSettings(section);

  m_crosshair_image.SetPixels(Resources::CROSSHAIR_IMAGE_WIDTH, Resources::CROSSHAIR_IMAGE_HEIGHT,
                              Resources::CROSSHAIR_IMAGE_DATA.data());

  m_crosshair_image_scale = 1.0f;

  m_x_scale = g_host_interface->GetFloatSettingValue(section, "XScale", 1.0f);

  m_y_scale = g_host_interface->GetFloatSettingValue(section, "YScale", 1.0f);
}

bool NamcoGunCon::GetSoftwareCursor(const Common::RGBA8Image** image, float* image_scale, bool* relative_mode)
{
  *image = &m_crosshair_image;
  *image_scale = m_crosshair_image_scale;
  *relative_mode = false;
  return true;
}
