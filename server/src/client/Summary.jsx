'use strict';

import React from 'react';

class TempChart extends React.Component {

    componentDidMount() {
        var barEl = $("#demo-sparkline-bar");
        var barValues = [40,32,65,53,62,55,24,67,45,70,45,56,34,67,76,32,65,53,62,55,24,67,45,70,45,56,70,45,56,34,67,76,32,65,53,62,55];
        var barValueCount = barValues.length;
        var barSpacing = 1;
        var salesSparkline = function(){
             barEl.sparkline(barValues, {
                type: 'bar',
                height: 40,
                barWidth: Math.round((barEl.parent().width() - ( barValueCount - 1 ) * barSpacing ) / barValueCount),
                barSpacing: barSpacing,
                zeroAxis: false,
                tooltipChartTitle: 'Daily Sales',
                tooltipSuffix: ' Sales',
                barColor: 'rgba(255,255,255,.7)'
            });
        }
    }

    render() {
        return <div className="col-sm-6">
            <div className="panel panel-info panel-colorful">
                <div className="pad-all">
                    <p className="text-lg text-semibold">Line Chart</p>
                </div>
                <div className="pad-all text-center">
                    <div id="demo-sparkline-line"></div>
                </div>
            </div>
        </div>
    }
}

class AltChart extends React.Component {

    render() {
        return <div className="col-sm-6">

        </div>
    }
}


class Summary extends React.Component {

    renderAllSensorData() {
        return <table className="table table-striped">
            <thead>
                <tr>
                    <th>Node</th>
                    <th>Time</th>
                    <th>Temperature</th>
                    <th>Altitude</th>
                </tr>
            </thead>
            <tbody>
            {
                this.props.data.allSensorData.map((d, i) =>
                    <tr key={`all-sensor-${i}`}>
                        <td>{d.sender == 3 ? 'Member 1' : 'Leader'}</td>
                        <td>{new Date(parseInt(d.senttime)).toString()}</td>
                        <td>{d.temp}</td>
                        <td>{d.alt}</td>
                    </tr>
                )
            }
            </tbody>
        </table>

    }

    renderAllSosData() {
        return <table className="table table-striped">
            <thead>
                <tr>
                    <th>Node</th>
                    <th>Time</th>
                </tr>
            </thead>
            <tbody>
            {
                this.props.data.allSosData.map((d, i) =>
                    <tr key={`all-sos-${i}`}>
                        <td>{d.sender == 3 ? 'Member 1' : 'Leader'}</td>
                        <td>{new Date(parseInt(d.senttime)).toString()}</td>
                    </tr>
                )
            }
            </tbody>
        </table>
    }

    renderSummary() {
        return <div className="row">
            <div className="col-sm-4">
                <div className="panel panel-colorful panel-danger">
                    <div className="panel-heading">
                        <h3 className="panel-title">3 SOS messages from Node 1</h3>
                    </div>
                    <div className="panel-body">
                        <p>Last one received at 14:59.</p>
                    </div>
                </div>

            </div>
        </div>
    }

    render() {

        let data = this.props.data;

        return (<div>
            <div id="page-title">
                <h1 className="page-header text-overflow">Reports</h1>

                <div id="page-content">

                    <h3>Summary</h3>
                    {this.renderSummary()}

                    <div className="row">
                        <TempChart data={data.sensorChartData} />
                    </div>

                    <h3>All Data</h3>
                    <div className="row">
                        <div className="col-sm-6">
                            <div className="panel">
                                <div className="panel-heading">
                                    <h5 className="panel-title">SOS</h5>
                                </div>
                                <div className="panel-body">
                                    {this.renderAllSosData()}
                                </div>
                            </div>
                        </div>

                        <div className="col-sm-6">
                            <div className="panel">
                                <div className="panel-heading">
                                    <h5 className="panel-title">Sensor</h5>
                                </div>
                                <div className="panel-body">
                                    {this.renderAllSensorData()}
                                </div>
                            </div>
                    </div>

                </div>
            </div>
        </div>
        </div>);
    }
}

export default Summary;