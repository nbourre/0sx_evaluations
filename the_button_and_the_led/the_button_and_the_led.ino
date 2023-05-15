#include <OneButton.h>

enum AppState {OFF, PULSE, BLINK};

AppState appState = OFF;

unsigned long currentTime;

#define LED_PIN LED_BUILTIN
#define BTN_PIN 2

bool debugging = false;

String serialInput = "";      // a String to hold incoming data
bool stringRecue = false;  // whether the string is complete

int blinkRate = 1000;
int pulseRate = 2;

OneButton button(BTN_PIN);

// Cette fonction est appelée AUTOMATIQUEMENT
// lorsqu'il y a des données reçues via
// le port série.
void serialEvent() {
  static char inChar = '\0';

  while (Serial.available()) {
    // get the new byte:
    inChar = (char)Serial.read();
    // add it to the inputString:
    serialInput += inChar;
    // Après la lecture d'une nouvelle ligne, lever le drapeau pour que
    // le programme principal fasse de quoi avec
    if (inChar == '\n') {
      stringRecue = true;
    }
  }
}


void stopState() {

}

// État du clignotement
void blinkState() {
  static unsigned long lastTime = 0;
  static int val = 0;

  if (currentTime - lastTime < blinkRate) return;
  lastTime = currentTime;

  val = !digitalRead(LED_PIN);

  digitalWrite (LED_PIN, val);
}


// État de la pulsation
void pulseState() {
  static unsigned long lastTime = 0;
  static int val = 0;
  static int increment = 1;

  if (currentTime - lastTime < pulseRate) return;
  
  lastTime = currentTime;

  val += increment;
  if (val >= 255 || val <= 0) {
    increment *= -1;
  }  

  analogWrite(LED_PIN, val);
 
}

// Fonction pour la gestion des commandes 
// reçues du port série
void commandeTask() {
  // Aucune commande de reçu
  if (!stringRecue) return;

  // La commande est seulement le
  // premier caractère
  char cmd = serialInput[0];

  switch (cmd) {
    case '0':
      blinkRate = 0;
      pulseRate = 0;
      break;
    case '1'...'9':
      // Hack : Le caractère '0' = 48, '1' = 49, etc.
      // donc '0' - 48 = 0 en valeur numérique
      int valeur = cmd - 48;
      blinkRate = 1000/valeur;
      pulseRate = 2 * valeur;
      break;
    default:
      blinkRate = 1000/20;
      break;
  }
  
  // Remettre le drapeau à faux
  stringRecue = false;
  serialInput = "";
  
  if (debugging) {
    Serial.print ("serialInput : ");
    Serial.print (serialInput);
    Serial.print ("\tCommande : ");
    Serial.println (cmd);
  }
}


// Gestion du clic du bouton
void btnClick() {
  Serial.print ("Clique : ");

  switch (appState) {
    case BLINK:
      appState = PULSE;
      Serial.println ("Pulsation");
      break;
    case PULSE:
      appState = BLINK;
      Serial.println ("Clignotement");
      break;
  }
}

// Gestion du long clic du bouton
void btnLongPressStart() {
  Serial.print ("Long press : ");
  switch (appState) {
    case OFF:
      appState = BLINK;
      Serial.println ("Clignotement");
      break;
    case PULSE:
    case BLINK:
      appState = OFF;
      Serial.println ("Arrêt");
      break;
  }
}

void setup() {
  Serial.begin(9600);

  pinMode (LED_PIN, OUTPUT);
  pinMode (LED_PIN, OUTPUT);
  pinMode (BTN_PIN, INPUT_PULLUP);

  button.attachClick(btnClick);
  button.attachLongPressStart(btnLongPressStart);
  
  Serial.println("Setup done!");
}

void loop() {
  currentTime = millis();

  commandeTask();

  switch (appState) {
    case OFF:
      stopState();
      break;
    case PULSE:
      pulseState();
      break;
    case BLINK:
      blinkState();
      break;
  }

  button.tick();
}

// Copier-coller cette fonction pour faire
// une tâche ou etat au lieu de retaper le code 
// similaire à chaque fois
void modelTask() {
  static unsigned long lastTime = 0;
  int rate = 250; // Adapter à la convenance

  if (currentTime - lastTime < rate) return;

  lastTime = currentTime;

  // Ajouter le code ici
}

