
#!/bin/bash
VIDEO_OPTS="-s 854x480 -c:v libx264 -b:v 800000"
OUTPUT_HLS="-hls_time 10 -hls_list_size 10 -start_number 1"
ffmpeg -i udp://1.52.96.119:6786$VIDEO_OPTS $OUTPUT_HLS mystream.m3u8

