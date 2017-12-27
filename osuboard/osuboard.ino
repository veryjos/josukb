#include <usb_keyboard.h>
#include <Bounce.h>

// Cherry MX blues are rated for a 5ms debounce time
#define DEBOUNCE_TIME 5

typedef void (usb_keyboard_class::*keysetter_t)(uint8_t);

typedef struct key {
  int pin;
  int keycode;
  Bounce* bounce;
  keysetter_t setter;
} padkey_t;

// Initialize a list of keys
padkey_t keys[] = {
  { 20, KEY_Z, 0, &usb_keyboard_class::set_key1 },
  {  5, KEY_X, 0, &usb_keyboard_class::set_key2 }
};

int num_keys = sizeof(keys) / sizeof(keys[0]);

void setup() {
  for (int i=0;i<num_keys;++i) {
    padkey_t* k = &keys[i];

    // Initialize the pullup resistor for each key    
    pinMode(k->pin, INPUT_PULLUP);

    // Initialize bounce for each key
    k->bounce = new Bounce(k->pin, DEBOUNCE_TIME);
  }
}

void loop() {
  // Loop over each key, detect a state change, and update the keyboard if so
  bool keystate_changed = false;

  for (int i=0;i<num_keys;++i) {
    padkey_t* k = &keys[i];

    k->bounce->update();
    if (k->bounce->fallingEdge()) {
      (Keyboard.*(k->setter))(k->keycode);

      keystate_changed = true;
    } else if (k->bounce->risingEdge()) {
      (Keyboard.*(k->setter))(0);

      keystate_changed = true;
    }
  }

  if (keystate_changed) {
    Keyboard.send_now();
  }
}
