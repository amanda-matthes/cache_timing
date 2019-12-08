# Cache-Timing

In this simple demo I show how one can indirectly access memory content by timing memory accesses.

## Demo output



## Things I learned...


### ... when I was trying to create a value that is actually secret
- Private class members are not really private at all if you know their memory location (which turns out to be right next to the public ones)
- VirtualProtect is an easy way to make memory actually private, but it will make all pages containing protected bytes private. This can cause very weird bugs. For example...

### ... when I was trying to recover from a Segmentation Fault after using a value that was actually secret
- There is no (easy) way to somehow "catch" a seg fault. It will end your program
- It is possible to share memory between processes using boost::interprocess

### ... from the whole project
- Compilers are really good at optimising code and are hard to predict without compiler knowledge
- Making a variable volatile does not protect from all kinds of optimisation
- Computers are weird and I love it
