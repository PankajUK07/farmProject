import { initializeApp }
    from "https://www.gstatic.com/firebasejs/10.12.2/firebase-app.js";

import {

    getDatabase,
    ref,
    onValue,
    set

} from
    "https://www.gstatic.com/firebasejs/10.12.2/firebase-database.js";

// ================= FIREBASE =================

const firebaseConfig = {

    apiKey: "AIzaSyAibfhocPq34T88lX9B1xMumLGlLpYxORE",

    databaseURL:
        "https://farm-project-d8451-default-rtdb.asia-southeast1.firebasedatabase.app/"
};

const app = initializeApp(firebaseConfig);

const db = getDatabase(app);

// ================= CHART DATA =================

let tempLabels = [];

let tempValues = [];

let gasLabels = [];

let gasValues = [];

let humidityLabels = [];

let humidityValues = [];

let soilLabels = [];

let soilValues = [];

// ================= TEMPERATURE CHART =================

const tempCtx =
    document.getElementById('tempChart');

const tempChart =
    new Chart(tempCtx, {

        type: 'line',

        data: {

            labels: tempLabels,

            datasets: [{

                label: 'Temperature',

                data: tempValues
            }]
        }
    });

// ================= GAS CHART =================

const gasCtx =
    document.getElementById('gasChart');

const gasChart =
    new Chart(gasCtx, {

        type: 'line',

        data: {

            labels: gasLabels,

            datasets: [{

                label: 'Gas',

                data: gasValues
            }]
        }
    });

// ================= HUMIDITY CHART =================

const humidityCtx =
    document.getElementById('humidityChart');

const humidityChart =
    new Chart(humidityCtx, {

        type: 'line',

        data: {

            labels: humidityLabels,

            datasets: [{

                label: 'Humidity',

                data: humidityValues
            }]
        }
    });
// ================= SOIL MOISTURE CHART =================

const soilCtx =
    document.getElementById('soilChart');

const soilChart =
    new Chart(soilCtx, {

        type: 'line',

        data: {

            labels: soilLabels,

            datasets: [{

                label: 'Soil Moisture',

                data: soilValues
            }]
        }
    });
// ================= FIREBASE LIVE =================

const deviceRef = ref(db, 'device1');

onValue(deviceRef, (snapshot) => {

    const data = snapshot.val();

    // ================= SENSOR VALUES =================

    document.getElementById("temp").innerHTML =
        data.temperature + " °C";

    document.getElementById("humidity").innerHTML =
        data.humidity + " %";

    document.getElementById("gas").innerHTML =
        data.gas;

    document.getElementById("fanState").innerHTML =
        data.fanState ? "ON" : "OFF";

    document.getElementById("soil").innerHTML =
        data.soil;

    // ================= MODE =================

    if (data.mode) {

        document.getElementById("modeText").innerHTML =
            data.mode;
    }

    // ================= GAS ALERT =================

    if (data.gas > 1200) {
        document.body.classList.add('alert-high-gas');
    }
    else {
        document.body.classList.remove('alert-high-gas');
    }

    // ================= LAST SEEN (optional UI) =================
    if (data.lastSeen) {
        const last = new Date(data.lastSeen).toLocaleTimeString();
        const el = document.getElementById('lastSeenText');
        if (el) el.textContent = last;
    }

    // ================= OFFLINE DETECTION =================

    const now = Date.now();


    // ================= DEVICE STATUS PILL =================
    const statusEl = document.getElementById('deviceStatusPill');

    if (statusEl) {
        if (!data.lastSeen) {
            statusEl.textContent = 'UNKNOWN';
        } else {
            const diff = now - data.lastSeen;
            statusEl.textContent = diff > 30000 ? 'OFFLINE' : 'ONLINE';
        }
    }


    if (data.lastSeen) {

        const diff = now - data.lastSeen;

        if (diff > 30000) {

            const legacyStatus = document.getElementById("deviceStatus");
            if (legacyStatus) legacyStatus.innerHTML = "OFFLINE";
        }

        else {

            const legacyStatus = document.getElementById("deviceStatus");
            if (legacyStatus) legacyStatus.innerHTML = "ONLINE";
        }
    }





    // ================= CHART UPDATE =================

    const time =
        new Date().toLocaleTimeString();

    tempLabels.push(time);

    tempValues.push(data.temperature);

    gasLabels.push(time);

    gasValues.push(data.gas);

    humidityLabels.push(time);

    humidityValues.push(data.humidity);

    soilLabels.push(time);

    soilValues.push(data.soil);

    // Limit chart points

    if (tempLabels.length > 10) {

        tempLabels.shift();

        tempValues.shift();

        gasLabels.shift();

        gasValues.shift();

        humidityLabels.shift();

        humidityValues.shift();

        soilLabels.shift();

        soilValues.shift();
    }

    tempChart.update();

    gasChart.update();

    humidityChart.update();

    soilChart.update();
});

// ================= MODE CONTROL =================

window.setAuto = function () {

    set(ref(db, 'device1/mode'),
        "AUTO");
}

window.setManual = function () {

    set(ref(db, 'device1/mode'),
        "MANUAL");
}

// ================= FAN CONTROL =================

window.fanON = function () {

    set(ref(db, 'device1/fan'),
        1);
}

window.fanOFF = function () {

    set(ref(db, 'device1/fan'),
        0);
}

if ('serviceWorker' in navigator) {

  // Use absolute path to avoid base-tag/subpath issues when hosted on ESP32/any server
  navigator.serviceWorker
  .register('/service-worker.js')

  .then(() => {

    console.log("Service Worker Registered");
  });
}