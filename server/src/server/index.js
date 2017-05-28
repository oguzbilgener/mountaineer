'use strict';

import * as path from 'path';
const express = require('express');
const bodyParser = require('body-parser');
const app = express();
import ReactDOMServer from 'react-dom/server';
import React from 'react';
import Wrapper from './wrapper';
import Summary from '../client/Summary';
import device from  './services/device';
import summaryService from  './services/summary';

import dbConfig from './dbConfig';
import {serverError, ok} from './response';

const pg = require('pg');

const rawBodyParser = bodyParser.raw({type: '*/*'});
const pool = new pg.Pool(dbConfig);

app.use(function(req, res, next) {
    var data = '';
    req.on('data', function(chunk) {
        data += chunk;
    });
    req.on('end', function() {
        req.rawBody = data;
    });
    next();
});

app.get('/', function (req, res) {
    // TODO: load stuff from db
    summaryService(pool, req, res).then((data) => {
        res.end(ReactDOMServer.renderToStaticMarkup(
            React.createElement(Wrapper, {
                content: ReactDOMServer.renderToString(React.createElement(Summary, {data})),
                data
            })
        ));
    }).catch((err) => {
        serverError(res, 'html', err.toString());
    });
});

function sendNode(req, res, body) {
    let responseType = req.query.type == 'json' ? json : 'text';
    if (responseType == 'json') {
        res.set('Content-Type', 'text/json');
    }
    else {
        res.set('Content-Type', 'text/plain');
    }

    device(pool, body, req, res).then((result) => {
        console.log("ok! ", result);
        ok(res, responseType, "OK "+result);
    }).catch((err) => {
        serverError(res, responseType, err.toString());
    });
}

app.post('/node', rawBodyParser, function (req, res) {
    sendNode(req, res, req.rawBody);
});

app.get('/node', rawBodyParser, function (req, res) {
    sendNode(req, res, req.query.data);
});


app.use('/mountaineer', express.static(path.resolve(__dirname + '/../../dist')));
app.use('/assets', express.static(path.resolve(__dirname + '/../../assets')));

let port = 9009;
app.listen(port, function () {
    console.log(`mountaineer server started at port ${port}.`);
});

pool.on('error', function (err, client) {
  console.error('idle client error', err.message, err.stack);
});