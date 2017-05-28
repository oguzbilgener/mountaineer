"use strict";

import * as es6Promise from 'es6-promise';
import React           from 'react';
import ReactDOM        from 'react-dom';

import {Provider}      from 'react-redux';


ReactDOM.render(
    React.createElement(Provider, {},
        React.createElement(Layout)
    ),
    document.getElementById('app')
);