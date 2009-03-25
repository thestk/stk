/*******  Pitch and amplitude Test Loops   ********/
    
/*    for (j=0;j<12;j++)  {
        printf("%lf\n",55.0 * pow(1.414,j));
        instrument.noteOn(55.0 * pow(1.414,j),0.1);
        for (i=0;i<15000;i++) output.tick(instrument.tick());
        instrument.noteOff(0.1);
        for (i=0;i<5000;i++) output.tick(instrument.tick());
        instrument.noteOn(55.0 * pow(1.414,j),0.4);
        for (i=0;i<15000;i++) output.tick(instrument.tick());
        instrument.noteOff(0.1);
        for (i=0;i<5000;i++) output.tick(instrument.tick());
        instrument.noteOn(55.0 * pow(1.414,j),0.7);
        for (i=0;i<15000;i++) output.tick(instrument.tick());
        instrument.noteOff(0.1);
        for (i=0;i<5000;i++) output.tick(instrument.tick());
        instrument.noteOn(55.0 * pow(1.414,j),1.0);
        for (i=0;i<15000;i++) output.tick(instrument.tick());
        instrument.noteOff(0.1);
        for (i=0;i<5000;i++) output.tick(instrument.tick());
    }
*/
    
/************  Controller Test Loops  ***************/
    for (j=0;j<5;j++)  {
        instrument.noteOn(440,1.0);
        instrument.controlChange(MOD_WHEEL,3 + (j*31));
        for (i=0;i<20000;i++) {
            output.tick(instrument.tick());
        }
    }
    for (j=0;j<5;j++)  {
        instrument.noteOn(440,1.0);
        instrument.controlChange(CONTROL3,3 + (j*31));
        for (i=0;i<20000;i++) {
            output.tick(instrument.tick());
        }
    }
    instrument.controlChange(CONTROL3,64);
    instrument.controlChange(MOD_WHEEL,0);
    for (j=0;j<5;j++)  {
        instrument.controlChange(CONTROL1,3 + (j*31));
        instrument.noteOn(440,1.0);			/* some need it before, some after */
        instrument.controlChange(CONTROL1,3 + (j*31));
        for (i=0;i<20000;i++) {
            output.tick(instrument.tick());
        }
    }
    instrument.controlChange(CONTROL1,64);
    for (j=0;j<5;j++)  {
        instrument.controlChange(CONTROL2,3 + (j*31));
        instrument.noteOn(440,1.0);			/* some need it before, some after */
        instrument.controlChange(CONTROL2,3 + (j*31));
        for (i=0;i<20000;i++) {
            output.tick(instrument.tick());
        }
    }
    instrument.controlChange(CONTROL2,64);


