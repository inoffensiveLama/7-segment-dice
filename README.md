# 7-segment-dice

WORK IN PROGRESS.

Its a small dice, that uses 7 segment displays. You can choose how many faces your dice should have. It will randomly roll a value and output it for you.

# Technologies used

It runs on an ESP32 C3. It uses 7 segment displays. There will be a custom PCB, you can order from you favourite PCB manufacturer (I usually go with JLCPCB). The 7 Segment displays, as well as the choosing of the correct dice is done via shift registers (one with serial in - parallel out and the other one with parallel in - serial out).