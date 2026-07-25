#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

// Define SoftwareSerial pins for DFPlayer Mini
SoftwareSerial mySoftwareSerial(6, 8); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// Initialize I2C LCD (address 0x27 is common, adjust if needed)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define RFID pins
#define RST_PIN 9
#define SS_PIN 10
MFRC522 rfid(SS_PIN, RST_PIN);

// Button pins for manual control
#define NEXT_BTN 2
#define PREV_BTN 3
#define VOLUME_UP_BTN 4
#define VOLUME_DOWN_BTN 5

// LED pins for status indication
#define VALID_LED 7     // Green LED
#define INVALID_LED 12  // Red LED
#define STATUS_LED 13   // Blue LED for system status

// Timing constants
const unsigned long rfidMessageDuration = 3000;
const unsigned long mp3PlayDuration = 2000;
const unsigned long scrollDelay = 300;
const unsigned long idleTimeout = 10000; // 10 seconds idle timeout for scan screen
const unsigned long gameIdleTimeout = 5000; // 5 seconds idle timeout for game

// System variables
int currentVolume = 30;
int totalTracks = 10; // Adjust based on your MP3 files
int currentTrack = 1;
bool isPlaying = false;
unsigned long lastActivity = 0;
unsigned long lastScroll = 0;
int scrollPosition = 0;
bool inIdleMode = false;

// Dinosaur game variables
int dinoPos = 1;
int cactusPos = 15;
bool dinoFrame = false;
int gameSpeed = 500;
int score = 0;
bool gameActive = false;
bool dinoJumping = false;
int jumpHeight = 0;
unsigned long jumpStartTime = 0;
const int jumpDuration = 800; // 800ms jump duration

// Enhanced user database with names and track assignments
struct User {
  byte uid[4];
  String name;
  int trackNumber;
  bool isAdmin;
};

// User database - add more users as needed
User users[] = {
  {{0xB3, 0xE3, 0x24, 0x30}, "Angelico", 1, true},
  {{0xAA, 0xBB, 0xCC, 0xDD}, "Jane Smith", 3, false},
  {{0x11, 0x22, 0x33, 0x44}, "Admin User", 5, true}
};
const int numUsers = sizeof(users) / sizeof(users[0]);

// Custom LCD characters for animations
byte musicNote[8] = {0x02, 0x03, 0x02, 0x02, 0x0E, 0x1E, 0x0C, 0x00};
byte speaker[8] = {0x01, 0x03, 0x0F, 0x0F, 0x0F, 0x03, 0x01, 0x00};
byte heart[8] = {0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00};
// Dinosaur game characters
byte dino1[8] = {0x07, 0x05, 0x17, 0x1F, 0x1F, 0x1E, 0x0A, 0x0A}; // Running dino frame 1
byte dino2[8] = {0x07, 0x05, 0x17, 0x1F, 0x1F, 0x1E, 0x14, 0x05}; // Running dino frame 2
byte cactus[8] = {0x04, 0x15, 0x0E, 0x0E, 0x04, 0x04, 0x04, 0x04}; // Cactus obstacle
byte ground[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F}; // Ground line

void setup() {
  Serial.begin(9600);
  
  // Initialize pin modes
  pinMode(NEXT_BTN, INPUT_PULLUP);
  pinMode(PREV_BTN, INPUT_PULLUP);
  pinMode(VOLUME_UP_BTN, INPUT_PULLUP);
  pinMode(VOLUME_DOWN_BTN, INPUT_PULLUP);
  pinMode(VALID_LED, OUTPUT);
  pinMode(INVALID_LED, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  
  // LED startup sequence
  startupLEDSequence();
  
  // Initialize SPI and RFID
  SPI.begin();
  rfid.PCD_Init();
  
  // Initialize SoftwareSerial for DFPlayer
  mySoftwareSerial.begin(9600);
  
  // Initialize LCD with custom characters
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.createChar(0, musicNote);
  lcd.createChar(1, speaker);
  lcd.createChar(2, heart);
  lcd.createChar(3, dino1);
  lcd.createChar(4, dino2);
  lcd.createChar(5, cactus);
  lcd.createChar(6, ground);
  
  // Boot animation
  bootAnimation();
  
  // Initialize DFPlayer with enhanced error handling
  if (!initializeDFPlayer()) {
    showError("DFPlayer Failed!");
    return;
  }
  
  // Set initial volume
  myDFPlayer.volume(currentVolume);
  
  // Show initial scan message
  showScanMessage();
  
  lastActivity = millis();
  Serial.println(F("Enhanced MP3 Player System Ready!"));
}

void loop() {
  unsigned long currentTime = millis();
  
  // Handle button inputs
  handleButtons();
  
  // Handle RFID scanning
  handleRFID(currentTime);
  
  // Handle idle mode and screensaver
  handleIdleMode(currentTime);
  
  // Update LED status
  updateStatusLED();
  
  delay(50);
}

bool initializeDFPlayer() {
  int attempts = 0;
  const int maxAttempts = 3;
  
  while (attempts < maxAttempts) {
    if (myDFPlayer.begin(mySoftwareSerial)) {
      Serial.println(F("DFPlayer Mini online."));
      return true;
    }
    
    attempts++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Init attempt ");
    lcd.print(attempts);
    lcd.setCursor(0, 1);
    lcd.print("Please wait...");
    delay(1000);
  }
  
  return false;
}

void handleButtons() {
  static unsigned long lastButtonPress = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastButtonPress < 200) return; // Debounce
  
  // If in game over state, any button restarts the game
  if (inIdleMode && !gameActive) {
    gameActive = true;
    initDinoGame();
    lastButtonPress = currentTime;
    lastActivity = currentTime; // Update last activity
    return;
  }
  
  if (digitalRead(NEXT_BTN) == LOW) {
    if (inIdleMode && gameActive) {
      jumpDino(); // Use next button as jump in game
    } else {
      nextTrack();
    }
    lastButtonPress = currentTime;
    lastActivity = currentTime;
  }
  
  if (digitalRead(PREV_BTN) == LOW) {
    if (inIdleMode && gameActive) {
      jumpDino(); // Use prev button as jump in game
    } else {
      prevTrack();
    }
    lastButtonPress = currentTime;
    lastActivity = currentTime;
  }
  
  if (digitalRead(VOLUME_UP_BTN) == LOW) {
    volumeUp();
    lastButtonPress = currentTime;
    lastActivity = currentTime;
  }
  
  if (digitalRead(VOLUME_DOWN_BTN) == LOW) {
    volumeDown();
    lastButtonPress = currentTime;
    lastActivity = currentTime;
  }
}

void handleRFID(unsigned long currentTime) {
  static unsigned long rfidMessageTime = 0;
  static bool showingRfidMessage = false;
  
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    
    // If in dino game mode, use card scan as jump control
    if (inIdleMode && gameActive) {
      jumpDino();
      rfid.PICC_HaltA();
      lastActivity = currentTime; // Update last activity
      return;
    }
    
    lastActivity = currentTime;
    inIdleMode = false;
    
    User* foundUser = findUser(rfid.uid.uidByte);
    
    if (foundUser != nullptr) {
      handleValidUser(foundUser);
    } else {
      handleInvalidUser();
    }
    
    // Print UID for debugging
    printUID(rfid.uid.uidByte, rfid.uid.size);
    rfid.PICC_HaltA();
    
    showingRfidMessage = true;
    rfidMessageTime = currentTime;
  }
  
  // Clear RFID message after duration
  if (showingRfidMessage && (currentTime - rfidMessageTime >= rfidMessageDuration)) {
    showingRfidMessage = false;
    showMainScreen();
  }
}

User* findUser(byte* scannedUID) {
  for (int i = 0; i < numUsers; i++) {
    if (compareUID(scannedUID, users[i].uid)) {
      return &users[i];
    }
  }
  return nullptr;
}

bool compareUID(byte* uid1, byte* uid2) {
  for (int i = 0; i < 4; i++) {
    if (uid1[i] != uid2[i]) return false;
  }
  return true;
}

void handleValidUser(User* user) {
  digitalWrite(VALID_LED, HIGH);
  digitalWrite(INVALID_LED, LOW);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(2); // Heart symbol
  lcd.print(" Welcome!");
  lcd.setCursor(0, 1);
  lcd.print(user->name);
  
  // Play user's assigned track at max volume for 2 seconds
  currentTrack = user->trackNumber;
  myDFPlayer.volume(30); // Max volume
  myDFPlayer.play(currentTrack);
  isPlaying = true;
  
  Serial.print(F("Welcome "));
  Serial.print(user->name);
  Serial.print(F(" - Playing track "));
  Serial.print(currentTrack);
  Serial.println(F(" at MAX volume for 2 seconds"));
  
  // Play for exactly 2 seconds
  delay(2000);
  myDFPlayer.stop();
  isPlaying = false;
  myDFPlayer.volume(currentVolume); // Reset to normal volume
  
  digitalWrite(VALID_LED, LOW);
}

void handleInvalidUser() {
  digitalWrite(INVALID_LED, HIGH);
  digitalWrite(VALID_LED, LOW);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Denied!");
  lcd.setCursor(0, 1);
  lcd.print("Unknown Card");
  
  // Play error sound at max volume for 2 seconds
  myDFPlayer.volume(30); // Max volume
  myDFPlayer.play(2); // Error sound
  Serial.println(F("Access Denied - Playing error sound at MAX volume for 2 seconds"));
  
  // Play for exactly 2 seconds
  delay(2000);
  myDFPlayer.stop();
  myDFPlayer.volume(currentVolume); // Reset to normal volume
  
  // Blink invalid LED
  for (int i = 0; i < 3; i++) {
    delay(200);
    digitalWrite(INVALID_LED, LOW);
    delay(200);
    digitalWrite(INVALID_LED, HIGH);
  }
  digitalWrite(INVALID_LED, LOW);
}

void handleIdleMode(unsigned long currentTime) {
  // Handle transitions between scan screen and game
  if (!inIdleMode && (currentTime - lastActivity > idleTimeout)) {
    // Idle on scan screen for 10 seconds -> start game
    inIdleMode = true;
    gameActive = true;
    score = 0;
    cactusPos = 15;
    dinoPos = 1;
    gameSpeed = 500;
    initDinoGame();
    lastActivity = currentTime; // Reset activity timer for game
    Serial.println(F("Idle on scan screen: Starting Dino Game"));
  } else if (inIdleMode && gameActive && (currentTime - lastActivity > gameIdleTimeout)) {
    // Idle in game for 5 seconds -> return to scan screen
    inIdleMode = false;
    gameActive = false;
    showScanMessage();
    lastActivity = currentTime; // Reset activity timer for scan screen
    Serial.println(F("Idle in game: Returning to Scan Screen"));
  }
  
  // Update game if active
  if (inIdleMode && gameActive && currentTime - lastScroll > gameSpeed) {
    updateDinoGame();
    lastScroll = currentTime;
  }
}

void showScanMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan Your Card");
}

void initDinoGame() {
  lcd.clear();
  // Draw ground line
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
    lcd.write(6); // Ground character
  }
  
  // Show game instructions and score
  lcd.setCursor(0, 0);
  lcd.print("SCAN=JUMP  S:");
  lcd.print(score);
  
  // Draw initial dino on ground
  dinoJumping = false;
  jumpHeight = 0;
  lcd.setCursor(dinoPos, 1);
  lcd.write(3); // Dino frame 1
}

void updateDinoGame() {
  unsigned long currentTime = millis();
  
  // Handle dino jumping physics
  if (dinoJumping) {
    int jumpProgress = currentTime - jumpStartTime;
    if (jumpProgress >= jumpDuration) {
      // Jump finished - land dino
      dinoJumping = false;
      jumpHeight = 0;
    } else {
      // Calculate jump height (arc motion)
      float progress = (float)jumpProgress / jumpDuration;
      jumpHeight = (progress < 0.5) ? 1 : 0; // Simple up/down jump
    }
  }
  
  // Clear previous positions
  lcd.setCursor(dinoPos, 0);
  lcd.print(" ");
  lcd.setCursor(dinoPos, 1);
  lcd.write(6); // Ground
  
  if (cactusPos >= 0 && cactusPos < 16) {
    lcd.setCursor(cactusPos, 1);
    lcd.write(6); // Ground
  }
  
  // Draw dino at correct height
  dinoFrame = !dinoFrame;
  int dinoRow = (jumpHeight > 0) ? 0 : 1; // Row 0 = jumping, Row 1 = ground
  lcd.setCursor(dinoPos, dinoRow);
  lcd.write(dinoFrame ? 3 : 4); // Alternate between dino frames
  
  // Move cactus
  cactusPos--;
  
  // Draw cactus if on screen
  if (cactusPos >= 0 && cactusPos < 16) {
    lcd.setCursor(cactusPos, 1);
    lcd.write(5); // Cactus
  }
  
  // Check collision (only if dino is on ground)
  if (cactusPos == dinoPos && jumpHeight == 0) {
    gameOver();
    return;
  }
  
  // Reset cactus and increase score
  if (cactusPos < 0) {
    cactusPos = 15 + random(3, 8); // Random spacing
    score++;
    
    // Update score display
    lcd.setCursor(0, 0);
    lcd.print("SCAN=JUMP  S:");
    lcd.print(score);
    
    // Increase speed every 3 points
    if (score % 3 == 0 && gameSpeed > 150) {
      gameSpeed -= 30;
    }
  }
}

void jumpDino() {
  if (!dinoJumping && gameActive) {
    dinoJumping = true;
    jumpStartTime = millis();
    jumpHeight = 1;
    
    // Play a short beep for jump feedback
    myDFPlayer.volume(10); // Lower volume for game sounds
    myDFPlayer.play(3); // Use track 3 for jump sound (optional)
    delay(50);
    myDFPlayer.stop();
    myDFPlayer.volume(currentVolume);
    
    Serial.println(F("Dino Jump!"));
    lastActivity = millis(); // Update last activity on jump
  }
}

void gameOver() {
  gameActive = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GAME OVER!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  lcd.print(" SCAN=RETRY");
  
  // Wait for card scan to restart
  Serial.println(F("Game Over! Scan any card to restart."));
  
  // Reset for next game
  score = 0;
  cactusPos = 15;
  gameSpeed = 500;
  dinoJumping = false;
  jumpHeight = 0;
}

void showScreensaver() {
  // This function is kept for compatibility but replaced by dino game
  initDinoGame();
}

void animateScreensaver() {
  // This function is kept for compatibility but replaced by dino game
  updateDinoGame();
}

void nextTrack() {
  currentTrack = (currentTrack >= totalTracks) ? 1 : currentTrack + 1;
  myDFPlayer.play(currentTrack);
  isPlaying = true;
  showTrackInfo();
}

void prevTrack() {
  currentTrack = (currentTrack <= 1) ? totalTracks : currentTrack - 1;
  myDFPlayer.play(currentTrack);
  isPlaying = true;
  showTrackInfo();
}

void volumeUp() {
  if (currentVolume < 30) {
    currentVolume++;
    myDFPlayer.volume(currentVolume);
    showVolumeInfo();
  }
}

void volumeDown() {
  if (currentVolume > 0) {
    currentVolume--;
    myDFPlayer.volume(currentVolume);
    showVolumeInfo();
  }
}

void showTrackInfo() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(0); // Music note
  lcd.print(" Track ");
  lcd.print(currentTrack);
  lcd.setCursor(0, 1);
  lcd.write(1); // Speaker
  lcd.print(" Volume: ");
  lcd.print(currentVolume);
  
  Serial.print(F("Playing Track: "));
  Serial.print(currentTrack);
  Serial.print(F(" Volume: "));
  Serial.println(currentVolume);
}

void showVolumeInfo() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(1); // Speaker
  lcd.print(" Volume: ");
  lcd.print(currentVolume);
  lcd.setCursor(0, 1);
  // Volume bar visualization
  int bars = map(currentVolume, 0, 30, 0, 16);
  for (int i = 0; i < bars; i++) {
    lcd.print((char)255); // Solid block
  }
}

void showMainScreen() {
  if (!inIdleMode) {
    showScanMessage();
  }
}

void startupLEDSequence() {
  // Fun LED startup sequence
  for (int i = 0; i < 3; i++) {
    digitalWrite(STATUS_LED, HIGH);
    delay(100);
    digitalWrite(VALID_LED, HIGH);
    delay(100);
    digitalWrite(INVALID_LED, HIGH);
    delay(100);
    digitalWrite(STATUS_LED, LOW);
    digitalWrite(VALID_LED, LOW);
    digitalWrite(INVALID_LED, LOW);
    delay(100);
  }
}

void bootAnimation() {
  // Simple boot message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Ready!");
  delay(1000);
}

void showWelcomeMessage() {
  // Removed - now goes directly to scan message
}

void showError(String message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ERROR:");
  lcd.setCursor(0, 1);
  lcd.print(message);
  
  // Blink error LED
  for (int i = 0; i < 10; i++) {
    digitalWrite(INVALID_LED, HIGH);
    delay(200);
    digitalWrite(INVALID_LED, LOW);
    delay(200);
  }
}

void updateStatusLED() {
  static unsigned long lastBlink = 0;
  static bool ledState = false;
  
  if (millis() - lastBlink > 1000) { // Blink every second
    ledState = !ledState;
    digitalWrite(STATUS_LED, ledState);
    lastBlink = millis();
  }
}

void printUID(byte* uid, byte size) {
  Serial.print(F("Scanned UID: "));
  for (byte i = 0; i < size; i++) {
    Serial.print(uid[i] < 0x10 ? " 0" : " ");
    Serial.print(uid[i], HEX);
  }
  Serial.println();
}