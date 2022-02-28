# One-Line-Communication
This is an experiment for transmitting bits on one line. The protocol is a simplified version of UART. One byte is transmitted once with a start bit and a stop bit. 
Two Arduino UNO boards, named as A and B, are connected by one line from A's digital pin 2 to B's digital pin 3. Program "one_line_A" and Program "one_line_B" are uploaded to board A and board B, respectively. After uploading the programs, open two serial-port monitors, one for A and the other for B.
The default baud rate is 115200 bps and each line must be appended with a NL (new-line) character. Input a string on A's monitor and push send button, then each character byte of this string will be sent to board B.
These characters will then shown on B's monitor in one by one mannar. The default bit interval is 100 ms and the default GPIO pins are 2 and 3 for board A and board B, repectively. All of them can be modified in header file "common.h". 
