/* Adafruit IO public-feed adapter for the patient health dashboard. */
(() => {
  'use strict';

  const AIO = {
    username: 'GouravSingh',
    baseUrl: 'https://io.adafruit.com/api/v2/GouravSingh/feeds/',
    feeds: {
      heartRate: 'max30102-heart-rate',
      hw827HeartRate: 'hw-827-heart-rate',
      temperature: 'temperature',
      humidity: 'humidity'
    },
    pollMs: 5000,
    staleAfterMs: 45000
  };

  const $ = id => document.getElementById(id);
  const num = value => {
    if (value === null || value === undefined || value === '') return null;
    const parsed = Number(value);
    return Number.isFinite(parsed) ? parsed : null;
  };
  const setText = (id, value) => {
    const el = $(id);
    if (el) el.textContent = value;
  };

  let pollTimer = null;
  let clockTimer = null;
  let lastSampleSignature = '';
  let lastSampleAt = 0;

  function setStatus(online, message) {
    const status = $('header-conn-status');
    if (status) {
      status.textContent = online ? 'ONLINE' : 'OFFLINE';
      status.className = 'status-pill ' + (online ? 'status-pill-normal' : 'status-pill-offline');
    }
    const indicator = $('rx-packet-indicator');
    if (indicator) {
      indicator.style.opacity = online ? '1' : '.2';
      indicator.style.backgroundColor = online ? '#34d399' : '#94a3b8';
    }
    if (message) setText('header-last-sync', message);
  }

  function setSensorStatus(id, label, state) {
    const el = $(id);
    if (!el) return;
    el.textContent = label + ': ' + state;
    el.classList.toggle('text-emerald-400', state === 'ONLINE');
    el.classList.toggle('text-slate-400', state !== 'ONLINE');
  }

  async function readFeed(feedKey) {
    const url = AIO.baseUrl + encodeURIComponent(feedKey) + '/data/last_value?type=json';
    const response = await fetch(url, { cache: 'no-store' });
    if (!response.ok) throw new Error('Feed ' + feedKey + ' returned HTTP ' + response.status);
    const item = await response.json();
    return {
      value: num(item.value),
      createdAt: item.created_at ? Date.parse(item.created_at) : 0
    };
  }

  function evaluateStatus(v) {
    const issues = [];
    if (v.heartRate !== null && (v.heartRate < 50 || v.heartRate > 120)) {
      issues.push('Heart rate outside configured prototype range');
    }
    if (v.temperature !== null && (v.temperature < 35 || v.temperature > 38)) {
      issues.push('Temperature outside configured prototype range');
    }
    setText('overall-status-badge', issues.length ? 'CHECK' : 'READINGS RECEIVED');
    setText(
      'overall-status-message',
      issues.length ? issues.join(' • ') : 'Latest available sensor readings are displayed. This is an educational prototype, not a medical diagnosis.'
    );
    const box = $('overall-status-container');
    if (box) {
      box.classList.toggle('border-l-red-500', issues.length > 0);
      box.classList.toggle('border-l-emerald-500', issues.length === 0);
    }
  }

  function renderVitals(values, sampleTime) {
    const v = {
      heartRate: values.heartRate.value > 0 ? values.heartRate.value : null,
      hw827HeartRate: values.hw827HeartRate.value > 0 ? values.hw827HeartRate.value : null,
      temperature: values.temperature.value,
      humidity: values.humidity.value,
      spo2: null
    };

    setText('hr-value', v.heartRate === null ? '--' : Math.round(v.heartRate));
    setText('hw827-value', v.hw827HeartRate === null ? '--' : Math.round(v.hw827HeartRate));
    setText('temp-value', v.temperature === null ? '--' : v.temperature.toFixed(1));
    setText('humidity-value', v.humidity === null ? '--' : v.humidity.toFixed(1));

    // The current ESP32 firmware does not calculate or publish SpO2.
    setText('spo2-value', 'N/A');
    setText('spo2-status-badge', 'NOT AVAILABLE');
    setText('header-wifi-rssi', 'N/A');
    setText('header-battery-pct', 'N/A');
    const batteryBar = $('header-battery-bar');
    if (batteryBar) batteryBar.style.width = '0%';

    setSensorStatus('sensor-status-max30100', 'MAX30102', v.heartRate === null ? 'WAITING' : 'ONLINE');
    setSensorStatus('sensor-status-temp', 'DHT11', v.temperature === null ? 'WAITING' : 'ONLINE');
    setSensorStatus('sensor-status-esp32', 'ESP32', 'ONLINE');
    setSensorStatus('sensor-status-wifi', 'Wi-Fi', 'ONLINE');
    setSensorStatus('sensor-status-oled', 'OLED', 'NOT REPORTED');
    setSensorStatus('sensor-status-sd', 'SD card', 'NOT REPORTED');

    if (sampleTime) {
      setText('header-last-sync', new Date(sampleTime).toLocaleTimeString());
    }
    if (window.addHealthChartPoint) window.addHealthChartPoint({
      heartRate: v.heartRate,
      spo2: null,
      temperature: v.temperature
    });

    const body = $('live-table-body');
    if (body) {
      const tr = document.createElement('tr');
      tr.className = 'text-slate-300';
      const vals = [
        sampleTime ? new Date(sampleTime).toLocaleTimeString() : new Date().toLocaleTimeString(),
        v.heartRate === null ? '--' : Math.round(v.heartRate),
        'N/A',
        v.temperature === null ? '--' : v.temperature.toFixed(1) + ' °C',
        v.humidity === null ? '--' : v.humidity.toFixed(1) + '%',
        v.hw827HeartRate === null ? '--' : Math.round(v.hw827HeartRate),
        'Adafruit IO',
        'N/A'
      ];
      vals.forEach(value => {
        const td = document.createElement('td');
        td.className = 'px-3 py-2 text-xs';
        td.textContent = value;
        tr.appendChild(td);
      });
      body.prepend(tr);
      while (body.children.length > 50) body.lastElementChild.remove();
    }
    evaluateStatus(v);
  }

  async function pollAdafruitIO() {
    try {
      const keys = Object.keys(AIO.feeds);
      const results = await Promise.all(keys.map(key => readFeed(AIO.feeds[key])));
      const values = {};
      keys.forEach((key, index) => { values[key] = results[index]; });

      const timestamps = results.map(item => item.createdAt).filter(Boolean);
      const newestTimestamp = timestamps.length ? Math.max.apply(null, timestamps) : 0;
      const now = Date.now();
      const signature = keys.map(key => {
        const item = values[key];
        return String(item.value) + ':' + String(item.createdAt);
      }).join('|');

      if (signature !== lastSampleSignature) {
        lastSampleSignature = signature;
        lastSampleAt = newestTimestamp || now;
        renderVitals(values, lastSampleAt);
      }

      const age = newestTimestamp ? now - newestTimestamp : Infinity;
      const online = age >= 0 && age <= AIO.staleAfterMs;
      setStatus(
        online,
        newestTimestamp ? (online ? 'Updated ' + Math.round(age / 1000) + 's ago' : 'Feed data is stale') : 'No feed data yet'
      );

      setSensorStatus('sensor-status-esp32', 'ESP32', online ? 'ONLINE' : 'OFFLINE');
      setSensorStatus('sensor-status-wifi', 'Wi-Fi', online ? 'ONLINE' : 'OFFLINE');
    } catch (error) {
      console.error('Adafruit IO read failed:', error);
      setStatus(false, 'Adafruit IO read error');
    }
  }

  function init() {
    const mode = $('connection-mode-select');
    if (mode) {
      mode.value = 'adafruit';
      mode.addEventListener('change', () => {
        if (mode.value === 'adafruit') {
          pollAdafruitIO();
        } else {
          setStatus(false, 'Select Adafruit IO Public Feeds for live readings');
        }
      });
    }

    const clock = $('header-current-clock');
    const tick = () => {
      if (clock) clock.textContent = new Date().toLocaleTimeString();
    };
    tick();
    clockTimer = setInterval(tick, 1000);

    setStatus(false, 'Connecting to Adafruit IO…');
    pollAdafruitIO();
    pollTimer = setInterval(pollAdafruitIO, AIO.pollMs);
  }

  window.toggleTheme = () => document.documentElement.classList.toggle('light-mode');
  window.closeSettingsModal = () => {
    const modal = $('settings-modal');
    if (modal) modal.classList.add('hidden');
  };

  document.addEventListener('DOMContentLoaded', init);
  window.addEventListener('beforeunload', () => {
    if (pollTimer) clearInterval(pollTimer);
    if (clockTimer) clearInterval(clockTimer);
  });
})();