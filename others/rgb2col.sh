#!/bin/sh
#	Christopher Alexander North-Keys
#	Tue Jun  7 02:07:11 MET DST 1994

cat $* \
	| awk '/^[^#!\t]+/ \
		{
			r = $1;
			g = $2;
			b = $3;
			name = ""
			for(i = 4 ; i <= NF ; ++i)
			{
				if(i < NF)
					name = name $i " "
				else
					name = name $i
			}
			printf("%s\t\tRGB:%02x/%02x/%02x\n", name, r, g, b);
		}

#---------eof


	
