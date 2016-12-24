
/// ------------------------------------------------------------------------------------------
/// \file Nunmouse - Teensy program to use a modified Nunchuck as a mouse.
///
/// \author 2016 Christophe BLAESS <christophe@blaess.fr>
///
/// \license GPL
/// ------------------------------------------------------------------------------------------

// Remember to Select "Mouse" in "Tools-> USB Type" menu.


// The nunchuck is powered up by the D17 output of the Teensy card.
#define POWER_UP_PIN  17

// The Button 1 (Z) is connected to the D20 input of the Teensy.
#define BUTTON_1_PIN  20

// The Button 2 (C) is connected to the D21 input of the Teensy.
#define BUTTON_2_PIN  21

// The central pin of the lateral potentiometer is connected to the A5 input of the Teensy.
#define POSITION_X_INPUT  5

// The central pin of the axial potentiometer is connected to the A4 input of the Teensy.
#define POSITION_Y_INPUT  4

// The delay between two measures in millisec.
#define LOOP_DELAY    15

// The tolerance around the rest position.
#define REST_POSITION_TOLERANCE  5 


/// \brief Power up the nunchuck and configure the digital I/O used to read the buttons.
///
static void nunchuck_init(void)
{
  // Digital output for the Nunchuck Vcc.
  pinMode(POWER_UP_PIN, OUTPUT);
  digitalWrite(POWER_UP_PIN, HIGH);

  // Digital input for the Button 1
  pinMode(BUTTON_1_PIN, INPUT);

  // Digital input  for the Button 2
  pinMode(BUTTON_2_PIN, INPUT);
  
  delay(100);
}


/// \brief Read the position of the joystick and the status of the buttons.
///
/// \param joystick_x: a pointer that (if not NULL) will be filled the X position of the joystick.
/// \param joystick_y: a pointer that (if not NULL) will be filled the Y position of the joystick.
/// \param button_1: a pointer that (if not NULL) will be filled the status of the upper button.
/// \param button_2: a pointer that (if not NULL) will be filled the status of the lower button.
///
static void nunchuck_read_status(int *joystick_x, int *joystick_y, int * button_1, int *button_2)
{
  if (joystick_x != NULL)
    (*joystick_x) = analogRead(POSITION_X_INPUT);

  if (joystick_y != NULL)
    (*joystick_y) = analogRead(POSITION_Y_INPUT);
    
  if (button_1 != NULL)
    (*button_1) = (digitalRead(BUTTON_1_PIN) == 0);

  if (button_2 != NULL)
    (*button_2) = (digitalRead(BUTTON_2_PIN) == 0);
}



// With NUNMOUSE_DEBUG defined, The teensy acts no more as a mouse but as a serial
// port, and displays the values.
// Remember to Select "Serial" in "Tools-> USB Type" menu.
//
//#define NUNMOUSE_DEBUG
 
// The rest position of the joystick (position at power up).
static int rest_x;
static int rest_y;

// The maximal and minimal positions of the joystick.
static int max_x;
static int max_y;

static int min_x;
static int min_y;



/// \brief Initialization of the microcontroler program.
///
void setup()
{
  #ifdef NUNMOUSE_DEBUG
    Serial.begin(115200);
  #endif

  nunchuck_init();
  nunchuck_read_status(&rest_x, &rest_y, NULL, NULL);
  delay(100);
  nunchuck_read_status(&rest_x, &rest_y, NULL, NULL);
  
  max_x = rest_x + 1;
  min_x = rest_x - 1;
  max_y = rest_y + 1;
  min_y = rest_y - 1;  
}



/// \brief Main microcontroler loop.
///
void loop()
{
  static int counter = 0;

  static int x = 0;
  static int y = 0;
  static int button_1 = 0;
  static int button_2 = 0;
  static int prev_button_1 = 0;
  static int prev_button_2 = 0;
  
  int x_mouse = 0;
  int y_mouse = 0;

  // The counter is used only in debug mode to see lines scrolling.
  counter ++;
  counter %= 1024;

  nunchuck_read_status(&x, &y, &button_1, &button_2);

  if (x < min_x)
    min_x = x;
  if (x > max_x)
    max_x = x;
  if (y < min_y)
    min_y = y;
  if (y > max_y)
    max_y = y;

  // Small tolerance around the rest position.
  if (abs(x_mouse-rest_x) < REST_POSITION_TOLERANCE)
    x_mouse = rest_x;
  if (abs(y_mouse-rest_y) < REST_POSITION_TOLERANCE)
    y_mouse = rest_y;


  // Let the values be in [-4, 4]:
  
  x_mouse = (x - rest_x);
  x_mouse = x_mouse * 4;
  if (x_mouse > 0) {
    x_mouse = x_mouse / (max_x - rest_x);
  } else {
    x_mouse = x_mouse / (rest_x - min_x);
  }

  y_mouse = (y - rest_y);
  y_mouse = y_mouse * 4;
  if (y_mouse > 0) {
    y_mouse = y_mouse / (max_y - rest_y);
  } else {
    y_mouse = y_mouse / (rest_y - min_y);
  }

  // x² transformation to have a kind of acceleration when the stick is fully inclinated.
  x_mouse = x_mouse * abs(x_mouse);
  y_mouse = y_mouse * abs(y_mouse);

  // Vertical screen coordinates are turned downward.
  y_mouse = -y_mouse;

  #ifdef NUNMOUSE_DEBUG
    if ((counter % 4) == 0) {
      Serial.printf("%3d Joystick=(%3d, %3d) Initial=(%3d, %3d) Mouse=(%2d, %2d) BT1=%d BT2=%d\n",
                    counter,
                    x, y,
                    rest_x, rest_y,
                    x_mouse, y_mouse,
                    button_1, button_2);
    }
  #else

    // Send a message only if the joystick has moved.
    if ((abs(x_mouse) >= 1) || (abs(y_mouse) >= 1)) {   
      Mouse.move(x_mouse, y_mouse);
    }
    // Send a message button if something changed.
    if ((button_1 != prev_button_1) || (button_2 != prev_button_2)) {
      Mouse.set_buttons(button_1, 0, button_2);
      prev_button_1 = button_1;
      prev_button_2 = button_2;
    }
  #endif
  
  delay(LOOP_DELAY);
}

