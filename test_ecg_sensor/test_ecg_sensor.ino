// Define the pin connected to the AD8232 OUTPUT
#define ECG_PIN 34  // Change this to match your wiring (use an ADC pin)
#define SAMPLE_RATE 500  // Sampling rate in Hz

#include <arduinoFFT.h>  // Ensure the correct library is installed

const int numSamples = 256;  // FFT requires power-of-2 samples
int ecgBuffer[numSamples];   // Circular buffer for ECG signal
int bufferIndex = 0;

float vReal[numSamples];
float vImag[numSamples];
ArduinoFFT<float> FFT = ArduinoFFT<float>(vReal, vImag, numSamples, SAMPLE_RATE); // Updated constructor format

// FIR filter coefficients for baseline drift removal
const float firCoeffs[5] = {0.2, 0.2, 0.2, 0.2, 0.2};
float firBuffer[5] = {0};  // FIR filter buffer
int firIndex = 0;

void setup() {
    Serial.begin(115200); // Start Serial communication at 115200 baud
    pinMode(ECG_PIN, INPUT); // Set the ECG pin as input
}

void loop() {
    // Read ECG signal from AD8232
    int ecgValue = analogRead(ECG_PIN);
    
    // Apply FIR filter for baseline drift removal
    firBuffer[firIndex] = ecgValue;
    firIndex = (firIndex + 1) % 5;
    float filteredValue = 0;
    for (int i = 0; i < 5; i++) {
        filteredValue += firCoeffs[i] * firBuffer[i];
    }
    
    // Store data in buffer for FFT processing
    ecgBuffer[bufferIndex] = filteredValue;
    vReal[bufferIndex] = filteredValue;
    vImag[bufferIndex] = 0;
    bufferIndex = (bufferIndex + 1) % numSamples;
    
    // Compute FFT every numSamples readings
    if (bufferIndex == 0) {
        FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward, false);
        FFT.compute(FFTDirection::Forward);
        FFT.complexToMagnitude();
        
        // Find peak frequency component
        float peakFrequency = 0;
        float maxMagnitude = 0;
        for (int i = 1; i < numSamples / 2; i++) {
            if (vReal[i] > maxMagnitude) {
                maxMagnitude = vReal[i];
                peakFrequency = (i * SAMPLE_RATE) / numSamples;
            }
        }
        Serial.print("Peak Frequency: ");
        Serial.println(peakFrequency);
    }
    
    // Peak detection (simple thresholding)
    static int lastPeak = 0;
    if (filteredValue > 800 && lastPeak == 0) { // Adjust threshold as needed
        Serial.println("R-Peak Detected");
        lastPeak = 1;
    }
    if (filteredValue < 600) {
        lastPeak = 0;
    }
    
    Serial.println(filteredValue); // Print processed ECG data
    delay(50); // Ensure consistent sampling rate
}