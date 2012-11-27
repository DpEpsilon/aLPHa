#include "Wire.h"
#include "ed1.h"
#include <LiquidCrystal.h>

// LED array
#define LATCH_PIN 11
#define DATA_PIN 12
#define CLOCK_PIN 10

// Other LED
#define LED_PIN 13

// Buttons
#define LEFT_BUTTON 14
#define RIGHT_BUTTON 15

// Buzzer
#define BUZZER_PIN 9

// Potentiometer
#define POT_PIN 3

// Sound constants
#define BEAT_SIZE 35
#define TUNING_NOTE 440
#define TEMPERAMENT 1.029302236643492 // pow(2.0,1.0/24.0)

// Game Constants
#define ENEMY_TIME 6000

// Game states

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

#define TITLE_SCREEN       0
#define MAIN_MENU          1
#define HIGH_SCORE         2
#define GAME               3
#define HIGH_SCORE_RESET   4
#define GAME_OVER          5
#define HIGH_SCORE_ENTRY   6
#define PLAYBACK           7
#define PLAYBACK_GAME_OVER 8
#define OPTIONS            9
#define PAUSE_MENU         10

// Sounds

#define THEME_SOUND   0
#define ENGINE_SOUND  1
#define BLIP_SOUND    2
#define NA_NA_SOUND   3
#define DAMAGE_SOUND  4
#define BEEP_SOUND    5
#define WOOSH_SOUND   6
#define TRILL_SOUND   7
#define FUNERAL_SOUND 8



// LCD display
LiquidCrystal lcd(6,7,8,2,3,4,5);

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
byte state = TITLE_SCREEN;
byte prevState = TITLE_SCREEN;
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
  ENEMY_TIME,
  ENEMY_TIME,
  ENEMY_TIME,
  ENEMY_TIME,
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
  // 4 Damage
  20,
  // 5 BEEP
  1,
  // 6 WOOSH
  17,
  // 7 Trill
  13,
  // 8 Funeral
  11,
};

// Durations
byte soundDurations[] =
{
  // 0
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
  
  // 1
  2,
  2,

  // 2
  1,
  1,
  1,

  // 3
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

  // 4
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

  // 5
  8,

  // 6
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

  // 7
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

  // 8
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
  // 0
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
  
  // 1
  -24,
  0,

  // 2
  24,
  31,
  38,

  // 3
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
  
  // 4  
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

  // 5
  6,

  // 6
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

  // 7
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

  // 8
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
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, a);
  digitalWrite(LATCH_PIN, HIGH);
}

// Changes the game state
void changeState(byte s)
{
  prevState = state;
  state = s;
  stateChanged = true;
}

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
    if (noteTime >= (int)(soundDurations[sumOfNotes + noteIndex] * BEAT_SIZE))
    {
      // decrement the note time
      noteTime -= (int)(soundDurations[sumOfNotes + noteIndex] * BEAT_SIZE);
      // increment the note;
      noteIndex++;

      // If the note is past the end of the sound
      if (noteIndex >= soundNoteCount[soundIndex])
      {
        // Stop the sound
        soundIndex = 255;
        noteIndex = 0;
        noteTime = 0;
        noTone(BUZZER_PIN);
      }
      else if (soundPitches[sumOfNotes + noteIndex] == -128) // If there is a rest
      {
        // Stop the buzzer
        noTone(BUZZER_PIN);
      }
      else // Otherwise
      {
        noTone(BUZZER_PIN);
        // Play the next note
        tone(BUZZER_PIN, TUNING_NOTE * pow(TEMPERAMENT,(float)soundPitches[sumOfNotes + noteIndex]), (int)(soundDurations[sumOfNotes + noteIndex] * BEAT_SIZE));
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
  if (!isMuted) {
    noTone(BUZZER_PIN);
    tone(BUZZER_PIN, TUNING_NOTE * pow(TEMPERAMENT,(float)soundPitches[sumOfNotes + noteIndex]), (int)(soundDurations[sumOfNotes + noteIndex] * BEAT_SIZE));
  }
}

// Stops the playing sound
void stopSound()
{
  soundIndex = 255;
  noTone(BUZZER_PIN);
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
  enemies[0] = ENEMY_TIME;
  enemies[1] = ENEMY_TIME;
  enemies[2] = ENEMY_TIME;
  enemies[3] = ENEMY_TIME;
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
    changeState(PLAYBACK_GAME_OVER);
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
    digitalWrite(LED_PIN, LOW);
    if (shot == 5)
    {
      // FIRE THE LAZER!
      // and miss
      startSound(TRILL_SOUND);
    }
    else
    {
      // DESTROY TEH ENEMY
      enemies[shot - 1] = ENEMY_TIME;
      enemiesHit++;
      // Play the BOOM
      startSound(WOOSH_SOUND);
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
        startSound(NA_NA_SOUND);
      }
    }
  }
}

void updateGame()
{
  int currentEnemyTime = constrain((ENEMY_TIME - (enemiesHit/50)*200),
                                   3000, ENEMY_TIME);
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
      enemies[i] = -(random((1000 * (i+1)) * 1/((currentTime - resetTime)/100000 + 1),
                            (2000* (i+1)) * 1/((currentTime - resetTime)/100000 + 1)));
      
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
      startSound(DAMAGE_SOUND);
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
      // Signal the charged lazer!
      digitalWrite(LED_PIN, HIGH);
      if (soundIndex != 3)
        startSound(BLIP_SOUND);
    }

    // If the button has just been pressed
    if (!leftButtonPrevious && leftButtonCurrent)
    {
      // Discharge the lazer
      lastLazer = currentTime;
      // FIRE THE LAZER!
      lazerPos = (playerPosPrev + 2)/5;

      // Signal lazer charge light to off
      digitalWrite(LED_PIN, LOW);

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
          enemies[i] = ENEMY_TIME;
          // Play the BOOM
          startSound(WOOSH_SOUND);
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
            startSound(NA_NA_SOUND);
          }
          break;
        }
      }
      
      if (!hit)
      {
        // Signal to Recorder
        buttonPressPre = 5;
        // YOU MISSED YOU N0000008!!!!!
        startSound(TRILL_SOUND);
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
    changeState(GAME_OVER);
  }
}

void drawGame()
{
  int currentEnemyTime = constrain((ENEMY_TIME - (enemiesHit/50)*200), 3000, ENEMY_TIME);
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
    boolean isBad = false;
    // Check for new enemies in the lazer square
    for (byte i = 0; i < 4; i++)
    {
      if (enemiesPos[i] == lazerPos && (enemies[i] < 0 || enemies[i] > currentEnemyTime))
      {
        isBad = true;
        break;
      }
    }
    // Draw the lazer beam if there is no new enemy in that square
    if (!isBad)
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
  case TITLE_SCREEN:
    // Display Title
    lcd.setCursor(5,0);
    lcd.print("aLPHa");
    lcd.setCursor(0,1);
    lcd.print("Press Any Button");
    break;
  case MAIN_MENU:
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
  case HIGH_SCORE:
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
  case GAME:
    drawGame();
    break;
  case HIGH_SCORE_RESET:
    // Draw Reset Highscore Prompt
    lcd.home();
    lcd.print("Reset HiScore?");
    lcd.setCursor(0, 1);
    lcd.print("Yes");
    lcd.setCursor(13, 1);
    lcd.print("No");
    break;
  case GAME_OVER:
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
  case HIGH_SCORE_ENTRY:
    // Display high score
    lcd.home();
    lcd.print(highScoreGuy);
    lcd.setCursor(3,0);
    lcd.print(' ');
    lcd.print(highScore);
    break;
  case PLAYBACK:
    drawGame();
    break;
  case PLAYBACK_GAME_OVER:
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
  case OPTIONS:
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
  case PAUSE_MENU:
    lcd.home();
    if (prevState == GAME)
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
  leftButtonCurrent = digitalRead(LEFT_BUTTON) == LOW;
  rightButtonCurrent = digitalRead(RIGHT_BUTTON) == LOW;
  potPrevious = potCurrent;
  potCurrent = analogRead(POT_PIN);
  // Update the accelerometer values if the game is in motion
  if (state == GAME)
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
    case TITLE_SCREEN:
      startSound(THEME_SOUND);
      resetGameVariables();
      break;
      
    case MAIN_MENU:
      startSound(BLIP_SOUND);
      currentIndex = 0;
      break;
      
    case HIGH_SCORE:
      startSound(BLIP_SOUND);
      break;
      
    case GAME:
      if (prevState != 10)
      {
        memoryPos = 0x200;
        filmIsSafe = false;
        EEPROM_ed1.sendB(0x199,0);
      }
      startSound(BLIP_SOUND);
      break;
      
    case HIGH_SCORE_RESET:
      startSound(BLIP_SOUND);
      break;
      
    case GAME_OVER:
      ledwrite(0);
      // If there is a new highscore
      if (enemiesHit > highScore)
      {
        startSound(NA_NA_SOUND);
      }
      else
      {
        startSound(FUNERAL_SOUND);
      }
      break;
      
    case HIGH_SCORE_ENTRY:
      currentIndex = 0;
      highScoreGuy[0] = ' ';
      highScoreGuy[1] = ' ';
      highScoreGuy[2] = ' ';
      break;
      
    case PLAYBACK:
      if (prevState != 10)
      {
        memoryPos = 0x200;
      }
      break;
      
    case PLAYBACK_GAME_OVER:
      startSound(BLIP_SOUND);
      ledwrite(0);
      break;
      
    case OPTIONS:
      startSound(BLIP_SOUND);
      break;
      
    case PAUSE_MENU:
      startSound(BLIP_SOUND);
      break;
    }
  }

  // Do normal logic
  switch (state)
  {
    case TITLE_SCREEN:
    // Poll for buttons
    if ((rightButtonCurrent && !rightButtonPrevious) || (leftButtonCurrent && !leftButtonPrevious))
    {
      changeState(MAIN_MENU);
    }
    break;
    
    case MAIN_MENU:
    // Toggle muted state
    if (potCurrent == 0 && potPrevious != 0)
    {
      isMuted = !isMuted;
      if (!isMuted)
        startSound(BLIP_SOUND);
    }
    
    if (leftButtonCurrent && !leftButtonPrevious)
    {
      changeState(GAME);
      pressTime = currentTime; // Make sure a double press is not registered.
    }
    else if (rightButtonCurrent && !rightButtonPrevious)
    {
      changeState(OPTIONS);
      pressTime = currentTime; // Make sure a double press is not registered.
    }
    break;
    
    case HIGH_SCORE:
    // Poll for buttons
    if (rightButtonCurrent && !rightButtonPrevious)
    {
      changeState(MAIN_MENU);
      pressTime = currentTime; // Make sure a double press is not registered.
    }
    else if (leftButtonCurrent && !leftButtonPrevious)
    {
      changeState(HIGH_SCORE_RESET);
      pressTime = currentTime; // Make sure a double press is not registered.
    }
    break;
    
  case GAME:
    updateGame();
    if (rightButtonCurrent && !rightButtonPrevious)
    {
      changeState(PAUSE_MENU);
    }
    break;
    
  case HIGH_SCORE_RESET:
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
      changeState(HIGH_SCORE);
      pressTime = currentTime; // Make sure a double press is not registered.
    }
    else if (rightButtonCurrent && !rightButtonPrevious)
    {
      // Go back
      changeState(HIGH_SCORE);
      pressTime = currentTime; // Make sure a double press is not registered.
    }
    break;
    
    // Game Over Screen
  case GAME_OVER:
    // Poll for buttons
    if ((!leftButtonPrevious || !rightButtonPrevious) && (leftButtonCurrent && rightButtonCurrent))
    {
      // If there is a new highscore
      if (enemiesHit > highScore)
      {
        // It's time to punch in those initals.
        highScore = enemiesHit;
        changeState(HIGH_SCORE_ENTRY);
      }
      else
      {
        // Otherwise, return to start
        changeState(TITLE_SCREEN);
      }
    }
    break;
    
  case HIGH_SCORE_ENTRY:
    highScoreGuy[currentIndex] = map(potCurrent, 1, 1023, 'Z', 'A');
    if (potCurrent == 0)
    {
      highScoreGuy[currentIndex] = ' ';
    }
    if (leftButtonCurrent && !leftButtonPrevious)
    {
      startSound(BLIP_SOUND);
      currentIndex++;
      pressTime = currentTime; // Make sure a double press is not registered.
    }
    else if (rightButtonCurrent && !rightButtonPrevious && currentIndex != 0)
    {
      startSound(BLIP_SOUND);
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
      changeState(TITLE_SCREEN);
    }
    break;

  case PLAYBACK:
    {
      int currentEnemyTime = constrain((ENEMY_TIME - (enemiesHit/50)*200), 3000, ENEMY_TIME);
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
          startSound(DAMAGE_SOUND);
          // Change the lastDamageTime
          lastDamageTime = currentTime;
        }
      }

      if (currentTime - lastLazer >= 500)
      {
        // If the lazer wasn't charged before
        if (previousTime - lastLazer < 500)
        {
          // Signal the charged lazer!
          digitalWrite(LED_PIN, HIGH);
          if (soundIndex != NA_NA_SOUND) // We like this sound to linger
            startSound(BLIP_SOUND);
        }
      }

      // Stop the game when the player is out of health
      if (health == 0 || health > 8)
      {
        changeState(PLAYBACK_GAME_OVER);
      }
    }
    
    if (rightButtonCurrent && !rightButtonPrevious)
    {
      changeState(PAUSE_MENU);
    }
    break;
  case PLAYBACK_GAME_OVER:
    if ((!leftButtonPrevious || !rightButtonPrevious) && (leftButtonCurrent && rightButtonCurrent))
    {
      // Return to start
      changeState(TITLE_SCREEN);
    }
    break;
  case OPTIONS:
    if (leftButtonCurrent && !leftButtonPrevious && filmIsSafe)
    {
      changeState(PLAYBACK);
    }
    else if (rightButtonCurrent && !rightButtonPrevious)
    {
      changeState(HIGH_SCORE);
      pressTime = currentTime; // Make sure a double press is not registered.
    }
    break;
  case PAUSE_MENU:
    if (rightButtonCurrent && !rightButtonPrevious)
    {
      changeState(prevState);
    }
    else if (leftButtonCurrent && !leftButtonPrevious)
    {
      changeState(TITLE_SCREEN);
    }
    break;
  }
  // Execute Sound Update Function
  if (!isMuted)
    playSounds();
}

void setup(void)
{
  // LCD
  lcd.begin(16,2);

  // Show loading screen
  lcd.print("   LOADING...");

  lcd.createChar(6, muted);

  // Serial
  Serial.begin(9600);

  // Leds
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  ledwrite(0);

  // Accelerometer
  if (Accel.begin(ACCEL_MEASURE_2G_MODE) != 0)
    exit(0);
  Accel.calibrate();

  // Random Function
  randomSeed(analogRead(0));

  // Buttons
  pinMode(LEFT_BUTTON, INPUT);
  pinMode(RIGHT_BUTTON, INPUT);
  digitalWrite(LEFT_BUTTON, HIGH);
  digitalWrite(RIGHT_BUTTON, HIGH);

  // EEPROM
  EEPROM_ed1.begin();

  highScoreGuy[0] = (char)EEPROM_ed1.readB(0x100);
  highScoreGuy[1] = (char)EEPROM_ed1.readB(0x101);
  highScoreGuy[2] = (char)EEPROM_ed1.readB(0x102);
  highScore = EEPROM_ed1.readI(0x103);

  filmIsSafe = EEPROM_ed1.readB(0x199) == 1;

  stopSound();

  // Mute on startup feature
  if (analogRead(POT_PIN) == 0)
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
