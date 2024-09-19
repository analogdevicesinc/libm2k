#
# Copyright (c) 2024 Analog Devices Inc.
#
# This file is part of libm2k
# (see http://www.github.com/analogdevicesinc/libm2k).
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 2.1 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

#This example measures ambiental temperature with
#an AD22100 voltage temperature sensor, using the M2K.
#It requires these connections:
#M2K    Sensor
#V+   -> V+
#GND  -> GND
#1+   -> V0

#uri defaults to local
#to use other backends, pass the uri as command line argument

OFFSET=1.375 #voltage offset
TEMP_CT=0.0225 #AD221OO constant
ts=600 #time between measurements in seconds
valid=true
count=1
uri="local:"

if [ $# != 0 ] && [ "$1" != "start" ] #automounter uses start as parameter
then
    uri="$1"
fi

#change directory to the drive, when running on local context
if [ "$uri" = "local:" ]; then cd /media/sda1/; fi
#create output file
touch output.csv

sleep 5
#calibrate ADC
m2kcli analog-in $uri -C
#power sensor
m2kcli power-supply $uri --generate channel=0 value=5
while [ $valid ]
do
#read voltage
voltage=$(m2kcli analog-in $uri -v channel=0 raw=0 -q)
temp=$(echo "($voltage-$OFFSET)/$TEMP_CT" | bc)
#write result to output file
echo "$count. $temp °C" >> output.csv
echo "Reading one value every $ts seconds. CTRL+C to break"
sleep $ts
count=$((count+1))
done
