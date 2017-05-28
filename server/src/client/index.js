"use strict";

import * as es6Promise from 'es6-promise';
import React           from 'react';
import ReactDOM        from 'react-dom';

import Summary         from './Summary';

const appData = window.appData;

ReactDOM.render(
    React.createElement(Summary, {data: appData}), document.getElementById('app')
);