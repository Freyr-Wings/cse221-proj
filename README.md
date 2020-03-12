# cse221-proj

/cpu/ctxswitch.c - Measure context switch overhead
<br>
/cpu/fork.c - Measure fork overhead
<br>
/cpu/measure.c - Measure time measurement (rdtscp) overhead
<br>
/cpu/procedurecall.c - Measure loop overhead (use parameter -1) & procedure call overhead
<br>
/cpu/syscall.c - Measure system call overhead
<br>
/cpu/thread.c - Measure thread creating overhead

/filesystem/contention.c - Measure disk access time with multiple threads
<br>
/filesystem/filecache.c - Measure the speed of reading different size of file in order to know size of filecache
<br>
/filesystem/readtime.c - Measure sequential and random read of different size of files
<br>
Programes in this folder need to be run after creating testfiles and clearing system file cache.


/memory/bandwidth.c - Measure the memory read/write bandwidth
<br>
/memory/latency.c - Measure the memory latency within different size of memories
<br>
/memory/pagefault.c - Measure the time of pagefault


/network/bandwidth_server(client).c - Measure network bandwidth
<br>
/network/connection_server(client).c - Measure the overhead of connect and close
<br>
/network/rtt_server(client).c - Measure the round-trip time of network
