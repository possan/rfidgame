//
// Game loop
//

#include <string.h>
#include <avr/pgmspace.h>
#include <LiquidCrystal.h>

#include "pins.h"
#include "constants.h"

LiquidCrystal lcd(LCD_PIN0, LCD_PIN1, LCD_PIN2, LCD_PIN3, LCD_PIN4, LCD_PIN5);

char current_target_helpline1[25] = { 0, };
char current_target_helpline2[25] = { 0, };
char current_target_tag[20] = { 0, };
char current_target_loc[10] = { 0, };
char current_hehe_loc1[10] = { 0, };
char current_hehe_loc2[10] = { 0, };
int current_hehe_offset = 0;

//
// Ladda zonerna, *here be dragons*
//

typedef struct {
  unsigned char index;
  unsigned char hahachance;
  unsigned char holdchance;
} Group;
const int GROUP_SIZE     = 3;

typedef struct {
  unsigned char level; // +1 = 1
  char alias[6];       // +6 = 7
  char display[6];     // +3 = 10
  char location[5];    // +5 = 15
  char hintline1[21];  // +21 = 36
  char hintline2[21];  // +21 = 47
} Zone;
const int ZONE_SIZE     = 1+6+6+5+21+21;

typedef struct {
  char rfidcode[13];   // 0 +13 = 13
  char alias[6];       // 13 +6 = 19
} Tag;
const int TAG_SIZE       = 19;

#define BEGINGROUPS(NNN)\
  const int number_of_levels = NNN;\
  PROGMEM const Group groups[] = {

#define GROUP(_index, _hahachance, _holdchance)\
  {_index, _hahachance, _holdchance},

#define BEGINZONES(NNN)\
  const int number_of_zones = NNN;\
  PROGMEM const Zone zones[] = {

#define ZONE(_group, _tag, _location, _hintline1, _hintline2)\
  {_group, _tag, _tag, _location, _hintline1, _hintline2},

#define ENDGROUPS()\
  };

#define ENDZONES()\
  };

extern const int number_of_tags;
extern const int number_of_levels;
extern const int number_of_zones;
extern const Zone zones[];
extern const Group groups[];
extern const Tag tags[];

#define BEGINTAGS(NNN)\
  const int number_of_tags = NNN;\
  PROGMEM const Tag tags[] = {

#define ENDTAGS() };

#define DEFINETAG(_rfid, _alias) { _rfid, _alias },


void _pgm_get_tag(int index, void *output) {
    memcpy_P(output, ((char *)&tags)+(TAG_SIZE*index), TAG_SIZE);
}

void _pgm_get_group(int index, void *output) {
    memcpy_P(output, ((char *)&groups)+(GROUP_SIZE*index), GROUP_SIZE);
}

void _pgm_get_zone(int index, void *output) {
    memcpy_P(output, ((char *)&zones)+(ZONE_SIZE*index), ZONE_SIZE);
}



long display_deadline = 0;

void backlight_force(int on = 1) {
  if (on) {
    display_deadline = seconds() + BACKLIGHT_TIME;
    digitalWrite(LCD_BACKLIGHT, 1);
  } else{
    display_deadline = seconds() + 3*BACKLIGHT_TIME;
    digitalWrite(LCD_BACKLIGHT, 0);
  }
}

void backlight_check() {
  if (digitalRead(BUTTON1))
    backlight_force();

  digitalWrite(LCD_BACKLIGHT, (seconds() <= display_deadline));
}





int current_level_index;
Group current_level;
Zone current_point;
Zone distraction1;
Zone distraction2;
int distraction_offset;

void setup() {

  lcd.begin(20, 4);

  pinMode(LCD_BACKLIGHT, OUTPUT);
  pinMode(BUTTON1, INPUT);
  backlight_force();

  Serial.begin(9600);

 // randomSeed(0);
 // srand(0);
 //  initzones();

  tag_clear();
  backlight_force();

  box_reset();
  // game_reset();

  // testkod
  /*
  Zone tmpzone;
  for (int i=0; i<number_of_zones; i++) {
    _pgm_get_zone(i, &tmpzone);
    Serial.write("zone #");
    Serial.print(i);
    Serial.write(" - level ");
    Serial.print(tmpzone.level);
    Serial.write(": ");
    Serial.write(tmpzone.location);
    Serial.write("\n");
  }
  Serial.write("test start:\n");
  for (int l=0; l<number_of_levels; l++) {
    Serial.write("level #");
    Serial.print(l);
    Serial.write("\n");
    for (int r=0; r<20; r++) {
      Serial.write("randomization test #");
      Serial.print(r);
      Serial.write(": ");
      Zone tmp;
      find_random_zone_in_current_level(l, &tmp);
      Serial.write(" tag=");
      Serial.write(tmp.alias);
      Serial.write(" location=");
      Serial.write(tmp.location);
      Serial.write("\n");
    }
    Serial.write("\n");
  }
  Serial.write("\n");
  */
}

long seconds() {
  return millis() / 1000;
}


char _tag[30];
char _alias[30];
char _buffer[30];
int _bufpos = 0;

void tag_clear() {
  _bufpos = 0;
}


void tag_find_alias_by_tag(char *tag, char *alias) {
  strcpy(_alias, "?????");
  for (int i=0; i<number_of_tags; i++) {
    Tag temptag;
    _pgm_get_tag(i, (Tag *)&temptag);
    if (strcmp(temptag.rfidcode, _tag) == 0) {
      strcpy(_alias, temptag.alias);
    }
  }
}

bool tag_available() {
  // hantera taggläsning
  if (Serial.available() <= 0)
    return false;
  unsigned char incoming = (unsigned char)Serial.read();
  // packet structure, see ID-12 datasheet
  // STX A B C D E F G H I J CR LF ETX
  //   2 - - - - - - - - - - 13 10 3
  if(incoming == 2 || incoming == 3 || incoming == 10) {
  } else if(incoming == 13) {
    strncpy(_tag, _buffer, _bufpos);
    tag_find_alias_by_tag(_tag, _alias);
    _bufpos = 0;
    memset(_buffer, 0, 30);
    return true;
  }
  else {
    _buffer[_bufpos] = incoming;
    _bufpos ++;
  }
  return false;
}






enum LOOPS {
  LOOP_NONE = 0,
  LOOP_WELCOME,
  LOOP_DEBUG,
  LOOP_FIND1,
  LOOP_FIND3,
  LOOP_HOLD,
  LOOP_RESPAWN,
  LOOP_END
};

LOOPS currentloop = LOOP_NONE;




void loop() {
  while(true) {
    backlight_check();
    if (tag_available()) {
      backlight_force();
      // vi har scannat nått!
      switch(currentloop) {
        case LOOP_WELCOME: welcome_scan(_tag, _alias); break;
        case LOOP_DEBUG: debug_scan(_tag, _alias); break;
        case LOOP_FIND1: find1_scan(_tag, _alias); break;
        case LOOP_FIND3: find3_scan(_tag, _alias); break;
        case LOOP_HOLD: hold_scan(_tag, _alias); break;
        case LOOP_RESPAWN: respawn_scan(_tag, _alias); break;
        case LOOP_END: end_scan(_tag, _alias); break;
      }
    }
    switch(currentloop) {
      case LOOP_WELCOME: welcome_loop(); break;
      case LOOP_DEBUG: debug_loop(); break;
      case LOOP_FIND1: find1_loop(); break;
      case LOOP_FIND3: find3_loop(); break;
      case LOOP_HOLD: hold_loop(); break;
      case LOOP_RESPAWN: respawn_loop(); break;
      case LOOP_END: end_loop(); break;
    }
    delay(5);
  }
}








void lcd_clear() {
  lcd.clear();
  lcd.setCursor(0, 0);
}

void lcd_print(int col, int row, char *str) {
  lcd.setCursor(col, row);
  lcd.print(str);
}

void lcd_print4(char *line0, char *line1, char *line2, char *line3) {
	lcd_clear();
	lcd_print(0, 0, line0);
	lcd_print(0, 1, line1);
	lcd_print(0, 2, line2);
	lcd_print(0, 3, line3);
}

void display_banner(char *line0, char *line1, char *line2, char *line3) {
	backlight_force();
	lcd_print4(line0, line1, line2, line3);
	delay(BANNER_MESSAGE_TIME * 1000);
	lcd_clear();
}







void box_reset() {
  welcome_begin();
}

void welcome_begin() {
  currentloop = LOOP_WELCOME;
  welcome_draw();
}

void welcome_draw() {
  // Skriv ut welcomeskärmen
  backlight_force();
  lcd_clear();
  lcd_print4(
  	//____________________
  	"      Welcome!      ",
  	"",
  	"Scan CHEAT to debug.",
  	"Scan SPAWN to start."
  );
}

void welcome_loop() {
  randomSeed(millis());
  srand(millis());
}

void welcome_scan(char *tag, char *alias) {
  // om SPAWN -> start spelet -> game_reset()
  // om CHEAT -> debugbös
  if (strcmp(alias, "SPAWN") == 0) {
  	game_reset();
  } else if (strcmp(alias, "CHEAT") == 0) {
  	debug_begin();
  }
}








void game_reset() {
 	// currentloop = LOOP_NONE;
  // resetta alla scores å räknare
  current_level_index = -1;
  game_begin_next_level();
}

char debug_lasttag[20] = {0, };
char debug_lastloc[20] = {0, };
char debug_lasttype[20] = {0, };

void debug_begin() {
  Serial.write("debug_begin");
  currentloop = LOOP_DEBUG;
  strcpy(debug_lasttag, "????????????");
	strcpy(debug_lastloc, "????");
	strcpy(debug_lasttype, "?????");
  debug_draw();
}

void debug_draw() {
  backlight_force();
	lcd_clear();
	lcd_print(0, 0, "Scan something!");

	lcd_print(0, 2, "TAG:");
	lcd_print(4, 2, debug_lasttag);
	lcd_print(0, 3, "TYP:");
	lcd_print(4, 3, debug_lasttype);
	lcd_print(10, 3, "LOC:");
	lcd_print(14, 3, debug_lastloc);
}

void debug_loop() {
  // kolla knappen! exita om tryck
  if (digitalRead(BUTTON1) == 1) {
  	box_reset();
  }
}

void debug_scan(char *tag, char *alias) {
	strcpy(debug_lasttag, tag);
	strcpy(debug_lasttype, alias);

	// kolla upp tagg

  strcpy(debug_lastloc, "????");
  Zone tmpzone;
  for (int i=0; i<number_of_zones; i++) {
    _pgm_get_zone(i, &tmpzone);
    if (strcmp(tmpzone.alias, alias) == 0)
      strcpy(debug_lastloc, tmpzone.location);
  }

  debug_draw();
}

















void game_begin_next_level() {
  // börja nästa level (eller första leveln)
  if (current_level_index >= number_of_levels-1) {
    end_begin();
    return;
  }
  current_level_index ++; // 0 om första
  _pgm_get_group(current_level_index, &current_level);
  find_random_zone_in_current_level(current_level_index, &current_point);
  game_begin_current_level();
}


void find_random_zone_in_current_level(int level, void *output) {
  // find n point in a level
  int cnt = 0;

  Zone tmpzone;
  for (int i=0; i<number_of_zones; i++) {
    _pgm_get_zone(i, &tmpzone);
    if (tmpzone.level == level)
      cnt ++;
  }

  // gå igenom alla zoner och räkna antalet i nuvarande bara, sen igen och ta N
  if (cnt<=0)
    return;

  int r = rand() % cnt;

  cnt = 0;
  for (int i=0; i<number_of_zones; i++) {
    _pgm_get_zone(i, &tmpzone);
    if (tmpzone.level == level) {
      if (cnt == r) {
        _pgm_get_zone(i, output);
        return;
      }
      cnt ++;
    }
  }
}


void game_begin_current_level() {
  // Skriv ut "välkommen till bana X" eller "sista banan"
	backlight_force();
  lcd_clear();

  if (current_level_index == number_of_levels-1) {
    lcd.setCursor(0, 1);
    //         01234012340123401234
    lcd.print("     LAST LEVEL!");
    lcd.setCursor(0, 2);
    //         01234012340123401234
    lcd.print("     WAY TO GO!");
  } else {
    lcd.setCursor(0, 1);
    //         01234012340123401234
    lcd.print("     LEVEL #");
    lcd.print(current_level_index + 1);
  }

  // vänta ut bannertiden, och töm
  delay(BANNER_MESSAGE_TIME * 1000);
  lcd_clear();

  // kolla om vi ska jävlas, eller bara visa ett mål

  // välj ett mål
  find_random_zone_in_current_level(current_level_index, &current_point);
  // current_point_index = rand() % current_level.numzones;
  // current_point = (Zone *)&zones[current_point_index];
  // _pgm_get_zone(current_point_index, &current_point);


  int r1 = rand() % 100;  
 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Lvl: ");
  lcd.setCursor(13,0);
  lcd.print(current_level_index);
  lcd.setCursor(0,1);
  lcd.print("Hahachans:");
  lcd.setCursor(13,1);
  lcd.print(current_level.hahachance);
  lcd.setCursor(5,2);
  lcd.print(current_level.holdchance);
  lcd.setCursor(10,2);
  lcd.print(current_level.index);
  lcd.setCursor(0,2);
  lcd.print("Slump:");
  lcd.setCursor(13,2);
  lcd.print(r1, DEC);
  delay(5000);
    
  if (r1 < current_level.hahachance) {
    //  < 100
    // välja två andra platser
  	// hitta en
    while(true) {
      int l1 = rand() % (number_of_levels-1);
      find_random_zone_in_current_level(l1, &distraction1);
      if (distraction1.level != current_point.level &&
        strcmp(distraction1.alias, current_point.alias) != 0) {
        break;
      }
    }

  	// hitta två
    while(true) {
      int l2 = rand() % (number_of_levels-1);
      find_random_zone_in_current_level(l2, &distraction2);
      if (distraction1.level != distraction2.level &&
        strcmp(distraction1.alias, distraction2.alias) != 0 &&
        distraction2.level != current_point.level &&
        strcmp(distraction2.alias, current_point.alias) != 0) {
        break;
      }
    }

    distraction_offset = rand() % 99;

    find3_begin();
  }
  else {    
    find1_begin();
  }
}







void find1_begin() {
  currentloop = LOOP_FIND1;
  find1_draw();
}

void find3_begin() {
  // hitta 3 ...
  currentloop = LOOP_FIND3;
  find3_draw();
}

void find1_draw() {
	backlight_force();
  lcd_clear();
  //
  // Skriv ut "hitta tag X, på plats Y"
  //
  // ====================
  //   FIND 01 AT KORD
  //
  // ????????????????????
  // ????????????????????
  // ====================
  //
  //               ____ ____ ____ _____
  lcd_print(0, 0, "    FIND _____       ");
  lcd_print(0, 1, "      AT ____       ");
  // lcd_print(0, 2, "????????????????????");
  // lcd_print(0, 3, "????????????????????");
  lcd_print(9, 0, current_point.display );
  lcd_print(9, 1, current_point.location );
  lcd_print(0, 2, current_point.hintline1 );
  lcd_print(0, 3, current_point.hintline2 );
}

void find3_draw() {
	backlight_force();
  lcd_clear();
  //
  // Skriv ut "hitta tag X, den finns på Y, Z, W"
  //
  // ====================
  //      FIND 01 AT
  // KORD or KORD or KORD
  // ????????????????????
  // ????????????????????
  // ====================
  //
  //               ____ ____ ____ _____
  lcd_print(0, 0, "    FIND _____ AT     ");
  lcd_print(0, 1, "____ OR ____ OR ____");
  // lcd_print(0, 2, "????????????????????");
  // lcd_print(0, 3, "????????????????????");

  lcd_print(9, 0, current_point.display );
  lcd_print(0, 2, current_point.hintline1 );
  lcd_print(0, 3, current_point.hintline2 );

  switch(distraction_offset % 3) {
  case 0:
    lcd_print(0, 1, current_point.location );
    lcd_print(8, 1, distraction1.location );
    lcd_print(16, 1, distraction2.location );
    break;
  case 1:
    lcd_print(0, 1, distraction2.location );
    lcd_print(8, 1, current_point.location );
    lcd_print(16, 1, distraction1.location );
    break;
  case 2:
    lcd_print(0, 1, distraction1.location );
    lcd_print(8, 1, distraction2.location );
    lcd_print(16, 1, current_point.location );
    break;
  }
}

void find1_loop() {}
void find3_loop() {}

void findX_scan(char *tag, char *alias, int tre) {
  // om SPAWN -> game_respawn()
  // om CHEAT -> börja nästa level -> game_begin_next_level()
  // om RESET -> avsluta spel -> game_end()
  // om rätt TAGXX -> game_has_found_point()
  // om fel TAGXX -> game_found_wrong_point() -> game_begin_find3()

  if (strcmp(alias, "SPAWN") == 0) {
  	respawn_begin();
  } else if (strcmp(alias, "CHEAT") == 0) {
  	game_has_found_point();
  } else if (strcmp(alias, "RESET") == 0) {
  	end_begin();
  } else if (strcmp(alias, current_point.alias) == 0) {
  	game_has_found_point();
  } else {
  	game_found_wrong_point();
    if (tre)
      find3_draw();
    else
      find1_draw();
  }
}

void find1_scan(char *tag, char *alias) { findX_scan(tag, alias, 0); }
void find3_scan(char *tag, char *alias) { findX_scan(tag, alias, 1); }





void game_found_wrong_point() {
  display_banner(
    //1234012340123401234
  	"",
  	"    WRONG POINT    ",
  	"",
  	""
  );
}



void game_has_found_point() {

  if (current_level_index >= number_of_levels-1) {
    hold_begin(HOLD_LAST_REPETITIONS);
    return;
  }

  // kolla om vi ska hålla eller bara tagga
  int r2 = rand() % 100;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Lvl: ");
  lcd.setCursor(13,0);
  lcd.print(current_level_index);
  lcd.setCursor(0,1);
  lcd.print("Holdchans:");
  lcd.setCursor(13,1);
  lcd.print(current_level.holdchance);
  lcd.setCursor(0,2);
  lcd.print("Slump:");
  lcd.setCursor(13,2);
  lcd.print(r2);
  delay(5000);

  if (r2 < current_level.holdchance) {
  	// vi ska hålla punkten
    hold_begin(HOLD_STANDARD_REPETITIONS);
    return;
  }

  display_banner(
		//1234012340123401234
    "",
		"   CORRECT POINT!",
		"",
		""
	);

  // rätt_punkt++;
  game_begin_next_level();
}





















int hold_current_repetition = 0;
int hold_repetitions_ok = 0;
int hold_time = 0;
int hold_timer = 0;
long hold_start = 0;

void hold_begin(int _reps) {
  currentloop = LOOP_HOLD;
  hold_current_repetition = 1;
  hold_repetitions_ok = 0;
  hold_time = HOLD_COUNT_AND_TAGGING_DURATION * _reps;
  hold_timer = hold_time;
  hold_start = seconds();
  hold_draw();
}

int hold_frame = 0;

void hold_draw() {
  //
  // Skriv ut "håll punkten" (display på jämt, blink vid tagg!)
  //
  // ====================
  //    HOLD THE POINT
  //        05:00
  //
  //        WAIT...
  // TAG TAG TAG TAG TAG
  // ====================
  //
  if (hold_frame % 40 == 0)
    lcd_clear();
  hold_frame ++;
  lcd_print(0, 0, "HOLD THIS POINT!");
  lcd.setCursor(0, 1);
  lcd.print("TIME: ");
  lcd.print(hold_timer);
  lcd.print(" SEC");
  // lcd.print(hold_repetitions_ok);

  if (((hold_timer-1) % HOLD_COUNT_AND_TAGGING_DURATION) < HOLD_TAGGING_DURATION) {
    if (hold_repetitions_ok < hold_current_repetition) {
      backlight_force((millis() % 250) < 100);
      lcd.setCursor(0, 3);
      //         01234012340123401234
  
      lcd.print("  TAG!  TAG!  TAG!  ");
     /* int scroll = (hold_frame >> 1) % 20;
      switch (scroll) {
        case 0: lcd.print("TAG!  TAG!  TAG!  TA"); break;
        case 1: lcd.print("AG!  TAG!  TAG!  TAG"); break;
        case 2: lcd.print("G!  TAG!  TAG!  TAG!"); break;
        case 3: lcd.print("!  TAG!  TAG!  TAG! "); break;
        case 4: lcd.print("  TAG!  TAG!  TAG!  "); break;
        case 5: lcd.print(" TAG!  TAG!  TAG!  T"); break;
      }
      */
    } else {
      backlight_force();
      lcd.setCursor(0, 3);
      lcd.print("YOU'RE SAFE! ");
    }
  } else {
    backlight_force();
    lcd.setCursor(0, 3);
    lcd.setCursor(0, 2);
    lcd.print("TAG IN: ");
    lcd.print(max(0, (hold_timer % HOLD_COUNT_AND_TAGGING_DURATION) - HOLD_TAGGING_DURATION));
    lcd.print(" SEC. ");
  }
}

void hold_loop() {
  // räkna ner.. blinka kanske?
  // vid timeout:
  hold_timer = hold_time - (seconds() - hold_start);
  hold_current_repetition = 1 + (seconds() - hold_start) / HOLD_COUNT_AND_TAGGING_DURATION;
  if (hold_timer <= 0) {
    game_begin_next_level();
    return;
  }
  if (abs(hold_repetitions_ok - hold_current_repetition) >= 2) {
    game_begin_current_level();
    return;
  }
  hold_draw();
}

void hold_scan(char *tag, char *alias) {
  // om SPAWN -> game_respawn()
  // om CHEAT -> samma som rätt tag vid rätt tillfälle
  // om RESET -> avsluta spel -> game_end()
  // om rätt TAGXX vid rätt tid -> fortsätta
  // om rätt TAGXX vid fel tid -> skriv rätt men fel
  // om fel TAGXX -> "fel tagg" -> game_begin_current_level();
  // timeout -> "tiden gick ut" -> game_begin_current_level();

  if (strcmp(alias, "SPAWN") == 0) {
  	respawn_begin();
  } else if (strcmp(alias, "CHEAT") == 0) {
    game_begin_next_level();
  } else if (strcmp(alias, "RESET") == 0) {
  	end_begin();
  } else if (strcmp(alias, current_point.alias) == 0) {
    // om ej rätt tid?
    if ((hold_timer % HOLD_COUNT_AND_TAGGING_DURATION) < HOLD_TAGGING_DURATION) {
      if (hold_repetitions_ok < hold_current_repetition)
        hold_repetitions_ok ++;
    }
    else
      display_banner(
        //1234012340123401234
        "",
        "    WAIT FOR IT!  ",
        "",
        "");
      hold_draw();
    // annars ...
  } else {
    display_banner(
      //1234012340123401234
      "",
      "   GOOD JOB! NOT!  ",
      "",
      ""
    );
    game_begin_current_level();
  }

}





int respawn_timer = 0;
long respawn_start = 0;

void respawn_begin() {
  currentloop = LOOP_RESPAWN;
  respawn_timer = RESPAWN_TIME;
  respawn_start = seconds();
  respawn_draw();
}
int respawn_frame = 0;

void respawn_draw() {
  backlight_force();
if (respawn_frame % 40 == 0)
    lcd_clear();
  respawn_frame ++;
  
  lcd.setCursor(6, 1);
  lcd.print("RESPAWN IN ");
  lcd.setCursor(8, 2);
  lcd.print(respawn_timer);
  lcd.print(" SEC");
  //
  // ====================
  //
  //     RESPAWNING IN
  // _______300 SEC
  //         05:00
  // ====================
  //
}


void respawn_loop() {
  respawn_timer = RESPAWN_TIME - (seconds() - respawn_start);

  if (respawn_timer < 0) {
    game_begin_current_level();
    return;
  }
  respawn_draw();
}

void respawn_scan(char *tag, char *alias) {
  if (strcmp(alias, "RESET") == 0) { box_reset(); }
}














void end_begin() {
  currentloop = LOOP_END;
  end_draw();
}


void end_draw() {
  backlight_force();
  lcd_clear();
  //               01234012340123401234
  lcd_print(0, 0, "   GAME OVER MAN,  ");
  lcd_print(0, 1, "     GAME OVER!  ");
  lcd_print(0, 3, "   SHOUT 'GAME!!'  ");
  //
  // Skriv ut "game over"
  //
  // ====================
  //      GAME OVER!
  //   YOU WON, OR LOST
  //
  // Level: X of Y
  // You tagged XX pts.
  // You held XX pts.
  // ====================
  //
}

void end_loop() {
  // scrolla!
}

void end_scan(char *tag, char *alias) {
  // om SPAWN -> game_reset()
  // om RESET -> box_reset()
  if (strcmp(alias, "SPAWN") == 0) {
  	game_reset();
  } else if (strcmp(alias, "RESET") == 0) {
  	box_reset();
  }
}
















#include "tags.h"
#include "zones.h"





