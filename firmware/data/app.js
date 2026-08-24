/* ============================================================
   DC Assistant · Echo Mind — lógica da interface real
   Consome /api/status do ESP32-S3 e controla a navegação.
   ============================================================ */
(function () {
  'use strict';

  var screens = document.querySelectorAll('.screen');
  var navDots = document.querySelectorAll('.nav-dot');

  function showScreen(name) {
    screens.forEach(function (s) { s.classList.toggle('active', s.dataset.screen === name); });
    navDots.forEach(function (d) { d.classList.toggle('active', d.dataset.goto === name); });
  }

  document.querySelectorAll('[data-goto]').forEach(function (btn) {
    btn.addEventListener('click', function () { showScreen(btn.dataset.goto); });
  });

  // Tabs (Profile / Analytics / Shop)
  document.querySelectorAll('.tab').forEach(function (tab) {
    tab.addEventListener('click', function () {
      document.querySelectorAll('.tab').forEach(function (t) { t.classList.remove('active'); });
      tab.classList.add('active');
    });
  });

  // Botão de microfone (Voice)
  var micBtn = document.getElementById('mic-btn');
  var waveStage = document.getElementById('wave-stage');
  var kicker = document.getElementById('voice-kicker');
  var listening = false;
  micBtn.addEventListener('click', function () {
    listening = !listening;
    micBtn.classList.toggle('active', listening);
    waveStage.classList.toggle('listening', listening);
    kicker.textContent = listening ? "I'm listening..." : "Go ahead, I'm listening...";
    micBtn.setAttribute('aria-label', listening ? 'Stop listening' : 'Start listening');
  });

  // Status do dispositivo via /api/status
  function fmtUptime(s) {
    var h = Math.floor(s / 3600), m = Math.floor((s % 3600) / 60);
    return h + 'h ' + m + 'm';
  }
  function fetchStatus() {
    fetch('/api/status')
      .then(function (r) { return r.json(); })
      .then(function (d) {
        setText('st-wifi', d.wifi_mode === 'AP' ? 'AP (DC-Assistant)' : 'STA conectado');
        setText('st-ip', d.ip);
        setText('st-bat', d.battery_mv + ' mV');
        setText('st-sd', d.sd ? 'OK' : 'FALHA');
        setText('st-uptime', fmtUptime(d.uptime_s));
        setText('st-fw', d.version);
      })
      .catch(function () { setText('st-wifi', 'offline'); });
  }
  function setText(id, val) {
    var el = document.getElementById(id);
    if (el) el.textContent = val;
  }
  fetchStatus();
  setInterval(fetchStatus, 5000);

  // Simulação de FPS/CPU no rodapé da Home
  setInterval(function () {
    setText('fps', Math.floor(50 + Math.random() * 50) + ' FPS');
    setText('cpu', Math.floor(40 + Math.random() * 50) + '% CPU');
  }, 2000);
})();
