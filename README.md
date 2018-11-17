# Gbox420

<div>
<div><img width="96" height="96" src="https://sites.google.com/site/growboxguy/home/gbox420.png" style="display:inline;float:left" >
<a href="https://www.patreon.com/growboxguy"><img width="48" height="48" src="https://sites.google.com/site/growboxguy/home/patreon.png" style="border:none;margin:0;padding:0px"></a>
<a href="https://www.facebook.com/Gbox420-1495966037151416/"><img width="48" height="48" src="https://sites.google.com/site/growboxguy/home/if_facebook_1807546.png" style="border:none;margin:0;padding:0px"></a>
<a href="https://github.com/growboxguy/Gbox420"><img width="48" height="48" src="https://sites.google.com/site/growboxguy/home/if_github.png" style="border:none;margin:0;padding:0px"></a>
<a href="https://plus.google.com/106380858241253565250"><img width="48" height="48" src="https://sites.google.com/site/growboxguy/home/if_google__1807544.png" style="border:none;margin:0;padding:0px"></a>
<a href="https://www.tumblr.com/blog/growboxguy"><img width="48" height="48" src="https://sites.google.com/site/growboxguy/home/if_tumblr_986957.png" style="border:none;margin:0;padding:0px"></a>
<a href="https://twitter.com/growboxguy"><img width="48" height="48" src="https://sites.google.com/site/growboxguy/home/if_4_939755.png" style="border:none;margin:0;padding:0px"></a>
<a href="mailto:GrowBoxGuy@gmail.com"><img width="48" height="48" src="https://sites.google.com/site/growboxguy/home/if_Android-Gmail_72149%20%281%29.png" style="border:none;margin:0;padding:0px"></a>
</div>
<div style="display:inline!important">
<b style="font-size:16px">
<font color="#444444">&nbsp;Project made by </font>
<font color="#00ab44">GrowBoxGuy</font><br>		
&nbsp;<a href="http://sites.google.com/site/growboxguy/">sites.google.com/site/growboxguy/</a>
</b>
</div><br>
</div>

<div>
We build an Arduino based grow box monitoring and controlling system out of an old PC case with a power supply. The ATmega2560 microcontroller running the Gbox420 sketch is connected to an ESP8266 wifi chip with a custom ESP-Link firmware and reports sensor readings through PushingBox message API to Google Sheets. The ESP8266 wifi chip will act as an internet gateway and a web server at the same time, hosting a mobile friendly control and monitoring interface for the grow box.
</div>
<div>
<h2>Assembly guide: https://sites.google.com/site/growboxguy/arduino</h2>
</div>


![alt text](https://docs.google.com/drawings/d/saJcW0ASTVO8vFDMZu1nxyQ/image?w=896&h=417&rev=986&ac=1&parent=13WdrW3qaUhXpjld2XEMji3r6EkQRwYiImda7D3szmkA)

Website hosted by the ESP link, displaying sensor readings from Arduino and controlling the grow box
![alt text](https://lh6.googleusercontent.com/Q7CviRdc5dGbD8tyHI5lj1Z9tSE1Hl1tNV6eScdNlth4r2hYlj1uI-Kdv85KZcW4rLrM1pkxoVkLM3QKZe0QCQsP8f5jktpEer3btAU9ZXpUj2vGMS8KfEoqHiDXt28InjmjQrK3)

Google Sheets integrated into the grow box interface:
![alt text](https://lh3.googleusercontent.com/41GrczKaTNOsarf77zMgRfFIXO0N6_M5OUkGF6ExIX2rAOiUb2TsIqysGyW5Wgh4zuC2HWapsesOLbXWYjlrfgbsEbyCvl771q10y_Fa7Fm9chMcJQheiQtlYBmARe-N49YC4Ujh)

![alt text](https://lh3.googleusercontent.com/GYfuFEZYxv5bDz5c3xNUVWoYVUGvm7BrU0dTOZW3aT5HbDEg0XzhBQun3GnablQhYy24-9FeZQNwcu0NIn7_NnKEgtnC1NQ315UYyHmVLf3CL267afzK913bABHSn6Ym1bKlC4TW)

Inside the PC case: Arduino, power meter, led driver and dimming circuit, 8 relays for controlling lights/fans and pumps
![alt text](https://lh3.googleusercontent.com/BEJLnPGh5m7ayeMAGi42AZ67JkaiOU0_59zkJpUlvnXy1n1U1YEZpEUPduRhplLtXshTBAq42mLORqmNPgFvO-C8O9eElPmEN8FRaZPhtGOkoEtiiq6UptZxyeFbR2k3dKeF35cc)

External light, humidity and temperature sensors connected over UTP cable
![alt text](https://lh3.googleusercontent.com/RLd_GoQMXbY5050mcR9KNx0QPErBM82_zoO1bI0QXvM9aEZs-W4VhJLuANACFuNkh4AJ44UfSpaVPoQbqDHBF1N5JQQuiBkkZO3jxBrlkcyDmmMHLocpfxEQKFOYy52wV3c6QkXO)

Arduino controlling the LED COB lights
![alt text](https://lh3.googleusercontent.com/uFSBH5TT2EaEKr6cV1UqM_5PfG4ATpqilXDMEZxTxx3Qy18JCmlVoFfRVkALXTYOna-tuDUivfka7HD7I4TUDxPXBqD4W7aEM1B-DotJYhE2WEu-JrcyWCliqsgyKGTBHEB5h933)

</div>
