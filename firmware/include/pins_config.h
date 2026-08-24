#pragma once
// ============================================================
//  DC Assistant - Mapa de pinos conforme datasheet
//  ES3C28P / ES3N28P (LCDWIKI) - 2.8" IPS ESP32-S3, V1.0
//  Secao 4.2 "ESP32-S3 pin allocation"
// ============================================================

// ---- LCD ILI9341V (SPI 4-line, 240x320) ----
#define PIN_LCD_CS    10   // GPIO10  chip select (ativo baixo)
#define PIN_LCD_DC    46   // GPIO46  D/C (alto = dados, baixo = comando)
#define PIN_LCD_SCLK  12   // GPIO12  SPI clock
#define PIN_LCD_MOSI  11   // GPIO11  SPI MOSI (escrita)
#define PIN_LCD_MISO  13   // GPIO13  SPI MISO (leitura)
#define PIN_LCD_RST   -1   // reset compartilhado com CHIP_PU (ESP32-S3)
#define PIN_LCD_BL    45   // GPIO45  backlight (alto = liga, PWM = brilho)

// ---- Touch FT6336G (I2C, endereco 0x38) ----
#define PIN_TP_SDA    16   // GPIO16  I2C SDA (compartilhado codec/expansao)
#define PIN_TP_SCL    15   // GPIO15  I2C SCL (compartilhado codec/expansao)
#define PIN_TP_RST    18   // GPIO18  reset touch (ativo baixo)
#define PIN_TP_INT    17   // GPIO17  interrupcao (baixo em evento de toque)

// ---- Audio (I2S + codec) ----
#define PIN_AUDIO_PA   1   // GPIO1   enable amp de potencia (baixo = liga)
#define PIN_AUDIO_MCLK 4   // GPIO4   I2S MCLK
#define PIN_AUDIO_BCLK 5   // GPIO5   I2S BCLK
#define PIN_AUDIO_DOUT 8   // GPIO8   I2S DOUT (saida de dados)
#define PIN_AUDIO_LRCK 7   // GPIO7   I2S LRCK (alto = dir, baixo = esq)
#define PIN_AUDIO_DIN  6   // GPIO6   I2S DIN (entrada de dados)

// ---- LED RGB (controlador interno, 1 IO) ----
#define PIN_RGB_LED   42   // GPIO42  LED RGB tricolor

// ---- MicroSD (SDIO 4-bit) ----
#define PIN_SD_CLK    38   // GPIO38  SDIO CLK
#define PIN_SD_CMD    40   // GPIO40  SDIO CMD
#define PIN_SD_D0     39   // GPIO39  SDIO DATA0
#define PIN_SD_D1     41   // GPIO41  SDIO DATA1
#define PIN_SD_D2     48   // GPIO48  SDIO DATA2
#define PIN_SD_D3     47   // GPIO47  SDIO DATA3

// ---- Bateria ----
#define PIN_BAT_ADC    9   // GPIO9   ADC tensao da bateria (entrada)

// ---- Botoes ----
#define PIN_BOOT       0   // IO0    BOOT / download mode
// CHIP_PU = reset (compartilhado com LCD)

// ---- USB / Serial ----
// GPIO19 (D-), GPIO20 (D+), GPIO43 (TX0), GPIO44 (RX0)

// ---- Expansao ----
// GPIO2, GPIO3, GPIO14, GPIO21 (I/O livres, podem ser SPI)

// ---- PSRAM OPI interno / Flash QSPI externo ----
// GPIO26 CS-PSRAM | GPIO29 CS-Flash | GPIO30 CLK | GPIO32/31/28/27 DATA0-3
// GPIO33-36 DATA4-7 | GPIO37 DM/DCK  -> NAO UTILIZAR
