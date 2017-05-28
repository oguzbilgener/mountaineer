'use strict';

import React from 'react';

class Wrapper extends React.Component {

    render() {
        let dataStr = "window.appData = " + JSON.stringify(this.props.data) + ";";

        return (
            <html>
            <head>
                <meta charSet="utf-8" />
                <title>Mountaineer Server</title>
                <meta name="viewport" content="width=device-width, user-scalable=no" />
                <link href='https://fonts.googleapis.com/css?family=Open+Sans:400,300,600,700' rel='stylesheet' type='text/css' />
                <link href="/assets/css/bootstrap.min.css" rel="stylesheet" />
                <link href="/assets/css/nifty.min.css" rel="stylesheet" />
                <link href="/assets/css/morris.min.css" rel="stylesheet" />


            </head>
            <body id="mountaineer">
                <div id="container" className="effect aside-float aside-bright mainnav-out">
                    <header id="navbar">
                        <div id="navbar-container" className="boxed">
                            <div className="navbar-header">
                                <a href="/" className="navbar-brand">
                                    <div className="brand-title">
                                        <span className="brand-text">Mountaineer</span>
                                    </div>
                                </a>
                            </div>
                            <div className="navbar-content clearfix">
                                <ul className="nav navbar-top-links pull-right">
                                    <li id="dropdown-user" className="dropdown">
                                        <a href="#" data-toggle="dropdown" className="dropdown-toggle text-right">
                                            <span className="pull-right">
                                                <i className="demo-pli-male ic-user"></i>
                                            </span>
                                            <div className="username hidden-xs">Test User</div>
                                        </a>
                                    </li>
                                </ul>
                            </div>
                        </div>
                    </header>
                    <div className="boxed">
                        <div id="content-container">
                            <div id="app" dangerouslySetInnerHTML={{__html: this.props.content}}/>
                        </div>
                    </div>
                    <footer id="footer">
                        <div className="show-fixed pull-right">
                            You have <a href="#" className="text-bold text-main"><span className="label label-danger">3</span> pending action.</a>
                        </div>


                        <p className="pad-lft">&#0169; 2017 Mountaineer</p>



                    </footer>

                    <button className="scroll-top btn">
                        <i className="pci-chevron chevron-up"></i>
                    </button>

                </div>
                <script src="/assets/js/jquery.min.js" />
                <script src="/assets/js/bootstrap.min.js" />
                <script src="/assets/js/nifty.min.js" />
                <script src="/assets/js/jquery.sparkline.min.js" />
                <script src="/assets/js/morris.min.js" />
                <script src="/assets/js/raphael.min.js" />
                <script src="/mountaineer/mountaineer.js" defer/>
                <script dangerouslySetInnerHTML={{__html: dataStr }} />
            </body>
            </html>
        );
    }
};

export default Wrapper;
