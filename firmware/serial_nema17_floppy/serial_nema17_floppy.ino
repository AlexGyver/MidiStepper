#include <Arduino.h>
#include <MIDISerial.h>
MIDISerial midi(Serial);

#include <MIDIPlayer.h>

// ========== floppy ==========
#define STEP_X 2
#define STEP_Y 3
#define STEP_Z 4
#define DIR_X 5
#define DIR_Y 6
#define DIR_Z 7

MIDIPlayer* floppy_arr[] = {
    new MIDIFloppy(STEP_X, DIR_X, 500),
    new MIDIFloppy(STEP_Y, DIR_Y, 500),
    new MIDIFloppy(STEP_Z, DIR_Z, 500),
};
const uint8_t floppy_len = sizeof(floppy_arr) / sizeof(floppy_arr[0]);
MIDIPlayerMulti floppy(floppy_arr, floppy_len);

// ========== nema17 ==========
#define X_LIM 9
#define Y_LIM 10
#define Z_LIM 11
#define SP_EN 12

MIDIPlayer* nema17_arr[] = {
    new MIDIPlayerSoft(X_LIM),
    new MIDIPlayerSoft(Y_LIM),
    new MIDIPlayerSoft(Z_LIM),
    new MIDIPlayerSoft(SP_EN),
};
const uint8_t nema17_len = sizeof(nema17_arr) / sizeof(nema17_arr[0]);
MIDIPlayerMulti nema17(nema17_arr, nema17_len);

#define FLOPPY_THRSH 55

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
                    if (msg.note() < FLOPPY_THRSH) floppy.play(msg.note());
                    else nema17.play(msg.note());
                }
                break;

            case MIDICmd::NoteOff:
                if (msg.channel == 9 || msg.channel == 10) {
                    // drums
                } else {
                    // notes
                    if (msg.note() < FLOPPY_THRSH) floppy.stop(msg.note());
                    else nema17.stop(msg.note());
                }
                break;

            case MIDICmd::ProgChange:
                nema17.stop();
                floppy.stop();
                break;

            default: break;
        }
    });
}

void loop() {
    midi.tick();
    nema17.tick();
    floppy.tick();
}