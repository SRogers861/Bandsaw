# Bandsaw
HM130 Automation

Line 23 holds Manual Height float variable

Line 35 is a large gap for Manual Height Array

Menu Page 6 (line 5093) holds the Manual Height keypad (4 rows, 4 columns)
{1,2,3,Readout}
{4,5,6,"inches"}
{7,8,9,Null}
{.,0,Done Button,Back Button}

The manHeight float variable will need to be 6 digits including decimal
  example: 1.0625, or 1.5000
  
From what I've gathered about arrays, there needs to be some way to signify that you don't want to add any more inputs. The done button which is pressed on line 5162 can do this.

I realize there are plenty of areas where I can do other switches (line 4740), but I am a bit nervous to do that until I get the keypad working.

