# Cache-Timing

In this simple demo I show how one can indirectly access memory content by timing memory accesses.

## Demo output

    --------------------------------
    START
    --------------------------------


    -----------------------------------------------------
    Playing around with the cache

    First, I created a large array of size 131072 and
    filled it with random integers. Let's check out an
    element somewhere in the middle of my big array and
    see how long it takes:
    array[50000] = 1527
    This took 6.03 time units

    Now, we expect this part of the array to be cached.
    So if we access it again it should be quicker:
    array[50000] = 1527
    This took 3.9 time units

    So the difference is there but not always. Try re-
    running this a couple of times. I found that the
    second access is on average 3.5x faster. We can see
    that the effects of caching are definitely noticeable        
    but quite hard to predict and reproduce.

    -----------------------------------------------------
    Is this good enough to infer information by memory
    access timing?

    To answer this question, let's first create a simple
    secret...
    Created random secret number between 0 and 10.

    We should also make sure that nothing of interest is
    cached. To do that, we can simply do some calculations       
    with large arrays that will require lots of
    cache space...
    Flushed the cache.

    First, we call array[(secret+1)*81292] (The +1 is
    there because array[0] is almost always cached). We
    will never look at secret directly but we can
    reconstruct it, because we know that it will be in
    the cache.
    In this simple example we know that secret is between        
    0 and 9 so we can try all the possibilities:

    Accessing array[(0+1)*8192] took         6.55 time units     
    Accessing array[(1+1)*8192] took    2.08e-009 time units     
    Accessing array[(2+1)*8192] took    1.46e-008 time units     
    Accessing array[(3+1)*8192] took    1.04e-009 time units     
    Accessing array[(4+1)*8192] took    2.08e-009 time units     
    Accessing array[(5+1)*8192] took    1.46e-008 time units     
    Accessing array[(6+1)*8192] took    1.04e-009 time units     
    Accessing array[(7+1)*8192] took    1.04e-009 time units     
    Accessing array[(8+1)*8192] took    1.98e-008 time units     
    Accessing array[(9+1)*8192] took    1.46e-008 time units     

    A shorter time means that it was most likely in the
    cache and we expect array[(secret+1)*81292] to be
    in the cache.
    So the best guess for the secret value this round is: 3      

    Now we can repeat this a couple more times to make it        
    significant.
    Best guesses: 3 3 5 3 1 3 3 3 3 7 2 3 3 1 3 3 3 0 5 3 3 3 2 3 3 1 3 3 3 5

    The best guess overall is : 3
    The real secret value is  : 3
    Yay!

    I found that this works over 95% of the time.
    -----------------------------------------------------        


    --------------------------------
    DONE
    --------------------------------

## What this repo contains
- The "main" file that will produce the output above
- The "timing" header that is used to do the timing in main
- Other code that I did not end up using directly but is somewhat relevant:
    - "howToProtectMemory" is a simple demo of how to use "VirtualProtect" to make certain memory pages protected. I tried to use this to more closely mimic an actual exploit
    - "howToSharedMemory" shows how the boost library "interprocess" can be used to create virtual memory that can be shared across processes. I thought that I could use that to put the memory accesses in a separate process so that I could regain control after a segmentation fault
    - "howToAttemptAccess" and "attempt_access" show how this subprocess idea can be implemented. I did not end up using this approach in the main demo because it is much harder to control the cache if it is used from several processes

## Can this be used to read protected memory?
If there is no proper separation of virtual memory spaces and you have a rough idea of where the interesting stuff is, yes. But it has become much harder on recent machines.

## Things I learned...

### ... when I was trying to create a value that is actually secret
- Private class members are not really private at all if you know their memory location (which turns out to be right next to the public ones)
- VirtualProtect is an easy way to make memory actually private, but it will make all pages containing affected bytes private. This can cause very weird bugs. For example, the page could also contain the pointer that you used to make that memory protected. So now, you cannot use the pointer to make it accessible again.

### ... when I was trying to recover from a Segmentation Fault after using a value that was actually secret
- There is no (easy) way to somehow "catch" a seg fault. It will end your program
- It is possible to share memory between processes using boost::interprocess
- That way you can contain the memory access in a secondary process that can crash in case of a segmentation fault without losing the virtual memory

### ... when I was trying to time how long accessing a value takes
- All the "standard" ways to time things in C++ (ctime, chrono etc.) are too imprecise for this purpose. Instead, I ended up using the QueryPerformanceCounter which is less intuitive to use but very useful
- Sometimes the only way to force the computer to access some value is to print it

### ... from the whole project
- Compilers are really good at optimising code and are hard to predict
- Caching behaviour is not easily predictable or reproducible
- Making a variable volatile does not protect from all kinds of optimisation
- Computers are weird and I love it
