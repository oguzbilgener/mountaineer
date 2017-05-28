'use strict';

import parser from './parser';

export default function(pool, req, res) {
    return pool.connect().then((client) =>
        Promise.all(
            parser(req.rawBody).map((obj) =>
                client.query('INSERT INTO logs (type, sender, senttime, text, temp, alt) '+
                    'VALUES ($1, $2, $3, $4, $5, $6)',
                    [obj.type, obj.sender, obj.senttime, obj.text, obj.temp, obj.alt])
            )
        ).then((r) => r.length)
        .catch((e) => {
            throw e;
        })
    );
}