'use strict';

function mapAllData(rows) {
    return Promise.resolve().then(() => {
        return rows.rows;
    });
}

function mapSensorDataForCharts(rows) {
    let data = {
        leader: {temp: [], alt: []},
        member: {temp: [], alt: []}
    }

    rows.forEach((row) => {
        u = row.sender == 3 ? 'member' : 'leader';
        data[u]['temp'] = row.temp;
        data[u]['alt'] =  row.alt;
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