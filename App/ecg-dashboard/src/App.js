import React, { useState, useEffect } from 'react';
import { Line } from 'react-chartjs-2';
import { Chart as ChartJS } from 'chart.js/auto';

// use chart.js

const RealTimeECG = () => {
  const [ecgData, setEcgData] = useState([]);
  const [ws, setWs] = useState(null);

  useEffect(() => {
    const websocket = new WebSocket('ws://127.0.0.1:8080');  // WebSocket server connection

    websocket.onopen = () => {
      console.log('WebSocket is connected');
    };

    websocket.onmessage = (event) => {
      const data = JSON.parse(event.data);
      setEcgData((prevData) => {
        const updatedData = [...prevData, data.ecg_value];
        if (updatedData.length > 100) {
          updatedData.shift();  // Limit the number of points to 100
        }
        return updatedData;
      });
    };

    websocket.onclose = () => {
      console.log('WebSocket is closed');
    };

    setWs(websocket);

    return () => {
      websocket.close();
    };
  }, []);

  // Chart.js data
  const chartData = {
    labels: Array(ecgData.length).fill(''),  // Empty labels for the x-axis
    datasets: [
      {
        label: 'ECG Data',
        data: ecgData,
        borderColor: 'rgba(75,192,192,1)',
        borderWidth: 1,
        fill: false,
      },
    ],
  };

  return (
    <div>
      <h1>Real-Time ECG Monitor</h1>
      <Line data={chartData} />
    </div>
  );
};

export default RealTimeECG;
