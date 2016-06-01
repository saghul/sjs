const system = require('system');
print('Skookum JS ' + system.versions.sjs + ' running on ' + system.platform + ' ' + system.arch);
print('[Engine: Duktape ' + system.versions.duktape + ' (' + system.versions.duktapeCommit + ')]');
print('[Build: ' + system.build.type + ' on ' + system.build.timestamp + ']');
print('[' + system.build.compiler + ' ' + system.build.compilerVersion + ' on ' + system.build.system + ']');
