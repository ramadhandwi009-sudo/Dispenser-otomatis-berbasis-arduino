const int trigPin = 10;
const int echoPin = 11;
const int relayPin = 7;

const int distanceThreshold = 10; // cm, jarak tangan aktif
const int minDistance = 2;        // abaikan noise
const int readings = 5;           // jumlah pembacaan untuk rata-rata
const int offDelay = 1000;        // jeda 1 detik sebelum pompa mati

long distanceArray[readings];
unsigned long lastDetectedTime = 0; // untuk menunda mati
bool pumpOn = false;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Pompa OFF (relay aktif LOW)
  Serial.begin(9600);

  // Inisialisasi array jarak
  for (int i = 0; i < readings; i++) distanceArray[i] = distanceThreshold + 1;
}

long getDistance() {
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 25000); // timeout 25ms (max 4m)
  if (duration == 0) return 999;            // jika gagal baca
  distance = duration * 0.034 / 2;          // ubah ke cm
  return distance;
}

long getAverageDistance() {
  long sum = 0;
  for (int i = 0; i < readings; i++) {
    long d = getDistance();
    if (d > 0 && d < 400) { // hanya nilai valid
      distanceArray[i] = d;
      sum += d;
    } else {
      sum += distanceThreshold + 5; // abaikan nilai aneh
    }
    delay(20);
  }
  return sum / readings;
}

void loop() {
  long distance = getAverageDistance();
  Serial.print("Average Distance: ");
  Serial.println(distance);

  // Jika tangan terdeteksi
  if (distance > minDistance && distance < distanceThreshold) {
    lastDetectedTime = millis();
    if (!pumpOn) {
      pumpOn = true;
      digitalWrite(relayPin, LOW); // Pompa ON
      Serial.println("Pompa ON");
    }
  }

  // Jika sudah lewat waktu delay dan tangan tidak ada
  if (pumpOn && millis() - lastDetectedTime > offDelay) {
    pumpOn = false;
    digitalWrite(relayPin, HIGH); // Pompa OFF
    Serial.println("Pompa OFF");
  }

  delay(50);
}
