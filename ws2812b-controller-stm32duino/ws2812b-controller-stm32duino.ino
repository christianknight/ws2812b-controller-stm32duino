#include <WS2812B.h>

#define NUM_LEDS 183

WS2812B strip = WS2812B(NUM_LEDS);

uint8_t led_gamma[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
  2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
  5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};

bool state = true;
int brightness = 15;
String command_string;

void
setup() {
  strip.begin();
  strip.show();
  strip.setBrightness(brightness);

  Serial.begin(115200);
  Serial1.begin(115200);
}

void
loop() {
  if (state == true) {
    for (int j = 0; j < (256 * 5); j++) {
      for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, wheel(((i * 256 / strip.numPixels()) + j) & 255));
      }
      strip.show();
      delay(10);

      if (Serial1.available()) {
        process_serial(&Serial1);
        if (!state) {
          break;
        }
      }
      else if (Serial.available()) {
        process_serial(&Serial);
        if (!state) {
          break;
        }
      }
    }
  }
  else {
    /* Blank the LED strip */
    strip.clear();
    strip.show();

    /* Wait for the LED strip to be re-enabled over serial */
    while (!Serial.available() && !Serial1.available());
    if (Serial1.available()) {
      process_serial(&Serial1);
    }
    else if (Serial.available()) {
      process_serial(&Serial);
    }
  }
}

void
process_serial(Stream * s) {
  /* Read the next available byte from the given serial stream */
  char byte_read = s->read();
  command_string += byte_read;

  /* Copy the read byte to the other serial stream */
  if (s == &Serial) {
    Serial1.write(byte_read);
  }
  else if (s == &Serial1) {
    Serial.write(byte_read);
  }

  /* Chech if the incomming command is complete */
  check_command_complete(&command_string);
}

void
check_command_complete(String * command) {
  if (command->indexOf("\r\n") != -1) {
    brightness = (command->substring(0, command->indexOf("\r\n"))).toInt();
    strip.setBrightness(brightness);

    command->remove(0);
  }
}

/* Input an 8-bit angular position value to get a 32-bit RGB color value.
   The RGB output values transition from r -> g -> b -> r. */
uint32_t
wheel(byte wheel_pos) {
  if (wheel_pos < 85) {
    return strip.Color(wheel_pos * 3, 255 - wheel_pos * 3, 0);
  } 
  else {
    if (wheel_pos < 170) {
     wheel_pos -= 85;
     return strip.Color(255 - wheel_pos * 3, 0, wheel_pos * 3);
    }
    else {
      wheel_pos -= 170;
      return strip.Color(0, wheel_pos * 3, 255 - wheel_pos * 3);
    }
  }
}
