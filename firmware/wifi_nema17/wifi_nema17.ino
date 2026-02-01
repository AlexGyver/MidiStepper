#include <Arduino.h>
#include <EEPROM.h>
#include <LittleFS.h>
#include <SettingsGyver.h>
SettingsGyver sett("MIDI Player");

// ========== nema17 ==========
#include <MIDINote.h>
#include <MIDIPlayer.h>
#define STEP_X D1
#define STEP_Y D2
#define STEP_Z D3
#define STEP_A D4

MIDIPlayer* nema17_arr[] = {
    new MIDIPlayerSoft(STEP_X),
    new MIDIPlayerSoft(STEP_Y),
    new MIDIPlayerSoft(STEP_Z),
    new MIDIPlayerSoft(STEP_A),
};
const uint8_t nema17_len = sizeof(nema17_arr) / sizeof(nema17_arr[0]);
MIDIPlayerMulti nema17(nema17_arr, nema17_len);

// =========== play ==========
MIDINote note;
File notes;
bool playing = false;
uint32_t tmr;

void stop() {
    playing = false;
    nema17.stop();
}

// =========== cred ==========
const char def_key = 'A';
struct Cred {
    char key = def_key;
    char ssid[32] = {};
    char pass[32] = {};
};
Cred cred;

// ========= builder =========
void build(sets::Builder& b) {
    {
        sets::Group g(b, "Player");
        if (b.SelectText("Select file", sett.fs.listDir())) {
            notes = sett.fs.openRead(b.build.value.c_str());
            b.reload();
            stop();
        }
        b.Label("Current", notes.name());
        {
            sets::Buttons btns(b);
            if (b.Button("Play")) {
                notes.seek(0);
                playing = true;
                note.delay = 0;
                tmr = millis();
            }
            if (b.Button("Stop")) {
                stop();
            }
        }
    }
    {
        sets::Group g(b, "WiFi");
        b.clearSet();
        b.Pass("SSID", AnyPtr(cred.ssid, sizeof(cred.ssid)));
        b.Pass("Pass", AnyPtr(cred.pass, sizeof(cred.pass)));

        if (b.wasSet()) {
            EEPROM.put(0, cred);
            EEPROM.commit();
        }

        if (b.Button("Connect")) {
            WiFi.begin(cred.ssid, cred.pass);
        }
    }
}

// ========== setup ==========
void setup() {
    Serial.begin(115200);

    EEPROM.begin(100);
    EEPROM.get(0, cred);
    if (cred.key != def_key) {
        cred = Cred();
        EEPROM.put(0, cred);
        EEPROM.commit();
    }

    WiFi.begin(cred.ssid, cred.pass);
    WiFi.softAP("MIDI Player");

    LittleFS.begin();
    sett.begin();
    sett.onBuild(build);
}

// ========== loop ==========
void loop() {
    sett.tick();
    nema17.tick();

    if (playing && millis() - tmr >= note.delay) {
        tmr += note.delay;

        if (notes.available() >= (int)sizeof(MIDINote)) {
            notes.readBytes((char*)&note, sizeof(MIDINote));
            nema17.play(note.note, note.getDur());
        } else {
            stop();
        }
    }

    static wl_status_t stat;
    if (stat != WiFi.status()) {
        stat = WiFi.status();
        sett.reload();
    }
}