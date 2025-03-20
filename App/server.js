const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 8080 });

wss.on('connection', (ws) => {
  console.log('Client connected');

  ws.on('message', (message) => {
    try {
      // Parse the incoming message as JSON
      const data = JSON.parse(message);
      console.log(`Received ECG data: ${data.ecg_value}`);
      
      // Broadcast the message to all connected clients
      wss.clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
          client.send(JSON.stringify({ ecg_value: data.ecg_value }));
        }
      });
    } catch (error) {
      console.error('Error parsing message:', error);
    }
  });

  ws.on('close', () => {
    console.log('Client disconnected');
  });

  ws.on('error', (error) => {
    console.log('WebSocket error:', error);
  });
});

console.log('WebSocket server is running on ws://localhost:8080');
