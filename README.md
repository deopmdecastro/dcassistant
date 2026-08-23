# DC Assistant - Echo Mind UI

Interface do assistente pessoal Echo Mind para o projeto DC 0.3 (Wi-Fi + NVS + audio), rodando no ESP32-S3 (ESP32-S3-DevKitC-1).

## Stack

- Frontend: Vite + React + TypeScript + Tailwind CSS
- Servidor local: Docker (nginx) + docker-compose
- Banco de dados local: PostgreSQL 16
- Firmware: PlatformIO (Arduino), ESP32-S3

## Rodar localmente (Docker)

docker compose up --build

- Frontend: http://localhost:8080
- Banco de dados: localhost:5432 (dcassistant / dcassistant_local)

## Firmware (ESP32-S3)

cd firmware
pio run -t upload
pio device monitor

## Estrutura

- src/ - Interface React (Echo Mind UI)
- firmware/ - Firmware PlatformIO (ESP32-S3)
- platformio.ini - Configuracao PlatformIO
- Dockerfile - Imagem do frontend (nginx)
- docker-compose.yml - Frontend + banco de dados
- nginx.conf - Configuracao do servidor
