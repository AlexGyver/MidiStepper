#include <Arduino.h>
#include <MIDISerial.h>
MIDISerial midi(Serial);

#include <MIDIPlayer.h>

// ========== nema17 ==========
#define STEP_X 2
#define STEP_Y 3
#define STEP_Z 4
#define STEP_A 12

MIDIPlayer* nema17_arr[] = {
    new MIDIPlayerSoft(STEP_X),
    new MIDIPlayerSoft(STEP_Y),
    new MIDIPlayerSoft(STEP_Z),
    new MIDIPlayerSoft(STEP_A),
};
const uint8_t nema17_len = sizeof(nema17_arr) / sizeof(nema17_arr[0]);
MIDIPlayerMulti nema17(nema17_arr, nema17_len);

// ========== percussion ==========
#define HAT_PIN 13

void setup() {
    Serial.begin(115200);
    pinMode(HAT_PIN, OUTPUT);
    midi.onMIDI([](const MIDIMsg& msg) {
        switch (msg.cmd) {
            case MIDICmd::NoteOn:
                if (msg.channel == 9 || msg.channel == 10) {
                    // drums
                    switch (msg.note()) {
                        // hat
                        case 42:
                        case 44:
                        case 46:
                        case 53:
                            gio::write(HAT_PIN, !gio::read(HAT_PIN));
                            break;
                    }
                } else {
                    // notes
                    nema17.play(msg.note());
                }
                break;

            case MIDICmd::NoteOff:
                if (msg.channel == 9 || msg.channel == 10) {
                    // drums
                } else {
                    // notes
                    nema17.stop(msg.note());
                }
                break;

            case MIDICmd::ProgChange:
                nema17.stop();
                break;

            default: break;
        }
    });
}

void loop() {
    midi.tick();
    nema17.tick();
}