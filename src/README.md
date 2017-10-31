
*WSS* is short of **W**orking **S**et **S**ize

**SampledWSS.cpp**  Uniform sampling, sample *1* access every *n* accesses

**RandomSampledWSS.cpp** 

## Output Formation of Sampled Working Set Size

The output file is a sequence of WSS of continous windows. The sequence is composed of pairs *(wss, num, nsampled)*, meaning there are *num* continous windows, which their working set size are *wss* and excatly *nsampled* accesses are sampled in each window.

## TODO

Change the type of variable *ws* in *USamplerWSS.cpp* to **int64_t**