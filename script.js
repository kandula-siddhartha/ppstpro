// Simple localStorage keys
const FARM_KEY = "sf_farms";
const CROP_KEY = "sf_crops";
const SENSOR_KEY = "sf_sensors";

const farmForm = document.getElementById("farmForm");
const cropForm = document.getElementById("cropForm");
const sensorForm = document.getElementById("sensorForm");

const farmCount = document.getElementById("farmCount");
const cropCount = document.getElementById("cropCount");
const sensorCount = document.getElementById("sensorCount");
const alertCount = document.getElementById("alertCount");

const alertList = document.getElementById("alertList");
const sensorTableBody = document.getElementById("sensorTableBody");

function getData(key) {
  return JSON.parse(localStorage.getItem(key) || "[]");
}

function setData(key, data) {
  localStorage.setItem(key, JSON.stringify(data));
}

function getSensorStatus(type, value) {
  const v = Number(value);
  if (type === "Moisture") return v < 20 || v > 80;
  if (type === "Temperature") return v < 15 || v > 35;
  if (type === "Humidity") return v < 30 || v > 85;
  if (type === "WaterLevel") return v < 25 || v > 90;
  return false;
}

function getRange(type) {
  if (type === "Moisture") return "20 - 80 %";
  if (type === "Temperature") return "15 - 35 C";
  if (type === "Humidity") return "30 - 85 %";
  if (type === "WaterLevel") return "25 - 90 %";
  return "N/A";
}

function renderDashboard() {
  const farms = getData(FARM_KEY);
  const crops = getData(CROP_KEY);
  const sensors = getData(SENSOR_KEY);

  const alerts = sensors.filter((s) => getSensorStatus(s.type, s.value));

  farmCount.textContent = farms.length;
  cropCount.textContent = crops.length;
  sensorCount.textContent = sensors.length;
  alertCount.textContent = alerts.length;

  renderAlerts(alerts);
  renderSensorTable(sensors);
}

function renderAlerts(alerts) {
  alertList.innerHTML = "";

  if (!alerts.length) {
    const li = document.createElement("li");
    li.textContent = "No abnormal values. System is stable.";
    li.style.borderLeftColor = "#2f7d32";
    alertList.appendChild(li);
    return;
  }

  alerts.forEach((a) => {
    const li = document.createElement("li");
    li.textContent = `ALERT: ${a.type} sensor (${a.sensorId}) value ${a.value} is out of range (${getRange(a.type)}).`;
    alertList.appendChild(li);
  });
}

function renderSensorTable(sensors) {
  sensorTableBody.innerHTML = "";

  sensors.forEach((s) => {
    const isAbnormal = getSensorStatus(s.type, s.value);
    const tr = document.createElement("tr");

    tr.innerHTML = `
      <td>${s.sensorId}</td>
      <td>${s.farmId}</td>
      <td>${s.cropId}</td>
      <td>${s.type}</td>
      <td>${Number(s.value).toFixed(2)}</td>
      <td class="${isAbnormal ? "status-abnormal" : "status-normal"}">
        ${isAbnormal ? "ABNORMAL" : "NORMAL"}
      </td>
      <td>${s.location}</td>
      <td>${s.time}</td>
      <td><button class="delete-btn" onclick="deleteSensor('${s.sensorId}')">Delete</button></td>
    `;

    sensorTableBody.appendChild(tr);
  });
}

farmForm.addEventListener("submit", (e) => {
  e.preventDefault();

  const farms = getData(FARM_KEY);
  farms.push({
    id: document.getElementById("farmId").value.trim(),
    name: document.getElementById("farmName").value.trim(),
    location: document.getElementById("farmLocation").value.trim(),
  });

  setData(FARM_KEY, farms);
  farmForm.reset();
  renderDashboard();
});

cropForm.addEventListener("submit", (e) => {
  e.preventDefault();

  const crops = getData(CROP_KEY);
  crops.push({
    id: document.getElementById("cropId").value.trim(),
    farmId: document.getElementById("cropFarmId").value.trim(),
    cropName: document.getElementById("cropName").value.trim(),
    season: document.getElementById("cropSeason").value.trim(),
  });

  setData(CROP_KEY, crops);
  cropForm.reset();
  renderDashboard();
});

sensorForm.addEventListener("submit", (e) => {
  e.preventDefault();

  const sensors = getData(SENSOR_KEY);
  sensors.push({
    sensorId: document.getElementById("sensorId").value.trim(),
    farmId: document.getElementById("sensorFarmId").value.trim(),
    cropId: document.getElementById("sensorCropId").value.trim(),
    type: document.getElementById("sensorType").value,
    value: Number(document.getElementById("sensorValue").value),
    location: document.getElementById("sensorLocation").value.trim(),
    time: new Date().toLocaleString(),
  });

  setData(SENSOR_KEY, sensors);
  sensorForm.reset();
  renderDashboard();
});

function deleteSensor(sensorId) {
  const sensors = getData(SENSOR_KEY).filter((s) => s.sensorId !== sensorId);
  setData(SENSOR_KEY, sensors);
  renderDashboard();
}

// Initial render
renderDashboard();
