# IRReadOnlyRemote
A Read Only Arduino library for receiving IR remote control codes

Receive and store IR frames from IR remote controls on an Arduino using the NEC style protocol.
This library is suitable for projects that only need to receive (robots?), no sending required.
No timer interrupts are used, so it will not interfere with other libraries. Only one hardware interrupt is used.
The requirements of code and memory space are low, and the processing needs are very low.

The decoded IR frames are stored in a circular buffer that can store from 1 to 127 frames.
The default is 15 frames, change IR_BUF_SIZE for other sizes (size must be a power of 2). The capacity
of the buffer is one less than the defined size.

There is no need to retrieve the frames from the buffer as soon as they arrive. This enables the main program
to read the frames when it has time, and is not constrained by a requirement to read the frames as soon as they arrive
(as long as the buffer does not overflow) in order to not lose any frames.

