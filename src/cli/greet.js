const system = require('system');
console.log('Skookum JS ' + system.versions.sjs + ' running on ' + system.platform + ' ' + system.arch);
console.log('[Engine: Duktape ' + system.versions.duktape + ' (' + system.versions.duktapeCommit + ')]');
console.log('[Build: ' + system.build.type + ' on ' + system.build.timestamp + ']');
console.log('[' + system.build.compiler + ' ' + system.build.compilerVersion + ' on ' + system.build.system + ']');
