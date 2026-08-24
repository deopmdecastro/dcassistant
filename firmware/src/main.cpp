// ============================================================
//  DC Assistant - Firmware ESP32-S3
//  Configurado conforme datasheet ES3C28P / ES3N28P (LCDWIKI)
//  2.8" IPS 240x320 | ILI9341V | FT6336G | Audio I2S | MicroSD
//  | LED RGB | Bateria LiPo | Wi-Fi + WebServer (interface real)
// ============================================================

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <SD_MMC.h>
#include <driver/i2s.h>
#include <driver/i2c.h>
#include <ArduinoJson.h>
#include <LovyanGFX.hpp>
#include "pins_config.h"

// ------------------------------------------------------------------
//  Display: ILI9341V 240x320 - SPI 4-line (pinos do datasheet)
// ------------------------------------------------------------------
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9341 _panel;
  lgfx::Bus_SPI      _bus;
  lgfx::Light_PWM    _light;
  lgfx::Touch_FT5x06 _touch;   // FT6336G (classe FT5x06 do LovyanGFX)

public:
  LGFX(void) {
    { // barramento SPI
      auto cfg = _bus.config();
      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000;
      cfg.freq_read  = 16000000;
      cfg.spi_3wire  = false;
      cfg.use_lock   = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk   = PIN_LCD_SCLK;   // GPIO12
      cfg.pin_mosi   = PIN_LCD_MOSI;   // GPIO11
      cfg.pin_miso   = PIN_LCD_MISO;   // GPIO13
      cfg.pin_dc     = PIN_LCD_DC;     // GPIO46
      _bus.config(cfg);
      _panel.setBus(&_bus);
    }
    { // painel
      auto cfg = _panel.config();
      cfg.pin_cs       = PIN_LCD_CS;   // GPIO10
      cfg.pin_rst      = PIN_LCD_RST;  // -1: reset compartilhado com CHIP_PU
      cfg.panel_width  = 240;
      cfg.panel_height = 320;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.invert   = true;             // ILI9341
      cfg.rgb_order = false;
      _panel.config(cfg);
    }
    { // backlight GPIO45 (PWM)
      auto cfg = _light.config();
      cfg.pin_bl      = PIN_LCD_BL;    // GPIO45
      cfg.invert      = false;
      cfg.freq        = 12000;
      cfg.pwm_channel = 7;
      _light.config(cfg);
      _panel.setLight(&_light);
    }
    { // touch FT6336G - I2C 0x38 (GPIO16/15)
      auto cfg = _touch.config();
      cfg.x_min = 0;   cfg.x_max = 239;
      cfg.y_min = 0;   cfg.y_max = 319;
      cfg.pin_int = PIN_TP_INT;        // GPIO17
      cfg.pin_rst = PIN_TP_RST;        // GPIO18
      cfg.bus_shared = true;
      cfg.i2c_addr   = 0x38;
      cfg.i2c_port   = I2C_NUM_0;
      cfg.pin_sda    = PIN_TP_SDA;     // GPIO16
      cfg.pin_scl    = PIN_TP_SCL;     // GPIO15
      cfg.freq       = 400000;
      _touch.config(cfg);
      _panel.setTouch(&_touch);
    }
    setPanel(&_panel);
  }
};

LGFX tft;

// ------------------------------------------------------------------
//  Constantes e estado global
// ------------------------------------------------------------------
static const char* kDeviceName      = "DC Assistant";
static const char* kFirmwareVersion = "DC 0.4";

Preferences preferences;
WebServer server(80);

static bool   sdReady = false;
static bool   wifiAP  = false;
static String wifiIP  = "0.0.0.0";
static bool   rgbOn   = false;

// ------------------------------------------------------------------
//  Servidor de arquivos (LittleFS) - serve a interface real
// ------------------------------------------------------------------
static const char* contentTypeOf(const String& path) {
  if (path.endsWith(".html")) return "text/html; charset=utf-8";
  if (path.endsWith(".css"))  return "text/css; charset=utf-8";
  if (path.endsWith(".js"))   return "application/javascript; charset=utf-8";
  if (path.endsWith(".json")) return "application/json";
  if (path.endsWith(".svg"))  return "image/svg+xml";
  if (path.endsWith(".png"))  return "image/png";
  if (path.endsWith(".ico"))  return "image/x-icon";
  return "text/plain; charset=utf-8";
}

static bool serveFromFS(const String& path) {
  String p = path;
  if (p == "/") p = "/index.html";
  if (!LittleFS.exists(p)) return false;
  File f = LittleFS.open(p, "r");
  if (!f) return false;
  server.streamFile(f, contentTypeOf(p));
  f.close();
  return true;
}

void handleRoot() {
  if (!serveFromFS("/index.html"))
    server.send(404, "text/plain", "index.html nao encontrado - rode 'pio run -t uploadfs'");
}

void handleFile() {
  if (!serveFromFS(server.uri()))
    server.send(404, "text/plain", "arquivo nao encontrado");
}

void handleStatus() {
  JsonDocument doc;
  doc["device"]      = kDeviceName;
  doc["version"]     = kFirmwareVersion;
  doc["wifi_mode"]   = wifiAP ? "AP" : "STA";
  doc["ip"]          = wifiIP;
  doc["battery_mv"]  = analogReadMilliVolts(PIN_BAT_ADC); // ADC bruto (sem calibracao)
  doc["sd"]          = sdReady;
  doc["uptime_s"]    = millis() / 1000;
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

// ------------------------------------------------------------------
//  Wi-Fi: STA com credenciais salvas, fallback em Access Point
// ------------------------------------------------------------------
void setupWiFi() {
  preferences.begin("dcassistant", true);
  String ssid = preferences.getString("ssid", "");
  String pass = preferences.getString("pass", "");
  preferences.end();

  if (ssid.length() > 0) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    unsigned long t0 = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t0 < 10000) delay(200);
    if (WiFi.status() == WL_CONNECTED) {
      wifiAP = false;
      wifiIP = WiFi.localIP().toString();
      return;
    }
  }
  // fallback: Access Point "DC-Assistant"
  wifiAP = true;
  WiFi.mode(WIFI_AP);
  WiFi.softAP("DC-Assistant", "dcassistant");
  wifiIP = WiFi.softAPIP().toString();
}

// ------------------------------------------------------------------
//  Audio I2S (GPIO4 MCLK, 5 BCLK, 7 LRCK, 8 DOUT, 6 DIN, PA GPIO1)
// ------------------------------------------------------------------
void setupAudio() {
  pinMode(PIN_AUDIO_PA, OUTPUT);
  digitalWrite(PIN_AUDIO_PA, LOW); // baixo = amp habilitado

  i2s_config_t cfg = {};
  cfg.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
  cfg.sample_rate = 16000;
  cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  cfg.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
  cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  cfg.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
  cfg.dma_buf_count = 8;
  cfg.dma_buf_len = 64;
  cfg.use_apll = false;
  cfg.tx_desc_auto_clear = true;
  cfg.fixed_mclk = 0;

  i2s_pin_config_t pins = {};
  pins.mck_io_num = PIN_AUDIO_MCLK;
  pins.bck_io_num = PIN_AUDIO_BCLK;
  pins.ws_io_num  = PIN_AUDIO_LRCK;
  pins.data_out_num = PIN_AUDIO_DOUT;
  pins.data_in_num  = PIN_AUDIO_DIN;

  if (i2s_driver_install(I2S_NUM_0, &cfg, 0, nullptr) == ESP_OK) {
    i2s_set_pin(I2S_NUM_0, &pins);
  }
}

void playBeep(uint16_t ms, uint16_t freq) {
  int16_t buf[256];
  for (int i = 0; i < 256; i++) {
    float t = (float)i / 16000.0f;
    buf[i] = (int16_t)(sinf(2.0f * PI * freq * t) * 12000.0f);
  }
  uint32_t written = 0;
  uint32_t total   = (ms * 16000) / 1000 * 2;
  while (written < total) {
    size_t w = 0;
    if (i2s_write(I2S_NUM_0, buf, sizeof(buf), &w, portMAX_DELAY) != ESP_OK) break;
    written += w;
  }
}

// ------------------------------------------------------------------
//  MicroSD (SDIO 4-bit)
// ------------------------------------------------------------------
void setupSD() {
  SD_MMC.setPins(PIN_SD_CLK, PIN_SD_CMD, PIN_SD_D0, PIN_SD_D1, PIN_SD_D2, PIN_SD_D3);
  sdReady = SD_MMC.begin("/sdcard", false); // false = modo 4-bit
}

// ------------------------------------------------------------------
//  Tela nativa (dashboard de status no LCD 240x320)
// ------------------------------------------------------------------
void drawStatus(int16_t tx, int16_t ty, bool touched) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(0xC27CFF, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(8, 10);
  tft.print("DC ASSISTANT");
  tft.setTextColor(0x9D8DA9, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(8, 34);
  tft.printf("FW %s | ESP32-S3", kFirmwareVersion);

  tft.drawFastHLine(8, 48, 224, 0x3A2B4A);

  tft.setTextColor(0xE6DEF0, TFT_BLACK);
  tft.setCursor(8, 60);
  tft.printf("WiFi: %s", wifiAP ? "AP (DC-Assistant)" : "STA");
  tft.setCursor(8, 76);
  tft.printf("IP:   %s", wifiIP.c_str());
  tft.setCursor(8, 92);
  tft.printf("Bat:  %d mV (ADC)", analogReadMilliVolts(PIN_BAT_ADC));
  tft.setCursor(8, 108);
  tft.printf("SD:   %s", sdReady ? "OK" : "FALHA");
  tft.setCursor(8, 124);
  tft.printf("LED:  %s", rgbOn ? "RGB on" : "RGB off");
  tft.setCursor(8, 140);
  tft.printf("Upt:  %lus", millis() / 1000);

  if (touched) {
    tft.setTextColor(0x69D89E, TFT_BLACK);
    tft.setCursor(8, 160);
    tft.printf("Touch: (%d,%d)", tx, ty);
    tft.fillCircle(tx, ty, 5, 0x69D89E);
  } else {
    tft.setTextColor(0x9D8DA9, TFT_BLACK);
    tft.setCursor(8, 160);
    tft.print("Touch: aguardando...");
  }

  tft.setTextColor(0xC27CFF, TFT_BLACK);
  tft.setCursor(8, 190);
  tft.print("Interface web:");
  tft.setTextColor(0xFFFFFF, TFT_BLACK);
  tft.setCursor(8, 206);
  tft.printf("http://%s", wifiIP.c_str());
  tft.setTextColor(0x9D8DA9, TFT_BLACK);
  tft.setCursor(8, 226);
  tft.print("Abra no navegador do celular");
}

// ------------------------------------------------------------------
//  Setup / Loop
// ------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(PIN_RGB_LED, OUTPUT);
  pinMode(PIN_BOOT, INPUT_PULLUP);

  // display
  if (tft.init()) {
    tft.setRotation(0); // retrato 240x320
    tft.setBrightness(255);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(0xC27CFF, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(8, 120);
    tft.print("DC Assistant");
    tft.setTextSize(1);
    tft.setTextColor(0x9D8DA9, TFT_BLACK);
    tft.setCursor(8, 150);
    tft.print("Inicializando...");
  }

  setupWiFi();
  setupAudio();
  setupSD();

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS: falha ao montar");
  }

  server.on("/", handleRoot);
  server.on("/api/status", handleStatus);
  server.onNotFound(handleFile);
  server.begin();

  playBeep(120, 880); // beep de boot

  Serial.printf("%s | %s | ESP32-S3 pronto | IP: %s\n", kDeviceName, kFirmwareVersion, wifiIP.c_str());
}

void loop() {
  server.handleClient();

  // leitura do touch FT6336G
  int16_t tx = -1, ty = -1;
  bool touched = tft.getTouch(&tx, &ty);

  // LED RGB piscando como status
  static unsigned long lastBlink = 0;
  if (millis() - lastBlink > 1000) {
    lastBlink = millis();
    rgbOn = !rgbOn;
    digitalWrite(PIN_RGB_LED, rgbOn ? HIGH : LOW);
  }

  // atualiza a tela nativa a cada segundo
  static unsigned long lastDraw = 0;
  if (millis() - lastDraw > 1000) {
    lastDraw = millis();
    drawStatus(tx, ty, touched);
  }

  delay(10);
}
