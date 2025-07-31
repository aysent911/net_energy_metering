import * as mqtt from 'mqtt';
import { MQTT_HOST, MQTT_USERNAME, MQTT_PASSWORD } from '../config/index.js';
import { addMeasurements } from '../controllers/pub.js';

let client = mqtt.connect(MQTT_HOST, {
    clean: true,
    username: MQTT_USERNAME,
    password: MQTT_PASSWORD,
});
client.on('error', (error) => {
    console.error(error);
});
client.on('connect', ( ) => {
    console.log(`connected to ${MQTT_USERNAME}@${MQTT_HOST}`);
});
client.on('message', (topic, payload, packet) => {
    console.log(`topic: ${topic}`);
    console.log(`payload: ${payload.toString()}`);
    switch(topic){
        case 'meter_data':
            addMeasurements(JSON.parse(payload));
            break;
    }
});

export default client;