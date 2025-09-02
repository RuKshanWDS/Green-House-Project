<h1>🌱 Smart Agriculture Monitoring System</h1>

<p>
An Arduino-based project to monitor <b>soil moisture</b> and <b>temperature</b>, and automatically control a 
<b>water pump</b> and <b>fan</b>. The system also includes an <b>ultrasonic sensor</b> for distance measurement and 
an <b>I2C LCD display</b> to show sensor readings and device status.
</p>

<hr>

<h2>📌 Features</h2>
<ul>
  <li>✅ Soil moisture monitoring</li>
  <li>✅ Temperature sensing</li>
  <li>✅ Automatic control of water pump & fan</li>
  <li>✅ Ultrasonic sensor for distance measurement</li>
  <li>✅ Real-time display on 16x2 I2C LCD</li>
  <li>✅ Buzzer alerts for warnings</li>
</ul>

<hr>

<h2>🔧 Components Used</h2>
<ul>
  <li>Arduino Uno / Nano</li>
  <li>Soil Moisture Sensor 🌱</li>
  <li>DHT11 / DHT22 Temperature Sensor 🌡️</li>
  <li>Ultrasonic Sensor (HC-SR04) 📏</li>
  <li>Water Pump + Relay 💧</li>
  <li>Cooling Fan + Relay 🌬️</li>
  <li>Buzzer 🔔</li>
  <li>16x2 I2C LCD 🖥️</li>
  <li>Jumper wires + Breadboard</li>
</ul>

<hr>

<h2>⚡ Circuit Diagram</h2>

<p align="center">
  <img src="C:\Users\RUKSHAN\Pictures\Camera Roll\greenHouseCircuit.png" alt="Circuit Diagram" width="600">
</p>

<hr>

<h2>🚀 How It Works</h2>
<ol>
  <li>Arduino reads <b>soil moisture</b> and <b>temperature</b>.</li>
  <li>If soil is dry → Pump turns ON 💧</li>
  <li>If temperature is high → Fan turns ON 🌬️</li>
  <li>Ultrasonic sensor measures distance and displays on LCD 📏</li>
  <li>LCD shows real-time values + motor status.</li>
  <li>Buzzer alerts if critical conditions are detected.</li>
</ol>

<hr>

<h2>▶️ Getting Started</h2>

<h3>🔹 Prerequisites</h3>
<ul>
  <li>Install <b>Arduino IDE</b></li>
  <li>Add required libraries:
    <ul>
      <li><code>LiquidCrystal_I2C.h</code></li>
      <li><code>DHT.h</code></li>
    </ul>
  </li>
</ul>

<h3>🔹 Uploading the Code</h3>
<ol>
  <li>Open <code>Full_code_01.ino</code> in Arduino IDE.</li>
  <li>Select correct board & COM port.</li>
  <li>Upload the sketch.</li>
</ol>

<hr>

<h2>📊 Output Example</h2>
<pre>
Soil: Wet 🌱  
Temp: 30°C 🌡️  
Fan: ON 🌬️  
Pump: OFF 💧  
Distance: 25 cm 📏  
</pre>

<hr>

<h2>🌟 Future Improvements</h2>
<ul>
  <li>Add WiFi (ESP8266/ESP32) 🌐 for IoT integration.</li>
  <li>Mobile app / web dashboard 📱.</li>
  <li>Automatic nutrient monitoring for plants 🧪.</li>
</ul>

<hr>

<h2>🤝 Contributing</h2>
<p>
Pull requests are welcome! For major changes, please open an issue first to discuss what you would like to change.
</p>

<hr>

<h2>📜 License</h2>
<p>This project has no any License , because this is for my practice.</p>
