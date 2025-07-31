import { INFLUXDB_URL, INFLUXDB_TOKEN, INFLUXDB_ORG, INFLUXDB_BUCKET } from "../config/index.js";
import { InfluxDB, Point, HttpError } from '@influxdata/influxdb-client';

const influxClientWriteAPI = new InfluxDB({url:INFLUXDB_URL, token: INFLUXDB_TOKEN}).getWriteApi(INFLUXDB_ORG, INFLUXDB_BUCKET, 'ns');

let createPowersPoint = async function(data_point){
    try{
        let point = new Point('powers')
            .tag('consumer', data_point.id)
            .floatField('Pload', data_point.Pload)
            .floatField('Qload', data_point.Qload)
            .floatField('Ppv', data_point.Ppv)
            .floatField('Qpv', data_point.Qpv)
            .floatField('Pgrid', data_point.Pgrid)
            .floatField('Qgrid', data_point.Qgrid)
            .floatField('S', data_point.S)
            .floatField('E', data_point.E)
            .floatField('Eexp',data_point.Eexp)
            .floatField('Eimp', data_point.Eimp)
            .floatField('Ea', data_point.Ea)
            .floatField('pf', data_point.pf)
            .timestamp(new Date(data_point.timestamp));
        influxClientWriteAPI.writePoint(point);
        //console.log(` ${point.toLineProtocol(influxClientWriteAPI)}`);
    }catch(err){
        console.error(err);
    }
}
export {createPowersPoint};
