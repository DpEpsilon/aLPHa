#include "Wire.h"
#include "ed1.h"
#include <LiquidCrystal.h>

// LCD display
LiquidCrystal lcd(6,7,8,2,3,4,5);

// LED array
static byte latchPin = 11;
static byte dataPin = 12;
static byte clockPin = 10;

// Other LED
static byte ledPin = 13;

// Buttons
static byte leftButton = 14;
static byte rightButton = 15;

// Buzzer
static byte buzzerPin = 9;

// Potentiometer
static byte potPin = 3;

// Sound constants
static byte beatSize = 35;
static int tuningNote = 440;
static float temperament = pow(2.0,1.0/24.0);

// Game Constants
static int enemyTime = 6000;

// Timing Values
unsigned long currentTime = 0;
unsigned long previousTime = 0;
unsigned long resetTime = 0;
unsigned long lastDrawTime = -150;

// Input values
boolean rightButtonPrevious = false;
boolean leftButtonPrevious = false;
boolean rightButtonCurrent = false;
boolean leftButtonCurrent = false;
unsigned long pressTime = 0;
int potPrevious = 0;
int potCurrent = 0;
int accelYPrevious = 0;
int accelYCurrent = 0;

// FPS counting
int numberOfLoops = 0;

// Note variables
int noteTime = 0;
byte noteIndex = 0;
byte soundIndex = 255;
boolean isMuted = false;

// State
byte state = 0;
byte prevState = 0;
boolean stateChanged = true;
byte currentIndex = 0;

// Game Variables
unsigned short enemiesHit = 0;
byte playerPos = 37;
byte lazerPos = 37;
unsigned long lastLazer = 0;
byte playerPosPrev = 37;
int lazerCharged = 500;
byte health = 8;
byte preHealth = 8;
int enemies[4] = 
{
  enemyTime,
  enemyTime,
  enemyTime,
  enemyTime,
};
int enemiesPos[4];
byte enemyNum = 0;
unsigned long lastDamageTime = 0;
unsigned long lastMoveTime = 0;

// Saved Film Recording Variables
long memoryPos = 0x200;
boolean buttonPressPre = false;
boolean playerMoved = false;
byte newEnemies = 0;
unsigned int numDraws = 0;
boolean filmIsSafe = false;

// Saved Film Playback Variables
unsigned int lastTimeSlot = 0;
byte infoByte = 0;

// Highscore Variables
char highScoreGuy[3];
unsigned int highScore;

// Glyphs
byte enemyShip[4] =
{
  B01110,
  B11111,
  B10101,
  B01110,
};

byte playerShip[4] =
{
  B00100,
  B01010,
  B01110,
  B11011,
};

byte muted[8] = 
{
  B00000,
  B00110,
  B00101,
  B00101,
  B10110,
  B01100,
  B01100,
  B10010,
};

// Temporary Glyphs used for proccessing before
// Sending them to the LCD
byte editingGlyphs[2][8];

// Sounds
// Note Counts
byte soundNoteCount[] =
{
  // 0 Theme
  27,
  // 1 Engine
  2,
  // 2 Blip
  3,
  // 3 NA NA NA NA NA
  15,
  // 4 IMA DYING
  20,
  // 5 BEEP
  1,
  // 6 WOOSH
  17,
  // 7 TRILL
  13,
  // 8 FUNERAL
  11,
};

// Durations
byte soundDurations[] =
{
  // First Sound
  36,
  12,
  12,
  12,
  12,
  12,
  24,
  12,
  12,
  12,
  12,
  12,
  12,
  48,
  18,
  18,
  12,
  24,
  24,
  18,
  18,
  12,
  48,
  24,
  12,
  12,
  48,
  // Second Sound
  2,
  2,

  1,
  1,
  1,

  1,
  2,
  1,
  2,
  1,
  2,
  1,
  2,
  1,
  1,
  1,
  4,
  1,
  1,
  4,

  1,
  1,
  1,
  1,
  2,
  1,
  1,
  1,
  1,
  2,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  6,

  8,

  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  8,

  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  2,

  8,
  6,
  2,
  8,
  6,
  2,
  6,
  2,
  6,
  2,
  8,
};

// Pitches
// (24 note octave, A4 is 0)
// (eg. -2 is Ab4 and 2 is A#4)
signed char soundPitches[] =
{
  // First Sound
  -24,
  -10,
  -18,
  0,
  -4,
  -10,
  -8,
  0,
  -4,
  -10,
  -14,
  -18,
  -26,
  -24,
  -28,
  -18,
  -10,
  -14,
  -24,
  -18,
  -10,
  -4,
  0,
  -128,
  -18,
  -20,
  -24,
  // Second Sound
  -24,
  0,

  24,
  31,
  38,

  -8,
  6,
  -128,
  6,
  -128,
  0,
  -128,
  10,
  -128,
  6,
  16,
  6,
  0,
  16,
  0,

  -18,
  -11,
  -4,
  1,
  -128,
  -20,
  -14,
  -7,
  -1,
  -128,
  -24,
  -18,
  -11,
  -5,
  -9,
  -13,
  -17,
  -20,
  -22,
  -24,

  6,

  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,

  6,
  30,
  5,
  29,
  4,
  28,
  3,
  27,
  2,
  26,
  1,
  25,
  0,

  -24,
  -24,
  -24,
  -24,
  -18,
  -20,
  -20,
  -24,
  -24,
  -26,
  -24,
};

// Power Function with bytes
byte power(byte number, byte power)
{
  byte retval = 1;
  for (byte i = 0; i < power; i++)
  {
    retval = retval * number;
  }
  return retval;
}

// Updates Enemy and Player Glyphs
void updateGlyphs(byte index, byte shift)
{
  // Reset the Glyphs before use
  for (byte i = 0; i < 8; i++)
  {
    editingGlyphs[0][i] = 0;
    editingGlyphs[1][i] = 0;
  }


  if (index < 4)
  {
    // Enemies
    for (byte i = 0; i < 4; i++)
    {
      // Move the enemy down
      if (((int)i) - 4 + ((int)shift) < 0)
        continue;
      editingGlyphs[0][i - 4 + shift] = enemyShip[i];
    }
    lcd.createChar(index, editingGlyphs[0]);
  }
  else
  {
    // Player
    for (int i = 0; i < 4; i++)
    {
      // Shift the player left and right
      editingGlyphs[0][i] = playerShip[i] >> shift;
      editingGlyphs[1][i] = playerShip[i] << (5 - shift);
    }
    lcd.createChar(4, editingGlyphs[0]);
    lcd.createChar(5, editingGlyphs[1]);
  }
}

// Writes a byte to leds
void ledwrite(byte a)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, a);
  digitalWrite(latchPin, HIGH);
}

// Changes the game state
void changeState(byte s)
{
  prevState = state;
  state = s;
  stateChanged = true;
}
// 0 opening
// 1 main menu
// 2 high score
// 3 game
// 4 high score reset prompt
// 5 Game Over Screen
// 6 High Score Entry
// 7 Playback
// 8 Playback Game Over Screen
// 9 Options Menu
// 10 Pause Menu for Game and Playback

// Gets the index of the first note of a sound
int sumNotes(byte si)
{
  int retval = 0;
  for (byte i = 0; i < si; i++)
  {
    retval += soundNoteCount[i];
  }
  return retval;
}

// Sound mechanism
void playSounds()
{
  if (soundIndex != 255)
  {
    // Increment the timer
    noteTime += currentTime - previousTime;

    // Calculate where the sound is in the arrays
    int sumOfNotes = sumNotes(soundIndex);

    // If the note has been going for too long
    if (noteTime >= (int)(soundDurations[sumOfNotes + noteIndex] * beatSize))
    {
      // decrement the note time
      noteTime -= (int)(soundDurations[sumOfNotes + noteIndex] * beatSize);
      // increment the note;
      noteIndex++;

      // If the note is past the end of the sound
      if (noteIndex >= soundNoteCount[soundIndex])
      {
        // Stop the sound
        soundIndex = 255;
        noteIndex = 0;
        noteTime = 0;
        noTone(buzzerPin);
      }
      else if (soundPitches[sumOfNotes + noteIndex] == -128) // If there is a rest
      {
        // Stop the buzzer
        noTone(buzzerPin);
      }
      else // Otherwise
      {
        // Play the next note
        tone(buzzerPin, tuningNote * pow(temperament,(float)soundPitches[sumOfNotes + noteIndex]), (int)(soundDurations[sumOfNotes + noteIndex] * beatSize));
      }
    }
  }
}

// Begin a sound
void startSound(byte i)
{
  soundIndex = i;
  noteIndex = 0;
  noteTime = 0;

  // Calculate where the sound is in the arrays
  int sumOfNotes = sumNotes(soundIndex);
  if (!isMuted)
    tone(buzzerPin, tuningNote * pow(temperament,(float)soundPitches[sumOfNotes + noteIndex]), (int)(soundDurations[sumOfNotes + noteIndex] * beatSize));
}

// Stops the playing sound
void stopSound()
{
  soundIndex = 255;
  noTone(buzzerPin);
}

void resetGameVariables()
{
  enemiesHit = 0;
  playerPos = 37;
  lazerPos = 37;
  lastLazer = 0;
  playerPosPrev = 37;
  lazerCharged = 500;
  health = 8;
  preHealth = 8;
  enemies[0] = enemyTime;
  enemies[1] = enemyTime;
  enemies[2] = enemyTime;
  enemies[3] = enemyTime;
  enemiesPos[0] = 0;
  enemiesPos[1] = 0;
  enemiesPos[2] = 0;
  enemiesPos[3] = 0;
  enemyNum = 0;
  lastDamageTime = 0;
  lastMoveTime = 0;
}

void updateGameRecorder()
{ 
  // Check if EEPROM is available
  if (memoryPos >= 0x3ff8)
    return;
  // Data to Write
  // Writes every draw
  // Press of fire button
  // The player has moved
  // There is a new enemy (or are new enemies) // Need to fix this
  EEPROM_ed1.sendB(memoryPos++, ((byte)playerMoved) + (newEnemies << 1) + (buttonPressPre << 5));

  if (playerMoved)
  {
    EEPROM_ed1.sendB(memoryPos++, playerPos);
  }
  if (newEnemies != 0)
  {
    for (byte i = 0; i < 4; i++)
    {
      if ((newEnemies & power(2,i)) != 0)
      {
        EEPROM_ed1.sendB(memoryPos++, enemiesPos[i]);
      }
    }
    newEnemies = 0;
  }
  if (buttonPressPre != 0)
  {
    EEPROM_ed1.sendB(memoryPos++, lazerPos);
    buttonPressPre = 0;
  }
}

void playBack()
{
  // Check if EEPROM is available
  if (memoryPos >= 0x3ff8)
  {
    memoryPos = 0x4000;
    changeState(8);
    return;
  }
  // Data to Read
  // Reads every draw
  // Press of fire button
  // The player has moved
  // There is a new enemy (or are new enemies)

  // InfoByte
  byte stuff = EEPROM_ed1.readB(memoryPos++);
  boolean moved = (stuff & 1) != 0;
  byte shot = stuff >> 5;

  if (moved)
  {
    playerPos = EEPROM_ed1.readB(memoryPos++);
  }
  for (byte i = 0; i < 4; i++)
  {
    if ((stuff & power(2,i+1)) != 0)
    {
      enemiesPos[i] = EEPROM_ed1.readB(memoryPos++);
      enemies[i] = 0;
    }
  }

  if (shot != 0)
  {
    lazerPos = EEPROM_ed1.readB(memoryPos++);
    // Discharge the lazer
    lastLazer = currentTime;
    // Signal lazer charge light to off
    digitalWrite(ledPin, LOW);
    if (shot == 5)
    {
      // FIRE THE LAAAAAAAZZZZZZZZZZZZZZZZZZZEEEEEEEEEEEEEEEEERRRRRRRRRRRRR!!!!!!!!!!!!
      // and miss
      startSound(7);
      // noob sound has been initialized
    }
    else
    {
      // DESTROY TEH ENEMY
      enemies[shot - 1] = enemyTime;
      enemiesHit++;
      // Play the BOOM
      startSound(6);
      if (enemiesHit % 50 == 0)
      {
        if (health < 7)
        {
          health += 2;
          preHealth = health;
        }
        else if (health < 8)
        {
          health += 1;
          preHealth = health;
        }
        startSound(3);
      }
    }
  }
}

void drawGame()
{
  int currentEnemyTime = constrain((enemyTime - (enemiesHit/50)*200), 3000, enemyTime);
  // If the player has not recently been hit
  if (currentTime - lastDamageTime > 2000)
  {
    // draw the health
    ledwrite(power(2, health) - 1);
  }
  else // If the player has recently been hit
  {
    // Flash the health
    if ((currentTime - lastDamageTime) % 1000 < 500)
    {
      ledwrite(power(2, preHealth) - 1);
    }
    else
    {
      ledwrite(power(2, health) - 1);
    }
  }
  // Draw the player
  lcd.setCursor(playerPosPrev/5,1);
  lcd.write(4);
  lcd.write(5);
  // Update the player glyph
  updateGlyphs(4, playerPos%5);
  // Enemy loop
  for (byte i = 0; i < 4; i++)
  {
    // Don't draw enemy if it isn't active
    if (enemies[i] < 0 || enemies[i] >= currentEnemyTime)
      continue;
    // Draw the enemy
    lcd.setCursor(enemiesPos[i],0);
    lcd.write(i);
    // If the enemy is firing
    if (enemies[i] > (currentEnemyTime - (currentEnemyTime/8)))
    {
      // Use the last glyph
      updateGlyphs(i, 8);
      // Draw a firing enemy
      lcd.setCursor(enemiesPos[i], 1);
      lcd.print("|");
    }
    else // If the enemy isn't firing
    {
      // Use an enemy moving glyph
      updateGlyphs(i, enemies[i]/(currentEnemyTime/8) + 1);
    }
  }

  // If the player is firing
  if (currentTime - lastLazer < 500)
  {
    boolean IsBad = false;
    // Check for new enemies in the lazer square
    for (byte i = 0; i < 4; i++)
    {
      if (enemiesPos[i] == lazerPos && (enemies[i] < 0 || enemies[i] > currentEnemyTime))
      {
        IsBad = true;
        break;
      }
    }
    // Draw the lazer beam if there is no new enemy in that square
    if (!IsBad)
    {
      lcd.setCursor(lazerPos,0);
      lcd.print("|");
    }
  }
}

// Draws the screen
void draw()
{
  // Clear
  lcd.clear();

  // Do something dependent on the state
  switch(state)
  {
    // Title screen
  case 0:
    // Display Title
    lcd.setCursor(5,0);
    lcd.print("aLPHa");
    lcd.setCursor(0,1);
    lcd.print("Press Any Button");
    break;
    // Menu Screen
  case 1:
    // Display Menu
    lcd.setCursor(5,0);
    lcd.print("aLPHa");
    lcd.setCursor(0,1);
    lcd.print("Play");
    lcd.setCursor(9,1);
    lcd.print("Options");
    if (isMuted)
    {
      lcd.setCursor(15,0);
      lcd.write(6);
    }
    break;
    // High score screen
  case 2:
    // Display high score
    lcd.home();
    lcd.print(highScoreGuy);
    lcd.setCursor(3,0);
    lcd.print(' ');
    lcd.print(highScore);
    lcd.setCursor(0,1);
    lcd.print("Reset");
    lcd.setCursor(12,1);
    lcd.print("Back");
    break;
    // Game screen
  case 3:
    drawGame();
    break;
    // Reset Highscore Prompt
  case 4:
    // Draw Reset Highscore Prompt
    lcd.home();
    lcd.print("Reset HiScore?");
    lcd.setCursor(0, 1);
    lcd.print("Yes");
    lcd.setCursor(13, 1);
    lcd.print("No");
    break;
    // Game Over screen
  case 5:
    lcd.setCursor(3,0);
    lcd.print("GAME OVER");
    lcd.setCursor(0,1);
    // If there is a new highscore
    if (enemiesHit > highScore || enemiesHit == 65535)
    {
      // Alternate between displaying
      // score and messsage
      if ((currentTime - resetTime) % 2000 < 1000)
      {
        if (enemiesHit != 65535)
        {
          lcd.setCursor(2,1);
          lcd.print("NEW HiScore!");
        }
        else
        {
          lcd.setCursor(1,1);
          lcd.print("MAXIMUM SCORE!");
        }
      }
      else
      {
        lcd.print(enemiesHit);
      }
    }
    else
    {
      lcd.print(enemiesHit);
    }
    break;
  case 6:
    // Display high score
    lcd.home();
    lcd.print(highScoreGuy);
    lcd.setCursor(3,0);
    lcd.print(' ');
    lcd.print(highScore);
    break;
  case 7:
    drawGame();
    break;
  case 8:
    if (memoryPos < 0x4000)
    {
      lcd.setCursor(3,0);
      lcd.print("GAME OVER");
    }
    else
    {
      lcd.setCursor(1,0);
      lcd.print("End of EEPROM");
    }
    lcd.setCursor(0,1);
    lcd.print(enemiesHit);
    break;
  case 9:
    lcd.setCursor(5,0);
    lcd.print("aLPHa");
    if (filmIsSafe)
    {
      lcd.setCursor(0,1);
      lcd.print("PlayBack");
    }
    lcd.setCursor(9,1);
    lcd.print("HiScore");
    break;
  case 10:
    lcd.home();
    if (prevState == 3)
    {
      if (currentTime % 2000 < 1000)
      {
        lcd.print("SCORE: ");
        lcd.print(enemiesHit);
      }
      else
      {
        lcd.print("HISCORE: ");
        lcd.print(highScore);
      }
    }
    else
    {
      lcd.print("SCORE: ");
      lcd.print(enemiesHit);
    }
    lcd.setCursor(0,1);
    lcd.print("Exit");
    lcd.setCursor(10,1);
    lcd.print("Resume");
    break;
  }
}

void update()
{
  // Update Timing Values;
  previousTime = currentTime;
  currentTime = millis();

  // Update Input Values
  leftButtonPrevious = leftButtonCurrent;
  rightButtonPrevious = rightButtonCurrent;
  leftButtonCurrent = digitalRead(leftButton) == LOW;
  rightButtonCurrent = digitalRead(rightButton) == LOW;
  potPrevious = potCurrent;
  potCurrent = analogRead(potPin);
  // Update the accelerometer values if the game is in motion
  if (state == 3)
  {
    accelYPrevious = accelYCurrent;
    accelYCurrent = Accel.readY();
  }

  // Do something if the state has changed
  if (stateChanged)
  {
    // Reset state changed values
    resetTime = currentTime;
    stateChanged = false;
    switch (state)
    {
    case 0:
      startSound(0);
      resetGameVariables();
      break;
      
    case 1:
      startSound(2);
      currentIndex = 0;
      break;
      
    case 2:
      startSound(2);
      break;
      
    case 3:
      if (prevState != 10)
      {
        memoryPos = 0x200;
        filmIsSafe = false;
        EEPROM_ed1.sendB(0x199,0);
      }
      startSound(2);
      break;
      
    case 4:
      startSound(2);
      break;
      
    case 5:
      ledwrite(0);
      // If there is a new highscore
      if (enemiesHit > highScore)
      {
        // NANANANANAAAANAAA
        // I beat ur score!!
        startSound(3);
      }
      else
      {
        // IMA DEAD!!!
        startSound(8);
      }
      break;
      
    case 6:
      currentIndex = 0;
      highScoreGuy[0] = ' ';
      highScoreGuy[1] = ' ';
      highScoreGuy[2] = ' ';
      break;
      
    case 7:
      if (prevState != 10)
      {
        memoryPos = 0x200;
      }
      break;
      
    case 8:
      startSound(2);
      ledwrite(0);
      break;
      
    case 9:
      startSound(2);
      break;
      
    case 10:
      startSound(2);
      break;
    }
  }

  // Do normal logic
  switch (state)
  {
    // Title Screen
  case 0:
    // Poll for buttons
    if ((rightButtonCurrent && !rightButtonPrevious) || (leftButtonCurrent && !leftButtonPrevious))
    {
      changeState(1);
    }
    break;
    
    // Menu Screen
  case 1:
    if (potCurrent == 0 && potPrevious != 0)
    {
      isMuted = !isMuted;
      if (!isMuted)
        startSound(2);
    }
    if (leftButtonCurrent && !leftButtonPrevious)
    {
      changeState(3);
      pressTime = currentTime; // Make sure a double press is not registered.
    }
    else if (rightButtonCurrent && !rightButtonPrevious)
    {
      changeState(9);
      pressTime = currentTime; // Make sure a double press is not registered.
    }
    break;
    
    // Highscore screen
  case 2:
    // Poll for buttons
    if (rightButtonCurrent && !rightButtonPrevious)
    {
      changeState(1);
      pressTime = currentTime; // Make sure a double press is not registered.
    }
    else if (leftButtonCurrent && !leftButtonPrevious)
    {
      changeState(4);
      pressTime = currentTime; // Make sure a double press is not registered.
    }
    break;
    
    // Game Screen
  case 3:
    {
      int currentEnemyTime = constrain((enemyTime - (enemiesHit/50)*200), 3000, enemyTime);
      playerPosPrev = playerPos;
      // Enemies loop
      for (byte i = 0; i < 4; i++)
      {
        // Update Active enemies
        if (enemies[i] < currentEnemyTime)
          enemies[i] += currentTime - previousTime;
        else
        {
          // Start timed-out enemies' timers'
          enemies[i] = -(random((1000 * (i+1)) * 1/((currentTime - resetTime)/100000 + 1), (2000* (i+1)) * 1/((currentTime - resetTime)/100000 + 1)));
          
          // Check for a new spot for the enemy
          while (true)
          {
            boolean IsBad = false;
            enemiesPos[i] = random(0, 15);
            for (int j = 0; j < 4; j++)
            {
              if (j == i)
                continue;
              if (enemiesPos[i] == enemiesPos[j] && enemies[j] < currentEnemyTime)
              {
                IsBad = true;
                break;
              }
            }
            if (!IsBad)
              break;
          }
        }
        
        // Record the new enemies
        if ((enemies[i] >= 0) && ((enemies[i] - ((int)(currentTime - previousTime))) < 0))
        {
          newEnemies = newEnemies | power(2,i);
        }
        
        // If enemy should shoot and hasn't started shooting
        if (enemies[i] >= (currentEnemyTime - (currentEnemyTime/8)) && enemies[i] - (currentTime - previousTime) < (currentEnemyTime - (currentEnemyTime/8)))
        {
          // Set previous health if the player
          // Hasn't taken any damage lately
          if (currentTime - lastDamageTime > 2000)
          {
            preHealth = health;
          }
          // Take away health;
          health -= 1;
          // Play an evil noise
          startSound(4);
          // Change the lastDamageTime
          lastDamageTime = currentTime;
        }
      }

      // If the player hasn't moved lately
      if (currentTime - lastMoveTime > 25)
      {
        // reset the lastMoveTime
        lastMoveTime = currentTime;

        if (accelYCurrent > 5 && playerPos < 75)
        {
          playerPos += 1;
          // Record the move player for the game recorder
          playerMoved = true;
        }
        else if (accelYCurrent < -5 && playerPos > 0)
        {
          playerPos -= 1;
          // Record the move player for the game recorder
          playerMoved = true;
        }
      }
      
      // If the player has a charged lazer
      if (currentTime - lastLazer >= 500)
      {
        // If the lazer wasn't charged before
        if (previousTime - lastLazer < 500)
        {
          // Signal the charged LAAAAAAAAAZZZZZZZZZZZEEEEEEEEEEEEEERRRRRRR!!!!!!!!!
          digitalWrite(ledPin, HIGH);
          if (soundIndex != 3)
            startSound(2);
        }

        // If the button has just been pressed
        if (!leftButtonPrevious && leftButtonCurrent)
        {
          // Discharge the lazer
          lastLazer = currentTime;
          // FIRE THE LAAAAAAAZZZZZZZZZZZZZZZZZZZEEEEEEEEEEEEEEEEERRRRRRRRRRRRR!!!!!!!!!!!!
          lazerPos = (playerPosPrev + 2)/5;

          // Signal lazer charge light to off
          digitalWrite(ledPin, LOW);

          boolean hit = false;

          // Check if an enemy has been hit
          for (byte i = 0; i < 4; i++)
          {
            // YES???
            if (enemiesPos[i] == lazerPos && enemies[i] < currentEnemyTime && enemies[i] >= 0)
            {
              // YES!!!

              // Signal to Recorder
              buttonPressPre = i + 1;

              // DESTROY TEH ENEMY
              enemies[i] = enemyTime;
              // Play the BOOM
              startSound(6);
              hit = true;
              enemiesHit++;
              if (enemiesHit % 50 == 0)
              {
                if (health < 7)
                {
                  health += 2;
                  preHealth = health;
                }
                else if (health < 8)
                {
                  health += 1;
                  preHealth = health;
                }
                startSound(3);
              }
              break;
            }
          }
          
          if (!hit)
          {
            // Signal to Recorder
            buttonPressPre = 5;
            // YOU MISSED YOU N0000008!!!!!
            startSound(7);
            // noob sound has been initialized
          }
        }
      }
      
      // Stop the game when the player is out of health
      if (health == 0 || health > 8)
      {
        // Mark the film as valid
        filmIsSafe = true;
        EEPROM_ed1.sendB(0x199,1);
        changeState(5);
      }
    }

    if (rightButtonCurrent && !rightButtonPrevious)
    {
      changeState(10);
    }
    break;
    
    // Reset Highscore Screen
  case 4:
    // Poll for buttons
    if (leftButtonCurrent && !leftButtonPrevious)
    {
      // Reset Highscore
      highScoreGuy[0] = 'N';
      EEPROM_ed1.sendB(0x100, (byte)'N');
      highScoreGuy[1] = '/';
      EEPROM_ed1.sendB(0x101, (byte)'/');
      highScoreGuy[2] = 'A';
      EEPROM_ed1.sendB(0x102, (byte)'A');
      highScore = 0;
      EEPROM_ed1.sendI(0x103, 0);

      // Go back
      changeState(2);
      pressTime = currentTime; // Make sure a double press is not registered.
    }
    else if (rightButtonCurrent && !rightButtonPrevious)
    {
      // Go back
      changeState(2);
      pressTime = currentTime; // Make sure a double press is not registered.
    }
    break;
    
    // Game Over Screen
  case 5:
    // Poll for buttons
    if ((!leftButtonPrevious || !rightButtonPrevious) && (leftButtonCurrent && rightButtonCurrent))
    {
      // If there is a new highscore
      if (enemiesHit > highScore)
      {
        // It's time to punch in those initals.
        highScore = enemiesHit;
        changeState(6);
      }
      else
      {
        // Otherwise, return to start
        changeState(0);
      }
    }
    break;
    
  case 6:
    highScoreGuy[currentIndex] = map(potCurrent, 1, 1023, 'Z', 'A');
    if (potCurrent == 0)
    {
      highScoreGuy[currentIndex] = ' ';
    }
    if (leftButtonCurrent && !leftButtonPrevious)
    {
      startSound(2);
      currentIndex++;
      pressTime = currentTime; // Make sure a double press is not registered.
    }
    else if (rightButtonCurrent && !rightButtonPrevious && currentIndex != 0)
    {
      startSound(2);
      highScoreGuy[currentIndex] = ' ';
      currentIndex--;
    }
    
    if (currentIndex == 3)
    {
      currentIndex = 0;

      EEPROM_ed1.sendB(0x100, (char)highScoreGuy[0]);
      EEPROM_ed1.sendB(0x101, (char)highScoreGuy[1]);
      EEPROM_ed1.sendB(0x102, (char)highScoreGuy[2]);
      EEPROM_ed1.sendI(0x103, highScore);
      changeState(0);
    }
    break;

  case 7:
    {
      int currentEnemyTime = constrain((enemyTime - (enemiesHit/50)*200), 3000, enemyTime);
      playerPosPrev = playerPos;
      // Enemies loop
      for (byte i = 0; i < 4; i++)
      {
        // Update Active enemies
        if (enemies[i] < currentEnemyTime)
        {
          enemies[i] += currentTime - previousTime;
        }

        // If enemy should shoot and hasn't started shooting
        if (enemies[i] >= (currentEnemyTime - (currentEnemyTime/8)) && enemies[i] - (currentTime - previousTime) < (currentEnemyTime - (currentEnemyTime/8)))
        {
          // Set previous health if the player
          // Hasn't taken any damage lately
          if (currentTime - lastDamageTime > 2000)
          {
            preHealth = health;
          }
          // Take away health;
          health -= 1;
          // Play an evil noise
          startSound(4);
          // Change the lastDamageTime
          lastDamageTime = currentTime;
        }
      }

      if (currentTime - lastLazer >= 500)
      {
        // If the lazer wasn't charged before
        if (previousTime - lastLazer < 500)
        {
          // Signal the charged LAAAAAAAAAZZZZZZZZZZZEEEEEEEEEEEEEERRRRRRR!!!!!!!!!
          digitalWrite(ledPin, HIGH);
          if (soundIndex != 3)
            startSound(2);
        }
      }

      // Stop the game when the player is out of health
      if (health == 0 || health > 8)
      {
        changeState(8);
      }
    }
    if (rightButtonCurrent && !rightButtonPrevious)
    {
      changeState(10);
    }
    break;
  case 8:
    if ((!leftButtonPrevious || !rightButtonPrevious) && (leftButtonCurrent && rightButtonCurrent))
    {
      // Return to start
      changeState(0);
    }
    break;
  case 9:
    if (leftButtonCurrent && !leftButtonPrevious && filmIsSafe)
    {
      changeState(7);
    }
    else if (rightButtonCurrent && !rightButtonPrevious)
    {
      changeState(2);
      pressTime = currentTime; // Make sure a double press is not registered.
    }
    break;
  case 10:
    if (rightButtonCurrent && !rightButtonPrevious)
    {
      changeState(prevState);
    }
    else if (leftButtonCurrent && !leftButtonPrevious)
    {
      changeState(0);
    }
    break;
  }
  // Execute Sound Update Function
  if (!isMuted)
    playSounds();
}

void setup(void)
{
  // Initialize

  // LCD
  lcd.begin(16,2);

  // Show loading screen
  lcd.print("   LOADING...");

  lcd.createChar(6, muted);

  // Serial
  Serial.begin(9600);

  // Leds
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  ledwrite(0);

  // Accelerometer
  if (Accel.begin(ACCEL_MEASURE_2G_MODE) != 0)
    exit(0);
  Accel.calibrate();

  // Random Function
  randomSeed(analogRead(0));

  // Buttons
  pinMode(leftButton, INPUT);
  pinMode(rightButton, INPUT);
  digitalWrite(leftButton, HIGH);
  digitalWrite(rightButton, HIGH);

  // EEPROM
  EEPROM_ed1.begin();

  highScoreGuy[0] = (char)EEPROM_ed1.readB(0x100);
  highScoreGuy[1] = (char)EEPROM_ed1.readB(0x101);
  highScoreGuy[2] = (char)EEPROM_ed1.readB(0x102);
  highScore = EEPROM_ed1.readI(0x103);

  filmIsSafe = EEPROM_ed1.readB(0x199) == 1;

  stopSound();

  if (analogRead(potPin) == 0)
    isMuted = true;
}

void loop(void)
{
  // Update
  update();
  // Draw if game has waited for the
  // Player's brain to take the visual
  // Data in to account
  // (Otherwise, screen would be practically invisible)
  if (currentTime >= lastDrawTime + 150)
  {
    lastDrawTime = currentTime;
    draw();

    if (state == 3)
    {
      // The game recorder needs to be updated every draw.
      updateGameRecorder();
    }
    else if (state == 7)
    {
      // The game player needs to be updated every draw.
      playBack();
    }
  }
  // FPS counting
  numberOfLoops++;

  if (currentTime % 1000 < previousTime % 1000)
  {
    Serial.print("FPS: ");
    Serial.print(numberOfLoops);
    Serial.println();
    numberOfLoops = 0;
  }
}
