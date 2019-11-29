# intel_brightness
Very simple replacement of xbacklight, which didn't work for me.

But writing value to `/sys/class/backlight/intel_backlight/brightness` works. To make this more interesting I wrote
this app without using `libc`. So it's really minimal.
