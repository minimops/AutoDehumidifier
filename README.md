# Automatic Dehumidifier

Just a small DIY project that makes our Dehumidifier in the Bathroom automatic.
The code is sorta work in progress / never really completed. It does however work sufficiently enough.
Its based on an arduino nano and turns humidifiers like *missing* on and off based on an upper & lower bound principle with an additional timer functions: - it runs for at least X mins when it hits the defined upper bound. - it stops (and throws an error) when it runs for too long and doesn't reach the lower bound. - It doesnt run at night.
