'use strict';

import React from 'react';

class Wrapper extends React.Component {

    render() {

        return (
            <html>
            <head>
                <meta charSet="utf-8" />
                <title>Mountaineer Server</title>
                <meta name="viewport" content="width=device-width, user-scalable=no" />

            </head>
            <body id="mountaineer">
                <div id="app" dangerouslySetInnerHTML={{__html: this.props.content}}/>
                <script src="/mountaineer/mountaineer.js" defer/>
            </body>
            </html>
        );
    }
};

export default Wrapper;
