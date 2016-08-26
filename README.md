# T.I.A.S - Tom's Image Analysis System

### What does TIAS do?
TIAS was built with the objective of aiding Forensic examiners in the identification of indecent images of children. The algorithm is crude, its crudeness eclipsed only by the shocking quality of the code. I'm not a professional coder and I fear that my basic coding knowledge did not give this noble project the attention it deserved, thus it is being released to the opensource community! Any help with the project will be very much appreciated - Tom

TIAS will currently load in a directory containing images. Each image is analysed, the results stored in an SQLite database. As the primary focus is the detection of indecent images, known indecent images (MD5 blacklisting) are displayed first followed by those identified by my algorithm. 

### The algorithm
TIAS was built to showcase my proposed algorithm. The algorithm relies on the detection of a the face. From the face a predicted body can be estimated. The number of skin coloured pixels with the predicted body are counted and if this figure is above a defined threshold, the image is said to contain nudity. The algorithm also incorporates Canny line detection and proportion analysis to reject false positives. 



![TIAS](https://tombu.co.uk/wp-content/uploads/2016/07/TiaPing-1024x546.png)
