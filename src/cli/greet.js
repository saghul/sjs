console.log('Skookum JS ' + system.versions.sjs + ' running on ' + system.platform + ' ' + system.arch);
if (system.build.type === 'Debug') {
    console.log('[Engine: Duktape ' + system.versions.duktape + ' (' + system.versions.duktapeCommit + ')]');
    console.log('[Build: ' + system.build.type + ' on ' + system.build.timestamp + ']');
    console.log('[' + system.build.compiler + ' ' + system.build.compilerVersion + ' on ' + system.build.system + ']');
}
