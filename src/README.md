
*WSS* is short of **W**orking **S**et **S**ize

**SampledWSS.cpp**  Uniform sampling, sample *1* access every *n* accesses

**RandomSampledWSS.cpp** 

## Output Formation of Sampled Working Set Size

The output file is a sequence of WSS of continous windows. 

The element of the sequence is either a pair *(wss, num)* or a single nagtive number *-wss*. 

The first case means the WSSs of following *num* windows are *wss*. The single negative number *-wss* means the WSS of a single number is *wss*.
