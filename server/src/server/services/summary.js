'use strict';
import moment from 'moment';

function mapAllData(rows) {
    return Promise.resolve().then(() => {
        return rows.rows;
    });
}

function mapSensorDataForCharts(rows) {
    let data = {
        leader: {temp: [], alt: []},
        member: {temp: [], alt: []},
    }

    let lim = 12;
    let start = Math.max(rows.length - 12, 0)
    rows.slice(start, start + lim).forEach((row) => {
        let u = row.sender == 3 ? 'member' : 'leader';
        let key = moment(parseInt(row.senttime)).format('mm');
        data[u]['temp'].push({'time': key, 'val': row.temp});
        data[u]['alt'].push({'time': key, 'val': row.alt});
    });

    return data;
}

export default function(pool, req, res) {
    return pool.connect().then((client) => {

        let pAllSensorData = client.query('SELECT * FROM logs WHERE type=3').then(mapAllData);
        let pAllSosData = client.query('SELECT * FROM logs WHERE type=2').then(mapAllData);

        return Promise.all([pAllSensorData, pAllSosData])
        .then(([allSensorData, allSosData]) => {
            let data = {};
            data.allSensorData = allSensorData;
            data.allSosData = allSosData;
            data.sensorChartData = mapSensorDataForCharts(allSensorData);
            return data;
        })

    });
}