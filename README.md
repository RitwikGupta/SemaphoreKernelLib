Semaphore Library with Producer Consumer Problem
=================================================

This project implements semaphores as part of the Linux kernel (2.6.23.1), with up and down actions exposed as syscalls. The producer-consumer problem is then solved using the implemented semaphore library.

1. Compile the Linux source 2.6.23.1 with the provided files replacing the ones that come bundled with the Linux source. Email me for help with that!
1. Run `make`
1. Run `./prodcons <num_producers> <num_consumers> <size_of_buffer>`
1. Press `C-c` to end execution!