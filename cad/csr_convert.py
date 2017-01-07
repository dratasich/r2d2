#!/usr/bin/python
##
# Converts CSR blueprint values into custom R2D2.
#
# Conversion factor is defined by dome diameter. The original R2D2 has a dome
# diameter of 461.264mm.
##

import argparse

#
# CSR main measurements
#
csr_diameter = 461.264
csr_domeheight = 247.269

#
# config
#

desc = "Converts measurement from CSR blueprint (in inch) to custom R2D2 (in mm)."
parser = argparse.ArgumentParser(description=desc)
parser.add_argument('-d', '--diameter', type=float, default=360.0,
                    help="Dome diameter.")
parser.add_argument('-m', '--metric', type=bool, default=False,
                    help="Input measurement is given in mm (default is inch).")
parser.add_argument('measurement', type=float,
                    help="Measurement to convert.")
args = parser.parse_args()

#
# convert
#

if not args.metric:
    args.measurement = args.measurement * 25.4

res = args.measurement * args.diameter / csr_diameter

#
# output
#

print "%.2f" % res
