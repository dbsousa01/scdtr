clear; close all; clc;

values = [38.00;
38.00;
39.00;
39.00;
38.00;
38.00;
39.00;
38.00;
38.00;
39.00;
39.00;
39.00;
38.00;
39.00;
40.00;
39.00;
39.00;
39.00;
40.00;
39.00;
40.00;
40.00;
40.00;
40.00;
39.00;
40.00;
40.00;
40.00;
40.00;
40.00;
40.00;
40.00;
40.00;
40.00;
40.00;
40.00;
40.00;
40.00;
40.00;
40.00;
40.00;
39.00;
40.00;
40.00;
40.00;
40.00;
40.00;
39.00;
40.00;
40.00;
41.00;
73.00;
136.00;
204.00;
273.00;
337.00;
389.00;
431.00;
467.00;
496.00;
519.00;
536.00;
550.00;
563.00;
572.00;
578.00;
585.00;
592.00;
596.00;
598.00;
601.00;
605.00;
607.00;
608.00;
611.00;
614.00;
615.00;
616.00;
617.00;
619.00;
620.00;
619.00;
621.00;
622.00;
623.00;
623.00;
623.00;
626.00;
626.00;
625.00;
626.00;
628.00;
628.00;
628.00;
628.00;
629.00;
630.00;
629.00;
630.00;
631.00;
631.00;
630.00;
631.00;
632.00;
630.00;
631.00;
632.00;
633.00;
632.00;
632.00;
633.00;
633.00;
632.00;
633.00;
634.00;
634.00;
634.00;
633.00;
634.00;
635.00;
634.00;
633.00;
634.00;
635.00;
635.00;
635.00;
635.00;
636.00;
635.00;
635.00;
636.00;
636.00;
635.00;
635.00;
636.00;
637.00;
636.00;
636.00;
637.00;
637.00;
636.00;
635.00;
636.00;
637.00;
636.00;
636.00;
637.00;
637.00;
636.00;
636.00;
638.00;
637.00;
636.00;
636.00;
638.00;
638.00;
637.00;
637.00;
638.00;
638.00;
637.00;
637.00;
638.00;
638.00;
637.00;
637.00;
639.00;
639.00;
637.00;
637.00;
638.00;
638.00;
637.00;
637.00;
639.00;
639.00;
638.00;
638.00;
639.00;
639.00;
637.00;
637.00;
639.00;
639.00;
638.00;
639.00;
639.00;
639.00;
638.00;
638.00;
638.00;
639.00;
638.00;
639.00;
640.00;
640.00;
638.00;
638.00;
639.00;
639.00;
638.00;
638.00;
639.00;
640.00;
639.00;
639.00;
639.00;
640.00;
638.00;
638.00;
639.00;
639.00;
639.00;
639.00;
639.00;
640.00;
639.00;
638.00;
640.00;
640.00;
639.00;
639.00;
640.00;
640.00;
639.00;
639.00;
641.00;
640.00;
639.00;
638.00;
639.00;
640.00;
640.00;
639.00;
640.00;
641.00;
639.00;
639.00;
639.00;
640.00;
639.00;
639.00;
640.00;
640.00;
640.00;
639.00;
640.00;
640.00;
639.00;
639.00;
640.00;
640.00;
639.00;
639.00;
640.00;
640.00;
639.00;
639.00;
640.00;
641.00;
639.00;
639.00;
641.00;
640.00;
640.00;
639.00;
641.00;
640.00;
639.00;
639.00;
640.00;
641.00;
640.00;
640.00;
641.00;
641.00;
639.00;
639.00;
640.00;
640.00;
639.00;
639.00;
641.00;
641.00;
639.00;
639.00;
641.00;
640.00;
640.00;
639.00;
640.00;
641.00;
640.00;
640.00;
641.00;
641.00;
639.00;
639.00;
640.00;
640.00;
640.00;
639.00;
641.00;
641.00;
639.00;
639.00;
641.00;
641.00;
639.00;
639.00;
641.00;
641.00;
640.00;
639.00;
640.00;
641.00;
639.00;
639.00;
641.00;
641.00;
640.00;
640.00;
641.00;
641.00;
639.00;
640.00;
641.00;
641.00;
640.00;
640.00;
641.00;
641.00;
640.00;
640.00;
641.00;
641.00;
640.00;
639.00;
641.00;
641.00;
640.00;
639.00;
642.00;
641.00;
640.00;
639.00;
640.00;
641.00;
640.00;
640.00;
641.00;
641.00;
640.00;
640.00;
641.00;
641.00;
640.00;
641.00;
641.00;
640.00;
640.00;
641.00;
641.00;
640.00;
639.00;
640.00;
641.00;
640.00;
640.00;
640.00;
642.00;
641.00;
640.00;
641.00;
641.00;
641.00;
639.00;
640.00;
641.00;
641.00;
640.00;
640.00;
641.00;
641.00;
640.00;
640.00;
640.00;
641.00;
640.00;
640.00;
641.00;
641.00;
640.00;
640.00;
641.00;
640.00;
640.00;
641.00;
642.00;
640.00;
641.00;
641.00;
641.00;
640.00;
640.00;
641.00;
641.00;
640.00;
640.00;
641.00;
642.00;
641.00;
640.00;
642.00;
641.00;
640.00;
639.00;
641.00;
641.00;
641.00;
640.00;
642.00;
642.00;
640.00;
640.00;
641.00;
641.00;
641.00;
640.00;
642.00;
642.00;
641.00;
640.00;
641.00;
641.00;
641.00;
639.00;
641.00;
642.00;
641.00;
640.00;
642.00;
641.00;
640.00;
639.00;
641.00;
641.00;
641.00;
641.00;
642.00;
641.00;
640.00;
641.00;
642.00;
641.00;
640.00;
640.00;
642.00;
641.00;
640.00;
640.00;
641.00;
641.00;
640.00;
640.00;
642.00;
641.00;
640.00;
641.00;
642.00;
641.00;
640.00;
641.00;
642.00;
641.00;
640.00;
640.00;
642.00;
642.00;
640.00;
640.00;
642.00;
641.00;
640.00;
640.00;
642.00;
642.00;
641.00;
641.00;
642.00;
642.00;
640.00;
639.00;
641.00;
641.00;
640.00;
640.00;
642.00;
642.00;
640.00;
];

AR = values;

PWM = zeros(500,1);

R1 = 10;
m = -0.692;
b = 1.808;

f = R1*(5-((AR.*5)./(1023)))./((AR.*5)./(1023));
lux = 10.^((log10(f)-b)/m);

PWM(51:500) = 128;

t = linspace(0,1e6,length(PWM))';

figure
plot(t,PWM)
title('PWM step input')
figure
plot(t,AR)
title('Values from Arduino Analog Read')

figure
plot(t,lux)
title('Values LUX')
