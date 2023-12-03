### Performance/area protocol

| Resource name | VGA_raster              |  hierarchi_part       | write_pixle       | all components    |
|:--------------|:-----------------------:|:---------------------:|:-----------------:|:-----------------:|
|Frequency (MHz)|-                        |-                      | -                 | 1xcloxk fmax 93.9 |
|Quartus Fit Resource Utilization||||
|LCs            | 1027                    | 1168                  | 478               | 2673              |
|FFs            | 668                     | 481                   | 199               | 1348              |
|RAMs           | 17                      | 1                     | 0                 | 18                |
|DSPs           |	0                       | 24                    | 12                | 36                |
|Estimated Resource Usage|||
|ALUTs          |	1008                    | 1396                  | 740               | 3144(8%)          |
|FFs            | 638                     | 569                   | 290               | 1497(3%)          |
|RAMs           | 13                      | 0                     | 0                 | 13(7%)            |
|DSPs           | 0                       | 6                     | 3                 | 9(6%)             |
|Loops analysis||||
|pipeline status| Yes                     | Yes                   | Yes               | Yes               |
|II             | >=1 and ~1              | >=1 and ~1            | ~1                | -                 |
|bottleneck(s)  | n/a                     | n/a                   | n/a               | n/a               |
