
const _io = require('_io');


function StdioStream(fd) {
    this._fd = fd;

    Object.defineProperty(this, 'fd', {
        get: function() {
            return this._fd;
        }
    });
}


StdioStream.prototype.read = function(nread) {
    var nread = (nread >>> 0) || 4096;
    return _io.read(this._fd, nread);
}


StdioStream.prototype.write = function(data) {
    return _io.write(this._fd, data);
}


// TODO: fix, this is not always the case
const stdin = new StdioStream(_io.c.STDIN_FILENO);
const stdout = new StdioStream(_io.c.STDOUT_FILENO);
const stderr = new StdioStream(_io.c.STDERR_FILENO);


module.exports = {
    stdin: stdin,
    stdout: stdout,
    stderr: stderr
};
