# cse221-proj

/cpu/ctxswitch.c - Measure context switch overhead

/cpu/fork.c - Measure fork overhead

/cpu/measure.c - Measure time measurement (rdtscp) overhead

/cpu/procedurecall.c - Measure loop overhead (use parameter -1) & procedure call overhead

/cpu/syscall.c - Measure system call overhead

/cpu/thread.c - Measure thread creating overhead


/filesystem/contention.c - Measure disk access time with multiple threads

/filesystem/filecache.c - Measure the speed of reading different size of file in order to know size of filecache

/filesystem/readtime.c - Measure sequential and random read of different size of files

Programes in this folder need to be run after creating testfiles and clearing system file cache.


/memory/bandwidth.c - Measure the memory read/write bandwidth

/memory/latency.c - Measure the memory latency within different size of memories

/memory/pagefault.c - Measure the time of pagefault


/network/bandwidth_server(client).c - Measure network bandwidth

/network/connection_server(client).c - Measure the overhead of connect and close

/network/rtt_server(client).c - Measure the round-trip time of network
