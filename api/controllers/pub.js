import { createPowersPoint } from "../services/influxdb.js";
import {PrismaClient} from '@prisma/client';

const prisma = new PrismaClient();

const addMeasurements = (measurements) => {
    const startTime = Date.parse(measurements.metadata.timestamp_start);
    const consumers = measurements.prosumers;
    console.log(JSON.stringify(consumers));
    for(let i=0; i<consumers.length; i++){
        prisma.energy.findFirst({
            where: {
                id: consumers[i].id
            }
        })
        .then(async (energyMeter) => {
            for(let j=0; j<consumers[i].payload.length; j++){
                consumers[i].payload[j].timestamp = new Date(startTime + consumers[i].payload[j].timestamp *1000);
                energyMeter.timestamp = consumers[i].payload[j].timestamp;
                energyMeter.netEnergy += consumers[i].payload[j].E;
                energyMeter.exportEnergy += consumers[i].payload[j].Eexp;
                energyMeter.importEnergy += consumers[i].payload[j].Eimp;
                energyMeter.apparentEnergy += consumers[i].payload[j].Ea;
                consumers[i].payload[j].E = energyMeter.netEnergy;
                consumers[i].payload[j].Eexp = energyMeter.exportEnergy;
                consumers[i].payload[j].Eimp = energyMeter.importEnergy;
                consumers[i].payload[j].Ea = energyMeter.apparentEnergy;
                createPowersPoint(consumers[i].payload[j]);
            }
            await prisma.energy.update({
                where:{
                    id: consumers[i].id,
                },
                data: energyMeter,
            });
        });
    }
}
const addPowerMeasurements = (measurements) => {
    const startTime = Date.parse(measurements.metadata.timestamp_start);
    const consumers = measurements.prosumers;
    console.log(JSON.stringify(consumers));
    for(let i=0; i<consumers.length; i++){
        for(let j=0; j<consumers[i].payload.length; j++){
            consumers[i].payload[j].timestamp = new Date(startTime + consumers[i].payload[j].timestamp *1000);
            createPowersPoint(consumers[i].payload[j]);
        }
    }
}
export {addMeasurements};