# hy457-Access_Control_System
@author George Kokolakis csd4254 (gkokol@ics.forth.gr)


Exercise 1 : fully implemented ✔️

ONE IMPORTANT implementation detail : 
In my implementation , im checking for each second! if the tracee
has executed the system calls more times than the allowed number 
using a thread , and im printing a suspect message.
(you said me that my solution is acceptable and not to change it). 
More : The thread im using fills with 0 the array that keeps how 
many times a system call is called every one second.

make tracee compiles trace.c program
make run    compiles ASC.c
I provide  test1.txt that shows the funcionality of the access 
control system. Comments in test1.txt explains exactly
how the acs should work.

run program with -->  ./a.out tests/test1.txt tracee

