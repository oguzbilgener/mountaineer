'use strict';

export function serverError(res, type, msg) {
    res.status(500);
    if (type == 'json') {
        res.end({
            error: msg
        });
    }
    else {
        res.end(`ERROR:${msg}`);
    }
}

export function ok(res, type, msg) {
    res.status(200);
    if (type == 'json') {
        res.json({
            result: msg
        });
    }
    else {
        res.end(msg);
    }
}