
import sys

with open(sys.argv[1], 'rb') as f:
    data = [str(ord(c)) for c in f.read()]

# append null byte
data.append('0')

print('static const char TMP[] = {%s};' % ', '.join(data))
