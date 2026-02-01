#include <Arduino.h>

// Пин зуммера
#define SPEAKER_PIN 2

// частоты нот
#define NOTE_B6 1976
#define NOTE_E7 2637
#define NOTE_C7 2093
#define NOTE_G7 3136
#define NOTE_G6 1568
#define NOTE_A6 1760
#define NOTE_E6 1319
#define NOTE_AS6 1865
#define NOTE_A7 3520
#define NOTE_F7 2794
#define NOTE_D7 2349

// Ноты Super Mario Theme
const int melody[] = {
    NOTE_E7, NOTE_E7, 0, NOTE_E7,
    0, NOTE_C7, NOTE_E7, 0,
    NOTE_G7, 0, 0, 0,
    NOTE_G6, 0, 0, 0,

    NOTE_C7, 0, 0, NOTE_G6,
    0, 0, NOTE_E6, 0,
    0, NOTE_A6, 0, NOTE_B6,
    0, NOTE_AS6, NOTE_A6, 0,

    NOTE_G6, NOTE_E7, NOTE_G7,
    NOTE_A7, 0, NOTE_F7, NOTE_G7,
    0, NOTE_E7, 0, NOTE_C7,
    NOTE_D7, NOTE_B6, 0, 0};

// Длительности нот: 4 = четвертная, 8 = восьмая и т. д.
const int noteDurations[] = {
    8, 8, 8, 8,
    8, 8, 8, 8,
    4, 8, 8, 8,
    4, 8, 8, 8,

    4, 8, 8, 4,
    8, 8, 4, 8,
    8, 4, 8, 4,
    8, 8, 4, 8,

    4, 8, 4,
    4, 8, 4, 4,
    8, 4, 8, 4,
    4, 4, 8, 8};

void setup() {
    for (int thisNote = 0; thisNote < sizeof(melody) / sizeof(int); thisNote++) {
        int noteDuration = 1000 / noteDurations[thisNote];
        if (melody[thisNote] == 0) {
            noTone(SPEAKER_PIN);
        } else {
            tone(SPEAKER_PIN, melody[thisNote], noteDuration);
        }

        delay(noteDuration);
        noTone(SPEAKER_PIN);
    }
}

void loop() {
}