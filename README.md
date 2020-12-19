# Page Replacement Algo Simulator

Authors:   Shaurya Kethireddy

Email:       skethireddy@wisc.edu

CS Login: shaurya

This program is a page replacement simulator which handles the fifo, lru, clock algorithms.
## Page replacemnt algorthims
There is one major function, page replacement, and two others for memory freeing and accesining memory. Note that not all modules will use the other two functions.
### clock module
This module handles all the necessary actions needed to run using the clock algorithm.
### fifo module
This module handles all the necessary actions needed to run using the fifo algorithm.
### lru module
This module handles all the necessary actions needed to run using the lru algorithm.
### minopt module
This module handles all the necessary actions needed to run using the minopt algorithm.

stats for 12mil

```
AMU: 0.964976
ARP: 0.001181
TMR: 12000000
TPI: 5083
RTime: 10166178048

Time elapsed: 2024.04663
```
## memoryTree module
This module handles the hash tree data storage. This is only used for the the TLB and utlizes the tsearch/tfind/tdelete functions.
## queue module
This module is a generic double linked list queue typed data structure. We choose a double linked list for non linear deletions for the LRU algorithm. It holds a data void* for generic use. Overall, the disk, process lookup, and physical memory use it, as well as some other places.
## reader module
This module parses through the cmd line and sets the initial variables and structs, as well as being able to read a certain line by an arg index. The read line function uses a cache implenatation to store the next 5 (or however defined) lines. This greatly reduces the fseek calls and improved our running time substantially. The cache was not completly used throughout the program due to use running out of time.
## runner module
This module runs the whole program and cotains all the logic for flow.
## stats module 
This file tracks all the stat info for the program and prints it at the end.