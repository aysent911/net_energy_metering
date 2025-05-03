import { createPowersPoint } from "../services/influxdb.js";

const addPowerMeasurements = (req, res, next) => {
    const startTime = Date.parse(req.body.metadata.timestamp_start);
    const consumers = req.body.prosumers;
    for(let i=0; i<consumers.length; i++){
        for(let j=0; j<consumers[i].payload.length; j++){
            consumers[i].payload[j].timestamp = new Date(startTime + consumers[i].payload[j].timestamp *1000);
            createPowersPoint(consumers[i].payload[j]);
        }
    }
    next();
}
export {addPowerMeasurements};