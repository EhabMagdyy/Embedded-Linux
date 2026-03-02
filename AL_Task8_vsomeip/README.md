### Problems
#### Stale socket file left in /tmp

If the server crashed, the socket file remains:
``` bash
ls -l /tmp/vsomeip-*
```
You might see:
``` bash
/tmp/vsomeip-0
```

but nothing is listening on it -> connection refused.

> Fix:
``` bash
sudo rm -f /tmp/vsomeip-*
```
Then restart server.