'use strict';

function parse(msg) {
    var s = 0;
    while(msg.charAt(s) != '$' && s < msg.length) {
        s++;
    }
    if (s >= msg.length - 7) {
        return null;
    }
    let interm = msg.substring(s);

    if (interm.substring(0, 3) != '$$$') {
        return null;
    }
    let type = interm[3];
    let sender = interm[4];

    var e = 3;
    while(interm.charAt(e) != '$' && e < interm.length) {
        e++;
    }

    if (e > interm.length - 3 || interm.substring(e, e+3) != '$$$') {
        return null;
    }

    let payload = interm.substring(5, e);


    let obj = {
        type,
        sender,
        senttime: new Date().getTime(),
        text: '',
        temp: 0,
        alt: 0
    };
    // HUMAN TEXT
    if (type == 1) {
        obj.text = payload;
    }
    // SOS
    else if (type == 2) {
        // pass
    }
    // SENSOR
    else if (type == 3) {
        let parts = payload.split('#');
        if (parts.length == 2) {
            obj.temp = parseFloat(parts[0])
            obj.alt = parseInt(parts[1])
        }
        else {
            return null;
        }
    }
    // ONLINE
    else if (TYPE == 4) {
        // pass
    }
    return obj;
}

export default function(body) {
    return body.trim().split('|').map(parse).filter(m => m != null);
}