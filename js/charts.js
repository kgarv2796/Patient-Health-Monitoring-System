/* Chart.js helpers for the existing dashboard canvases. */
(function () {
  const history = { labels: [], hr: [], spo2: [], temp: [] };
  const charts = {};
  function makeChart(id, label, color, values, min, max) {
    const canvas = document.getElementById(id);
    if (!canvas || !window.Chart) return null;
    return new Chart(canvas, {
      type: 'line',
      data: { labels: history.labels, datasets: [{ label, data: values, borderColor: color, backgroundColor: color + '22', fill: true, tension: .32, pointRadius: 0, borderWidth: 2 }] },
      options: { responsive: true, maintainAspectRatio: false, animation: false,
        plugins: { legend: { labels: { color: '#cbd5e1' } } },
        scales: { x: { ticks: { color: '#94a3b8', maxTicksLimit: 6 }, grid: { color: '#243247' } }, y: { min, max, ticks: { color: '#94a3b8' }, grid: { color: '#243247' } } } }
    });
  }
  function init() {
    if (!window.Chart) return;
    charts.hr = makeChart('chart-hr', 'Heart rate (BPM)', '#22d3ee', history.hr, 30, 180);
    charts.spo2 = makeChart('chart-spo2', 'SpO₂ (%)', '#34d399', history.spo2, 70, 100);
    charts.temp = makeChart('chart-temp', 'Temperature (°C)', '#fbbf24', history.temp, 25, 42);
  }
  window.addHealthChartPoint = function (v) {
    const label = new Date().toLocaleTimeString();
    history.labels.push(label); history.hr.push(Number.isFinite(v.heartRate) ? v.heartRate : null);
    history.spo2.push(Number.isFinite(v.spo2) ? v.spo2 : null); history.temp.push(Number.isFinite(v.temperature) ? v.temperature : null);
    if (history.labels.length > 60) Object.keys(history).forEach(k => history[k].shift());
    Object.values(charts).forEach(c => { if (c) c.update('none'); });
  };
  if (document.readyState === 'loading') document.addEventListener('DOMContentLoaded', init); else init();
})();
