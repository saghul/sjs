
import errno

for i in sorted(errno.errorcode.keys()):
    err = errno.errorcode[i]
    print('#ifdef %s' % err)
    print('    { "%s", %s },' % (err, err))
    print('#endif')

