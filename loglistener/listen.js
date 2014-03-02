
var net = require('net');

var s = net.createServer(function (sock) {
    sock.setEncoding('utf8');

    var body = "";
    sock.on('data', function (data) {
        console.log(data);
        //body = body + data;

    });

    sock.on('end', function() {
        console.log('end');
        // console.log(data);
        // postData(data);
    });

    // TODO error handling here
});

var port = 4444;
var host = "127.0.0.1";
s.listen(port, host, function () {
    console.log('Server listening on ' + host + ':' + s.address().port);
});