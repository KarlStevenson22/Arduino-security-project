This code should be able to convert increments into angles for ease of use.

It also has a soft reset functionality for 3 layers of security and initialisation routine for 
when the device is at rest i.e. it zeros the x and y values.

When soft reset is pressed the white LED should switch on indicating that the device is initialising.

The white LED switches off at the end of initialisation.

Z values are converted to G.

Pins are a bit different for this.

"gravi" is currently based on the mg/LSD of the accelerometer, but you can convert that to 9.8/avg_z.

I totally have enough willpower to stop coding.