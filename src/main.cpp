#include <pico/stdio.h>

#include <bsp/board_api.h>
#include <tusb.h>
#include <hardware/gpio.h>
#include <iostream>

#include "repl/Repl.h"
#include "ProfileManagement/Profile.h"
#include "ProfileManagement/Manager.h"
#include "ProfileManagement/ProfileExecutor.h"
#include "displaylib_16/ili9341.hpp"
#include "ui/ProfileSelectionScreen.h"
#include "driver/MCP3204.h"

#include "debug/lapTimer.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

#define BIT(n) (1UL << n)

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void led_blinking_task();
void configure_pins();
ILI9341_TFT tft_init();
TCFW::MCP3204 adc_init();

/*------------- MAIN -------------*/
int main(void) {
    board_init();

    // init device stack on configured roothub port
    tud_init(BOARD_TUD_RHPORT);

    if (board_init_after_tusb) {
        board_init_after_tusb();
    }

    stdio_init_all();
    configure_pins();
    sleep_ms(1000);
    auto myLCD = tft_init();
    auto myADC = TCFW::MCP3204(spi1);

    TCFW::Repl repl {};
    TCFW::ProfileManagement::InputGamepad input {};
    input.registerHatSource(GAMEPAD_HAT_UP, []() -> bool {return !gpio_get(4);});
    input.registerHatSource(GAMEPAD_HAT_DOWN, []() -> bool {return !gpio_get(5);});
    input.registerHatSource(GAMEPAD_HAT_RIGHT, []() -> bool {return !gpio_get(6);});
    input.registerHatSource(GAMEPAD_HAT_LEFT, []() -> bool {return !gpio_get(7);});
    input.registerStateSource(GAMEPAD_BUTTON_THUMBL, []() -> bool {return !gpio_get(2);});
    input.registerStateSource(GAMEPAD_BUTTON_THUMBR, []() -> bool {return !gpio_get(3);});
    input.registerStateSource(GAMEPAD_BUTTON_NORTH, []() -> bool {return !gpio_get(8);});
    input.registerStateSource(GAMEPAD_BUTTON_SOUTH, []() -> bool {return !gpio_get(9);});
    input.registerStateSource(GAMEPAD_BUTTON_EAST, []() -> bool {return !gpio_get(14);});
    input.registerStateSource(GAMEPAD_BUTTON_WEST, []() -> bool {return !gpio_get(15);});
    input.registerStateSource(GAMEPAD_BUTTON_TR, []() -> bool {return !gpio_get(21);});
    input.registerStateSource(GAMEPAD_BUTTON_TL, []() -> bool {return !gpio_get(22);});
    input.registerStateSource(GAMEPAD_BUTTON_TR2, []() -> bool {return !gpio_get(26);});
    input.registerStateSource(GAMEPAD_BUTTON_TL2, []() -> bool {return !gpio_get(27);});

    TCFW::ProfileManagement::OutputGamepad output {myADC};
    TCFW::ProfileManagement::ProfileExecutor executor{input, output};

    TCFW::ProfileManagement::Manager profileManager {executor};
    repl.registerExecutable("profile", std::make_shared<TCFW::ProfileManagement::ProfileManagerExecutable>(profileManager));
    auto initProfile = TCFW::ProfileManagement::ProfileManagerExecutable(profileManager);
    std::vector<std::string> initOutput {};
    initProfile.execute({"profile", "create", "Internal"}, initOutput);
    initProfile.execute({"profile", "create", "Triple Burst"}, initOutput);
    initProfile.execute({"profile", "set", "1", "GAMEPAD_BUTTON_NORTH", "press", "vGAMEPAD_BUTTON_NORTH", "200", "^GAMEPAD_BUTTON_NORTH", "100", "vGAMEPAD_BUTTON_NORTH", "200", "^GAMEPAD_BUTTON_NORTH", "100", "vGAMEPAD_BUTTON_NORTH", "200", "^GAMEPAD_BUTTON_NORTH", "100"}, initOutput);
    auto profile = profileManager.getProfiles().at(0).lock();
    for (auto &mapping : profile->mapping) {
        auto button = mapping.button;
        auto pressEvent = std::make_shared<TCFW::ProfileManagement::ActionEvent>([button]() {
            TCFW::UI::UIScreen::notifyEvent(button);
        });

        mapping.onPress = TCFW::ProfileManagement::EventSequence({{
            TCFW::ProfileManagement::EventCombo({pressEvent})
        }});
        mapping.onRelease = {};
    }
    profileManager.loadProfile(0);

    TCFW::UI::UIScreen::pushChild(std::make_shared<TCFW::UI::ProfileSelectScreen>(myLCD, profileManager));

    auto last = board_millis();
    bool lastHomePinState = false;
    while (1) {
        LAP_TIMER_CLEAR();
        tud_task(); // tinyusb device task
        LAP_TIMER_CHECKPOINT("TUD");
        led_blinking_task();
        LAP_TIMER_CHECKPOINT("LED");
        repl.task();
        LAP_TIMER_CHECKPOINT("REPL");
        bool homePinState = gpio_get(28);
        if (!lastHomePinState && homePinState) {
            TCFW::UI::UIScreen::reset();
            profileManager.loadProfile(0);
            TCFW::UI::UIScreen::pushChild(std::make_shared<TCFW::UI::ProfileSelectScreen>(myLCD, profileManager));
        }
        lastHomePinState = homePinState;
        TCFW::UI::UIScreen::task();
        LAP_TIMER_CHECKPOINT("UI");
        input.task();
        LAP_TIMER_CHECKPOINT("INPUT");
        executor.task();
        LAP_TIMER_CHECKPOINT("EXECUTE");
        output.task();
        LAP_TIMER_CHECKPOINT("OUTPUT");

//        LAP_TIMER_METRICS();
    }
}

void configure_pins() {
    auto pinMask =
            BIT(2) |
            BIT(3) |
            BIT(4) |
            BIT(5) |
            BIT(6) |
            BIT(7) |
            BIT(8) |
            BIT(9) |
            BIT(14) |
            BIT(15) |
            BIT(21) |
            BIT(22) |
            BIT(26) |
            BIT(27) |
            BIT(28);
    gpio_init_mask(pinMask);
    gpio_set_dir_masked(pinMask, GPIO_IN);
    for(uint i=0;i<NUM_BANK0_GPIOS;i++) {
        if (pinMask & 1) {
            gpio_pull_up(i);
        }
        pinMask >>= 1;
    }
    gpio_init(20);
}

ILI9341_TFT tft_init() {
    auto myTFT = ILI9341_TFT();
    myTFT.SetupSPI(8000, spi0);
    myTFT.SetupGPIO(-1, 20, 17, 18, 19);
    myTFT.SetupScreenSize(240, 320);
    myTFT.ILI9341Initialize();
    myTFT.setRotation(displaylib_16_graphics::Degrees_270);
    myTFT.fillScreen(ILI9341_TFT::C_BLACK);

    return myTFT;
}



//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance;
  (void) len;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // blink is disabled
  if (!blink_interval_ms) return;

  // Blink every interval ms
  if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}
